// ===================================================================================
// Project:   Tiny Lander - Conversion for CH32V003
// Version:   v1.0
// Year:      2023
// ===================================================================================
//
//   >>>>>  T-I-N-Y  L-A-N-D-E-R v1.0 for ATTINY85  GPLv3 <<<<
//              Programmer: (c) Roger Buehler 2020
//              Contact EMAIL: tscha70@gmail.com
//        Official repository:  https://github.com/tscha70/
//  Tiny Lander v1.0 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//     
//  This game uses features or part of code created by 
//  Daniel C (Electro L.I.B) https://www.tinyjoypad.com under GPLv3
//  to work with tinyjoypad game console's standard.
//
// ===================================================================================
// Modified to work with CH32V003 by Stefan Wagner: https://github.com/wagiminator
// ===================================================================================

#include "../common/driver.h"
#include "spritebank.h"


// ===================================================================================
// Function Prototypes
// ===================================================================================
void initGame(GAME * game);
void showAllScoresAndBonuses(GAME *game, DIGITAL *score, DIGITAL *velX, DIGITAL *velY);
void changeSpeed(GAME * game);
void moveShip(GAME * game);
void fillData(long myValue, DIGITAL * data);
void SetLandingMap(uint8_t level, GAME *game);
uint8_t ScoreDisplay(uint8_t x, uint8_t y, DIGITAL * score);
uint8_t VelocityDisplay(uint8_t x, uint8_t y, DIGITAL * velocity, uint8_t horizontal);
uint8_t DashboardDisplay(uint8_t x, uint8_t y, GAME * game);
uint8_t LanderDisplay(uint8_t x, uint8_t y, GAME * game);
uint8_t getLanderSprite(uint8_t x, uint8_t y, GAME * game);
uint8_t FuelDisplay(uint8_t x, uint8_t y, GAME * game);
uint8_t GameDisplay(uint8_t x, uint8_t y, GAME * game);
uint8_t StarsDisplay(uint8_t x, uint8_t y, GAME * game);
uint8_t LivesDisplay(uint8_t x, uint8_t y, GAME * game);
static void Tiny_Flip(uint8_t mode, GAME * game, DIGITAL * score, DIGITAL * velX, DIGITAL * velY);

void INTROJOY_sound(void);
void VICTORYJOY_sound(void);
void ALERTJOY_sound(void);
void HAPPYJOY_sound(void);
void SPLITDIGITS(uint16_t val, uint8_t *digits);
uint8_t GETLANDSCAPE(uint8_t x, uint8_t y, uint8_t level, GAME *game);
void SETNEXTLEVEL(uint8_t level, GAME *game);

// ===================================================================================
// Main Function
// ===================================================================================
int tiny_lander_run(void) {
  // Setup
  JOY_init();
  //JOY_OLED_fill(0x00);

  // Loop
  while(1) {
    DIGITAL score;
    DIGITAL velX;
    DIGITAL velY;
    GAME game;

  BEGIN:
    game.Level = 1;
    game.Score = 0;
    game.Lives = 4;
    while(1) {
      Tiny_Flip(1, &game, &score, &velX, &velY);
      if (JOY_act_pressed()) {
        if (JOY_up_pressed()){ 
          game.Level = 10;
          ALERTJOY_sound();
        }
        else if (JOY_down_pressed()) {
          game.Lives = 255;
          ALERTJOY_sound();
        }
        else {
          JOY_sound(100, 125);
          JOY_sound(50, 125);
        }

        goto START;
      }
      JOY_idle();
    }

  START:
    initGame(&game);
    INTROJOY_sound();
    while(1) {
      fillData(game.Score, &score);
      fillData(game.velocityX, &velX);
      fillData(game.velocityY, &velY);
      moveShip(&game);
      changeSpeed(&game);

      Tiny_Flip(0, &game, &score, &velX, &velY);
      if (game.EndCounter > 8) {
        if (game.HasLanded)
        {
          showAllScoresAndBonuses(&game, &score, &velX, &velY);
          JOY_DLY_ms(500);
          goto START;
        }
        else
        {
          JOY_DLY_ms (2000);
          if (game.Lives > 0)
            goto START;
          goto BEGIN;
        }

      }
      if (game.ShipExplode > 0 || game.Collision)
        game.EndCounter++;
      if (game.HasLanded)
        game.EndCounter = 10;
      JOY_SLOWDOWN();

      JOY_idle();
    }
  }
}

// ===================================================================================
// Functions
// ===================================================================================
void initGame(GAME * game)
{
  SETNEXTLEVEL(game->Level, game);

  game->velocityY = 0;
  game->velocityX = 0;
  game->velXCounter = 0;
  game->velYCounter = 0;
  game->ShipExplode = 0;
  game->Toggle = true;
  game->Collision = false;
  game->HasLanded = false;
  game->EndCounter = 0;
  game->Stars = 0;
}

void showAllScoresAndBonuses(GAME *game, DIGITAL *score, DIGITAL *velX, DIGITAL *velY)
{
  VICTORYJOY_sound();
  game->Level++;
  JOY_DLY_ms (1000);
  uint8_t bonusPoints = 0;

  // add bonus points
  if ((abs(game->velocityY)) <= BONUSSPEED2)
    bonusPoints++;
  if ((abs(game->velocityY)) <= BONUSSPEED1)
    bonusPoints++;
  if (game->Fuel >= game->FuelBonus)
    bonusPoints++;

  for (game->Stars = 1; game->Stars <= bonusPoints; game->Stars++)
  {
    Tiny_Flip(2, game, score, velX, velY);
    HAPPYJOY_sound();
    JOY_DLY_ms(500);
  }
  game->Stars--;

  uint16_t newScore = game->Score + game->LevelScore  + (game->LevelScore * bonusPoints );
  while (game->Score < newScore)
  {
    game->Score++;
    fillData(game->Score, score);
    Tiny_Flip(2, game, score, velX, velY);
    JOY_sound(129, 2);
  }
}

void changeSpeed(GAME * game)
{
  game->ThrustLEFT = JOY_right_pressed();
  game->ThrustRIGHT = JOY_left_pressed();
  game->ThrustUP = JOY_act_pressed();
  game->Toggle = !game->Toggle;

  if (game->ThrustLEFT && game->Fuel > 0)
  {
    game->Fuel -= (FULLTHRUST / 2);
    game->velocityX += TrustX;
    if ((game->velocityX) > VLimit)
      game->velocityX  = VLimit;
  }
  else if (game->ThrustRIGHT && game->Fuel > 0)
  {
    game->Fuel -= (FULLTHRUST / 2);
    game->velocityX -= TrustX;
    if ((game->velocityX) < -VLimit)
      game->velocityX  = -VLimit;
  }

  if (game->ThrustUP && game->Fuel > 0)
  {
    game->Fuel -= (FULLTHRUST * 2);
    game->velocityY += TrustY;
    if ((game->velocityY) > VLimit)
      game->velocityY  = VLimit;
  }
  else
  {
    game->velocityY -= (GRAVITYDECY);
    if ((game->velocityY) < -VLimit)
      game->velocityY  = -VLimit;
  }

  if ((game->Fuel) <= 0)
    game->Fuel = 0;
}

void moveShip(GAME * game)
{
  if (game->ShipExplode > 0 || game->Collision || game->HasLanded) return;

  game->velXCounter += abs(game->velocityX);
  game->velYCounter += abs(game->velocityY);

  if ((game->velXCounter) >= MoveX) {
    game->velXCounter = 0;
    if ((game->velocityX) > 0)
      game->ShipPosX += 1;
    if ((game->velocityX) < 0)
      game->ShipPosX -= 1;
  }

  if (game->velYCounter >= MoveY) {
    uint8_t inc = (abs(game->velocityY) / ACCELERATOR) + 1;
    (game->velYCounter) = 0;
    if ((game->velocityY) > 0)
      game->ShipPosY -= inc;
    if (game->velocityY < 0)
      game->ShipPosY += inc;
  }

  // boundaries....
  if (game->ShipPosX > 121)
  {
    game->ShipPosX = 121;
  }
  else if (game->ShipPosX < 23)
  {
    game->ShipPosX = 23;
  }
  if (game->ShipPosY > 55)
  {
    game->ShipPosY = 55;
  }
}

void fillData(long myValue, DIGITAL * data)
{
  SPLITDIGITS(abs(myValue), data->D);
  data->IsNegative = (myValue < 0);
}

uint8_t ScoreDisplay(uint8_t x, uint8_t y, DIGITAL * score) {
  // show score within the give limits on lin 1
  if  ((y != 1) || (x < SCOREOFFSET) || (x > (SCOREOFFSET + (SCOREDIGITS * DIGITSIZE) - 1))) {
    return 0;
  }
  // show all of the file digits
  uint8_t part =  (x - SCOREOFFSET) / (DIGITSIZE);
  return (DIGITS[x - SCOREOFFSET - (DIGITSIZE * part) + (score->D[(SCOREDIGITS - 1) - part] * DIGITSIZE)]);
}

uint8_t VelocityDisplay(uint8_t x, uint8_t y, DIGITAL * velocity, uint8_t horizontal)
{
  // if on line 4 or 5  for horizontal(4) an vertical(4) speed
  if ((horizontal == 1 && y != 4) || (horizontal == 0 && y != 5)) {
    return 0;
  }
  // display velocity within the limits ...
  if ((x < VELOOFFSET) || (x > (VELOOFFSET + (VELODIGITS * DIGITSIZE)) - 1)) {
    return 0;
  }
  // show plus or minus sign
  if ((x >= VELOOFFSET) && (x < (VELOOFFSET + DIGITSIZE))) {
    return (DIGITS[x - VELOOFFSET + ((10 + (velocity->IsNegative)) * DIGITSIZE)]);
  }
  // show just 3 digits
  uint8_t part =  ((x - VELOOFFSET) / (DIGITSIZE));
  return (DIGITS[x - VELOOFFSET - (DIGITSIZE * part) + (velocity->D[(VELODIGITS - 1) - part] * DIGITSIZE)]);
}

uint8_t DashboardDisplay(uint8_t x, uint8_t y, GAME * game)
{
  if (x >= 0 && x <= 22) {
    return (DASHBOARD[x + y * 23]);
  }
  return 0x00;
}

uint8_t LanderDisplay(uint8_t x, uint8_t y, GAME * game) {
  uint8_t line = game->ShipPosY / 8;
  uint8_t offset = game->ShipPosY % 8;
  if (y == line || ((y == line + 1) && offset > 0))
  {
    if (((x - game->ShipPosX) >= 0) && ((x - game->ShipPosX) < 7)) {
      uint8_t sprite = getLanderSprite (x, y, game);
      if (offset == 0 && y == line)
        return sprite;
      if (offset > 0 && y == line)
        return sprite << offset;
      if (offset > 0 && y == (line + 1))
        return sprite >> (8 - offset);
    }
  }
  return 0x00;
}

uint8_t getLanderSprite(uint8_t x, uint8_t y, GAME * game)
{
  uint8_t sprite = 0x00;

  if (game->ShipExplode > 0)
  {
    sprite = (LANDER[(x - game->ShipPosX) + ((8 - (game->ShipExplode)) * 7) ]);
    JOY_sound(20 * game->ShipExplode, 10);
    (game->ShipExplode)--;
    if (game->ShipExplode < 1)
      game->ShipExplode = 3;
    return sprite;
  }

  // top sprite (4 bit)
  if (game->ThrustLEFT)
    sprite = (LANDER[(x - game->ShipPosX) + 21]);
  else if (game->ThrustRIGHT)
    sprite = (LANDER[(x - game->ShipPosX) + 28]);
  else
    sprite = (LANDER[(x - game->ShipPosX) ]);

  // bottom spite (4 bit)
  if (game->ThrustUP && game->Toggle && game->Fuel > 0)
    return (sprite |= (LANDER[(x - game->ShipPosX) + 14]));
  else
    return (sprite |= (LANDER[(x - game->ShipPosX) + 7]));
}

uint8_t FuelDisplay(uint8_t x, uint8_t y, GAME * game)
{
  if (y != 6) return 0x00;
  if (x > 4 && x <= 19)
  {
    // max fuel = 15.000 Liter - each liter = 1 fuel-bar we have 15 bars
    if ((game->Fuel / 1000) + 1 > x - 4 || ((x - 4 == 1) && game->Fuel > 0))
      return 0xF8;
    else
      return 0x00;
  }
  return 0x00;
}

uint8_t GameDisplay(uint8_t x, uint8_t y, GAME * game)
{
  const uint8_t offset = 23;
  if (x >= offset)
  {
    uint8_t frame;
    if (x == offset || x == 127)
      // left and right border-line
      frame = 0xFF;
    else
      // draw the map from the coordinates given by the GAMEMAP
      frame = GETLANDSCAPE(x - offset, y, ((game->Level - 1) * 2), game);

    uint8_t ship = LanderDisplay(x, y, game);

    if (y == 7 && x >= (game->LandingPadLEFT + offset) && x <= (game->LandingPadRIGHT + offset))
    {
      if (ship != 0 && (0xFC | ship) != (0xFC + ship))
      {
        if (abs(game->velocityY) <= LANDINGSPEED && (game->ShipPosX >= game->LandingPadLEFT + offset) && (game->ShipPosX + 7 <= game->LandingPadRIGHT + offset) )
        {
          game->HasLanded = true;
          return frame | ship;
        }
        else
        {
          if (!game->Collision)
            game->Lives--;
          game->ShipExplode = 3;
          game->Collision = true;

          return frame | LanderDisplay(x, y, game);
        }
      }
    }
    else if  ((frame != 0 && ship != 0) && (frame | ship) != (frame + ship))
    {
      if (!game->Collision)
        game->Lives--;
      game->ShipExplode = 3;
      game->Collision = true;
      return frame | LanderDisplay(x, y, game);
    }

    return frame | ship;
  }
  return 0x00;
}

uint8_t StarsDisplay(uint8_t x, uint8_t y, GAME * game)
{
  const uint8_t o1 = 23;
  uint8_t bg = 0x00;
  if (y == 0 && x > o1)
  {
    bg |= 0x01;
  }
  if (x == o1)
  {
    bg |= 0xFF;
  }
  if (x == 127)
  {
    bg |= 0xFF;
  }
  if (y == 7 && x > o1)
  {
    bg |= 0x80;
  }

  const uint8_t offset = 40;
  if (y > 1 && y < 5)
  {
    if (x > offset &&  x < (offset + 72))
    {
      if (game->Stars > (x - offset) / 24)
      {
        return (STARFULL[((x - offset) % 24) + ((y - 2) * 24)] );
      }
      else
      {
        return (STAROUTLINE[((x - offset) % 24) + ((y - 2) * 24)] );
      }
    }
  }
  return bg;
}

uint8_t LivesDisplay(uint8_t x, uint8_t y, GAME * game)
{
  const uint8_t offset = 1;
  if (y == 7 && x >= offset && x < (4 * 5) + offset)
  {
    if (game->Lives > (x - offset) / 5)
      return (LIVE2[(x - offset) % 5]);
  }
  return 0x00;
}

void Tiny_Flip(uint8_t mode, GAME * game, DIGITAL * score, DIGITAL * velX, DIGITAL * velY) {
  uint8_t y, x;
  for (y = 0; y < 8; y++)
  {
    JOY_OLED_data_start(y);
    for (x = 0; x < 128; x++)
    {
      if (mode == 0) {
        JOY_OLED_send(GameDisplay(x, y, game) | LivesDisplay(x, y, game) | DashboardDisplay(x, y, game) | ScoreDisplay(x, y, score) | VelocityDisplay(x, y, velX, 1) | VelocityDisplay(x, y, velY, 0) | FuelDisplay(x, y, game));
      } else if (mode == 1) {
        JOY_OLED_send((INTRO[x + (y * 128)]));
      }
      else if (mode == 2)
      {
        JOY_OLED_send(StarsDisplay ( x, y, game) | LivesDisplay(x, y, game) | DashboardDisplay(x, y, game) | ScoreDisplay(x, y, score) | VelocityDisplay(x, y, velX, 1) | VelocityDisplay(x, y, velY, 0) | FuelDisplay(x, y, game));
      }
    }
    JOY_OLED_end();
  }
}

void SetLandingMap(uint8_t level, GAME *game)
{
  uint8_t i;
  uint8_t prev;
  game->LandingPadLEFT = 0;
  game->LandingPadRIGHT = 255;
  for (i = 0; i < 27; i++)
  {
    uint8_t val = (GAMEMAP[(level - 1) * 2][i]);

    if ((prev == 0 && (val != 0 || i == 26)) && game->LandingPadRIGHT == 0)
    {
      game->LandingPadRIGHT  = i * 4;
    }

    if (val == 0 && game->LandingPadLEFT == 0)
    {
      game->LandingPadLEFT = i * 4;
      game->LandingPadRIGHT = 0;
    }

    prev = val;
  }
}

void SETNEXTLEVEL(uint8_t level, GAME *game)
{
  if ( level > NUMOFGAMES)
    level = 1;
  game->Level = level;
  SetLandingMap(level, game);
  game->ShipPosX = (GAMELEVEL[level - 1][0]);
  game->ShipPosY = (GAMELEVEL[level - 1][1]);
  game->Fuel = 100 * (GAMELEVEL[level - 1][2]);
  game->LevelScore = (GAMELEVEL[level - 1][3]);
  game->FuelBonus = 100 * (GAMELEVEL[level - 1][4]);
}

uint8_t GETLANDSCAPE(uint8_t x, uint8_t y, uint8_t level, GAME *game)
{
  const uint8_t height = 63;
  uint8_t frame = 0x00;
  uint8_t t =  x % 4;
  uint8_t ind = x / 4;
  uint8_t val = height - (GAMEMAP[level][ind]);
  uint8_t valT = height - (GAMEMAP[level + 1][ind]);
  if (x > 0 && t != 0)
  {
    if ( (ind + 1) < 27)
    {
      if (val < height)
      { uint8_t val2 = height - (GAMEMAP[level][ind + 1]);
        val += ((val2 - val) / 4) * ( t);
      }
      uint8_t valT2 = height - (GAMEMAP[level + 1][ind + 1]);
      valT += ((valT2 - valT) / 4) * ( t);
    }
  }

  uint8_t b = val / 8;
  uint8_t bT = valT / 8;
  if (b == y)
  {
    // draw the landing-platform
    if (val == height)
      if (x % 2 == 0)
        frame |= 0xB8;
      else
        frame |= 0x58;
    else
      // draw pixel on the correct height
      frame |= (0xFF << (val - (b * 8)) ) ;
  }
  if (bT == y)
    frame |= (0xFF >>  (7 - (valT - (bT * 8))));
  if (y > b || y < bT )
    frame |= 0xFF;

  return frame;
}

// splits each digit in it's own byte
void SPLITDIGITS(uint16_t val, uint8_t *d)
{
  d[4] = val / 10000;
  d[3] = (val - (d[4] * 10000)) / 1000;
  d[2] = (val - (d[3] * 1000) - (d[4] * 10000)) / 100;
  d[1] = (val - (d[2] * 100) - (d[3] * 1000) - (d[4] * 10000)) / 10;
  d[0] = val - (d[1] * 10) - (d[2] * 100) - (d[3] * 1000) - (d[4] * 10000);
}

void INTROJOY_sound()
{
  JOY_sound(80, 55); DLY_ms(20); JOY_sound(90, 55); DLY_ms(20); JOY_sound(100, 55); JOY_sound(115, 255); JOY_sound(115, 255);
}
void VICTORYJOY_sound()
{
  JOY_sound(111, 100); DLY_ms(20); JOY_sound(111, 90); DLY_ms(20); JOY_sound(144, 255); JOY_sound(144, 255); JOY_sound(144, 255);
}

void ALERTJOY_sound()
{
  JOY_sound(150, 100); DLY_ms(100); JOY_sound(150, 90); DLY_ms(100); JOY_sound(150, 100);
}

void HAPPYJOY_sound()
{
  JOY_sound(75, 90); DLY_ms(10); JOY_sound(114, 90); JOY_sound(121, 90);
}
