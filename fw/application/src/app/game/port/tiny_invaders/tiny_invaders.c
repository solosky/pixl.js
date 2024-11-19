// ===================================================================================
// Project:   Tiny Invaders - Conversion for CH32V003
// Version:   v1.0
// Year:      2023
// ===================================================================================
//
//   >>>>>  T-I-N-Y  I-N-V-A-D-E-R-S v3.1 for ATTINY85  GPLv3 <<<<
//                   Programmer: Daniel C 2018-2021
//              Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                    https://www.tinyjoypad.com
//         https://sites.google.com/view/arduino-collection
//
//  Tiny Invaders v3.1 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ===================================================================================
// Modified to work with CH32V003 by Stefan Wagner: https://github.com/wagiminator
// ===================================================================================

#include "../common/driver.h"
#include "spritebank.h"

// ===================================================================================
// Global Variables
// ===================================================================================
static uint8_t Live = 0;
uint8_t ShieldRemoved = 0;
uint8_t MONSTERrest = 0;
uint8_t LEVELS = 0;
uint8_t SpeedShootMonster = 0;
uint8_t ShipDead = 0;
uint8_t ShipPos = 56;

#define SHOOTS 2

// ===================================================================================
// Function Prototypes
// ===================================================================================
void LoadMonstersLevels(int8_t Levels, SPACE *space);
void SnD(int8_t Sp_, uint8_t SN);
void SpeedControle(SPACE *space);
void GRIDMonsterFloorY(SPACE *space);
uint8_t LivePrint(uint8_t x, uint8_t y);
static void Tiny_Flip(uint8_t render0_picture1, SPACE *space);
uint8_t UFOWrite(uint8_t x, uint8_t y, SPACE *space);
void UFOUpdate(SPACE *space);
void ShipDestroyByMonster(SPACE *space);
void MonsterShootupdate(SPACE *space);
void MonsterShootGenerate(SPACE *space);
uint8_t MonsterShoot(uint8_t x, uint8_t y, SPACE *space);
uint8_t ShieldDestroy(uint8_t Origine, uint8_t VarX, uint8_t VarY, SPACE *space);
void ShieldDestroyWrite(uint8_t BOOLWRITE, uint8_t line, SPACE *space, uint8_t Origine);
uint8_t MyShield(uint8_t x, uint8_t y, SPACE *space);
uint8_t ShieldBlitz(uint8_t Part, uint8_t LineSH);
uint8_t BOOLREAD(uint8_t SHnum, uint8_t LineSH, SPACE *space);
void RemoveExplodOnMonsterGrid(SPACE *space);
uint8_t background(uint8_t x, uint8_t y, SPACE *space);
uint8_t Vesso(uint8_t x, uint8_t y, SPACE *space);
void UFO_Attack_Check(uint8_t x, SPACE *space);
uint8_t MyShoot(uint8_t x, uint8_t y, SPACE *space);
void Monster_Attack_Check(SPACE *space);
int8_t OuDansLaGrilleMonster(uint8_t x, uint8_t y, SPACE *space);
uint8_t SplitSpriteDecalageY(uint8_t Input, uint8_t UPorDOWN, SPACE *space);
uint8_t Murge_Split_UP_DOWN(uint8_t x, SPACE *space);
uint8_t WriteMonster14(uint8_t x);
uint8_t Monster(uint8_t x, uint8_t y, SPACE *space);
uint8_t MonsterRefreshMove(SPACE *space);
void VarResetNewLevel(SPACE *space);

// ===================================================================================
// Main Function
// ===================================================================================
int tiny_invaders_run(void) {
  // Setup
  JOY_init();

  // Loop
  while(1) {
    uint8_t Decompte = 0;
    uint8_t VarPot;
    uint8_t MyShootReady = SHOOTS;
    SPACE space;

  NEWGAME:
    Live = 3;
    LEVELS = 0;
    Tiny_Flip(1, &space);
    while(1) {
      if(JOY_act_pressed()) {
        JOY_sound(100, 125); JOY_sound(50, 125);
        goto BYPASS2;
      }
      JOY_idle();
    }

  NEWLEVEL:
    JOY_DLY_ms(1000);

  BYPASS2:
    VarResetNewLevel(&space);
    SpeedControle(&space);
    VarPot = 54;
    ShipPos = 56;
    space.ScrBackV=(ShipPos / 14) + 52;
    goto Bypass;

  RestartLevel:
    if(Live > 0) Live--;
    else goto NEWGAME;

  Bypass:
    ShipDead = 0;
    Decompte = 0;
    Tiny_Flip(0, &space);
    JOY_DLY_ms(1000);
    while(1) {
      if(MONSTERrest == 0) { 
        JOY_sound(110, 255); JOY_DLY_ms(40); JOY_sound(130, 255); JOY_DLY_ms(40);
        JOY_sound(100, 255); JOY_DLY_ms(40); JOY_sound(1, 155);   JOY_DLY_ms(20);
        JOY_sound(60, 255);  JOY_sound(60, 255);
        if(LEVELS < 9) LEVELS++;
        goto NEWLEVEL;
      }
      if((((space.MonsterGroupeYpos) + (space.MonsterFloorMax + 1)) == 7) && (Decompte == 0)) ShipDead = 1;
      if(SpeedShootMonster <= 9 - LEVELS) SpeedShootMonster++;
      else {SpeedShootMonster = 0; MonsterShootGenerate(&space);}
      space.ScrBackV = (ShipPos / 14) + 52;
      Tiny_Flip(0, &space);
      space.oneFrame = !space.oneFrame;
      RemoveExplodOnMonsterGrid(&space);
      MonsterShootupdate(&space);
      UFOUpdate(&space);
      if(((space.MonsterGroupeXpos >= 26) && (space.MonsterGroupeXpos <= 28))
        && (space.MonsterGroupeYpos == 2) && (space.DecalageY8 == 4)) space.UFOxPos = 127;
      if(VarPot > (ShipPos + 2)) ShipPos = ShipPos + ((VarPot - ShipPos) / 3);
      if(VarPot < (ShipPos - 2)) ShipPos = ShipPos - ((ShipPos - VarPot) / 3);
      if(ShipDead != 1) {
        if(space.frame < space.frameMax) space.frame++;
        else {
          GRIDMonsterFloorY(&space);
          space.anim = !space.anim;
          if(space.anim == 0) SnD(space.UFOxPos, 200);
          else SnD(space.UFOxPos, 100);
          MonsterRefreshMove(&space);
          space.frame = 0;
        }

        if(JOY_left_pressed()) {
          if(VarPot > 5) VarPot = VarPot - 6;
        }
        if(JOY_right_pressed()) {
          if(VarPot < 108) VarPot = VarPot + 6;
        }
        if((JOY_act_pressed()) && (MyShootReady == SHOOTS)) {
          JOY_sound(200, 4); MyShootReady = 0; space.MyShootBall = 6; space.MyShootBallxpos = ShipPos + 6;
        }
      }
      else {
        JOY_sound(80, 1); JOY_sound(100, 1); 
        Decompte++;
        if(Decompte >= 30) {
          JOY_DLY_ms(600);
          if(((space.MonsterGroupeYpos) + (space.MonsterFloorMax + 1)) == 7) goto NEWGAME;
          else goto RestartLevel;
        }
      }
      if(space.MyShootBall == -1) {
        if(MyShootReady<SHOOTS) MyShootReady++;
      }
      JOY_idle();
    JOY_SLOWDOWN();
    }
  }
}

// ===================================================================================
// Functions
// ===================================================================================
void LoadMonstersLevels(int8_t Levels, SPACE *space) {
  uint8_t x, y;
  for(y=0; y<5; y++) {
    for(x=0; x<6; x++) {
      if(y!=4) space->MonsterGrid[y][x] = MonstersLevels[(Levels * 24) + (y * 6) + x];
      else     space->MonsterGrid[y][x] = -1;
    }
  }
}

void SnD(int8_t Sp_, uint8_t SN) {
  if(Sp_ != -120) {JOY_sound(220, 8); JOY_sound(200, 4);}
  else JOY_sound(SN, 1);
}

void SpeedControle(SPACE *space) {
  uint8_t xx, yy;
  MONSTERrest = 0;
  for(yy=0; yy<4; yy++) {
    for(xx=0; xx<6; xx++) {
      if((space->MonsterGrid[yy][xx] != -1) && ((space->MonsterGrid[yy][xx] <=5 ))) MONSTERrest++;
    }
  }
  space->frameMax = (MONSTERrest >> 3);
}

// Thanks to Sven Bruns for informing me of an error in this function!
void GRIDMonsterFloorY(SPACE *space) {
  uint8_t y,x;
  space->MonsterFloorMax = 3;
  for(y=0; y<4; y++) {
    for(x=0; x<6; x++) {
      if(space->MonsterGrid[3-y][x] != -1) return;
    }
    space->MonsterFloorMax = space->MonsterFloorMax - 1;
  }
}

uint8_t LivePrint(uint8_t x, uint8_t y) {
  #define XLIVEWIDE ((5 * Live) - 1)
  if((0 >= (x - XLIVEWIDE)) && (y == 7)) {
    return LIVE[x];
  }
  return 0x00;
}

void Tiny_Flip(uint8_t render0_picture1, SPACE *space) {
  uint8_t y, x; 
  uint8_t MYSHIELD = 0x00;
  for(y=0; y<8; y++) {
    JOY_OLED_data_start(y);
    for(x=0; x<128; x++) {
      if(render0_picture1 == 0) {
        if(ShieldRemoved == 0) MYSHIELD = MyShield(x, y, space);
        else MYSHIELD = 0x00;
        JOY_OLED_send( background(x, y, space)
                       | LivePrint(x, y) 
                       | Vesso(x, y, space)
                       | UFOWrite(x, y, space)
                       | Monster(x, y, space)
                       | MyShoot(x, y, space)
                       | MonsterShoot(x, y, space)
                       | MYSHIELD);
      }
      else JOY_OLED_send(intro[x + (y * 128)]);
    }
    if(render0_picture1 == 0) {
      if(ShieldRemoved == 0) ShieldDestroy(0, space->MyShootBallxpos, space->MyShootBall, space);
    }
    JOY_OLED_end();
  }
  if(render0_picture1 == 0) {
    if(!(space->MonsterGroupeYpos < (2 + (4 - (space->MonsterFloorMax + 1))))) {
      if(ShieldRemoved != 1) {
        space->Shield[0] = 0x00;
        space->Shield[1] = 0x00;
        space->Shield[2] = 0x00;
        space->Shield[3] = 0x00;
        space->Shield[4] = 0x00;
        space->Shield[5] = 0x00;
        ShieldRemoved = 1;
      }
    }
  }
}

uint8_t UFOWrite(uint8_t x, uint8_t y, SPACE *space) {
  if((space->UFOxPos != -120) && (y == 0) && (space->UFOxPos <= x) && (space->UFOxPos >= (x - 14)))
    return Monsters[(x - space->UFOxPos) + (6 * 14) + (space->oneFrame * 14)];
  return 0x00;
}

void UFOUpdate(SPACE *space) {
  if(space->UFOxPos != -120) {
    space->UFOxPos = space->UFOxPos - 2;
    SnD(space->UFOxPos, 0);
    if(space->UFOxPos <= -20) space->UFOxPos = -120;
  }
}

void ShipDestroyByMonster(SPACE *space) {
  if((space->MonsterShoot[1] >= 14) && (space->MonsterShoot[1] <= 15) 
  && (space->MonsterShoot[0] >= ShipPos) && (space->MonsterShoot[0] <= ShipPos+14))
    ShipDead=1;
}

void MonsterShootupdate(SPACE *space) {
  if(space->MonsterShoot[1] != 16) {
    ShipDestroyByMonster(space);
    if(ShieldDestroy(1, space->MonsterShoot[0], space->MonsterShoot[1] >> 1, space))
      space->MonsterShoot[1] = 16;
    else
      space->MonsterShoot[1] = space->MonsterShoot[1] + 1;
  }
}

void MonsterShootGenerate(SPACE *space) {
  uint8_t a = JOY_random() % 3; 
  uint8_t b = JOY_random() % 6; 
  if(b >= 5) b = 5;
  if(space->MonsterShoot[1] == 16) {
    if(space->MonsterGrid[a][b] != -1) {
      space->MonsterShoot[0] = (space->MonsterGroupeXpos + 7) + (b * 14);
      space->MonsterShoot[1] = ((space->MonsterGroupeYpos + a) * 2) + 1;
    }
  }
}

uint8_t MonsterShoot(uint8_t x, uint8_t y, SPACE *space) {
  if(((space->MonsterShoot[1] >> 1) == y) && (space->MonsterShoot[0] == x)) {
    if((space->MonsterShoot[1] & 1) == 0) return 0b00001111;
    else return 0b11110000;
  }
  return 0x00;
}

uint8_t ShieldDestroy(uint8_t Origine, uint8_t VarX, uint8_t VarY, SPACE *space) {
  #define OFFSETXSHIELD -1
  if(VarY == 6) {
    if((VarX >= 20 + OFFSETXSHIELD) && (VarX <= 27 + OFFSETXSHIELD)) {
      if(BOOLREAD(0, VarX - (20 + OFFSETXSHIELD), space)) {
        ShieldDestroyWrite(0, VarX - (20 + OFFSETXSHIELD), space, Origine);
        return 1;
      }
    }
    if((VarX >= 28 + OFFSETXSHIELD) && (VarX <= 35 + OFFSETXSHIELD)) {
      if(BOOLREAD(1, VarX - (28 + OFFSETXSHIELD), space)) {
        ShieldDestroyWrite(1, VarX - (28 + OFFSETXSHIELD), space, Origine);
        return 1;
      }
    }
    if((VarX >= 55 + OFFSETXSHIELD) && (VarX <= 62 + OFFSETXSHIELD)) {
      if(BOOLREAD(2, VarX - (55 + OFFSETXSHIELD), space)) {
        ShieldDestroyWrite(2, VarX - (55 + OFFSETXSHIELD), space, Origine);
        return 1;
      }
    }
    if((VarX >= 63 + OFFSETXSHIELD) && (VarX <= 70 + OFFSETXSHIELD)) {
      if(BOOLREAD(3, VarX - (63 + OFFSETXSHIELD), space)) {
        ShieldDestroyWrite(3, VarX - (63 + OFFSETXSHIELD), space, Origine);
        return 1;
      }
    }
    if((VarX >= 90 + OFFSETXSHIELD) && (VarX <= 97 + OFFSETXSHIELD)) {
      if(BOOLREAD(4, VarX - (90 + OFFSETXSHIELD), space)) {
        ShieldDestroyWrite(4, VarX - (90 + OFFSETXSHIELD), space, Origine);
        return 1;
      }
    }
    if((VarX >= 98 + OFFSETXSHIELD) && (VarX <= 105 + OFFSETXSHIELD)) {
      if(BOOLREAD(5, VarX - (98 + OFFSETXSHIELD), space)) {
        ShieldDestroyWrite(5, VarX - (98 + OFFSETXSHIELD), space, Origine);
        return 1;
      }
    }
  }
  return 0;
}

void ShieldDestroyWrite(uint8_t BOOLWRITE, uint8_t line, SPACE *space, uint8_t Origine) {
  switch(line) {
    case 0:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 128;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    case 1:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 64;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    case 2:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 32;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    case 3:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 16;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    case 4:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 8;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    case 5:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 4;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    case 6:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 2;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    case 7:
      space->Shield[BOOLWRITE] = space->Shield[BOOLWRITE] - 1;
      if(Origine == 0) space->MyShootBall = -1;
      break;
    default:
      break;
  }
}

uint8_t MyShield(uint8_t x, uint8_t y, SPACE *space) {
  #define OFFSETXSHIELD -1
  if(y != 6) return 0x00;
  if((x >= 20 + OFFSETXSHIELD) && (x <= 27 + OFFSETXSHIELD)) {
    if(BOOLREAD(0, x - (20 + OFFSETXSHIELD), space)) return ShieldBlitz(0, x - (20 + OFFSETXSHIELD));
    else return 0x00;
  }
  if((x >= 28 + OFFSETXSHIELD) && (x <= 35 + OFFSETXSHIELD)) {
    if(BOOLREAD(1, x - (28 + OFFSETXSHIELD), space)) return ShieldBlitz(1, x - (28 + OFFSETXSHIELD));
    else return 0x00;
  }
  if((x >= 55 + OFFSETXSHIELD) && (x <= 62 + OFFSETXSHIELD)) {
    if(BOOLREAD(2, x - (55 + OFFSETXSHIELD), space)) return ShieldBlitz(0, x - (55 + OFFSETXSHIELD));
    else return 0x00;
  }
  if((x >= 63 + OFFSETXSHIELD) && (x <= 70 + OFFSETXSHIELD)) {
    if(BOOLREAD(3, x - (63 + OFFSETXSHIELD), space)) return ShieldBlitz(1, x - (63 + OFFSETXSHIELD));
    else return 0x00;
  }
  if((x >= 90 + OFFSETXSHIELD) && (x <= 97 + OFFSETXSHIELD)) {
    if(BOOLREAD(4, x - (90 + OFFSETXSHIELD), space)) return ShieldBlitz(0, x - (90 + OFFSETXSHIELD));
    else return 0x00;
  }
  if((x >= 98 + OFFSETXSHIELD) && (x <= 105 + OFFSETXSHIELD)) {
    if(BOOLREAD(5, x - (98 + OFFSETXSHIELD), space)) return ShieldBlitz(1, x - (98 + OFFSETXSHIELD));
    else return 0x00;
  }
  return 0x00;
}

uint8_t ShieldBlitz(uint8_t Part, uint8_t LineSH) {
  uint8_t Var0 = 0;
  switch(LineSH) {
    case 0: if(Part == 0) Var0 = 0b11110000; else Var0 = 0b00001111; break;
    case 1: if(Part == 0) Var0 = 0b11111100; else Var0 = 0b00001111; break;
    case 2:
    case 3:
    case 4:
    case 5: Var0 = 0b00001111; break;
    case 6: if(Part == 1) Var0 = 0b11111100; else Var0 = 0b00001111; break;
    case 7: if(Part == 1) Var0 = 0b11110000; else Var0 = 0b00001111; break;
    default: Var0 = 0b00000000; break;
  }
  return Var0;
}

uint8_t BOOLREAD(uint8_t SHnum, uint8_t LineSH, SPACE *space) {
  uint8_t Var0 = 0b10000000 >> LineSH;
  return((space->Shield[SHnum] & Var0) != 0);
}

void RemoveExplodOnMonsterGrid(SPACE *space) {
  uint8_t x, y;
  for(y=0; y<=3; y++) {
    for(x=0; x<=5; x++) {
      if(space->MonsterGrid[y][x] >= 11) space->MonsterGrid[y][x] = -1; 
      if(space->MonsterGrid[y][x] >= 8)  space->MonsterGrid[y][x] = space->MonsterGrid[y][x] + 1;
    }
  }
}

uint8_t background(uint8_t x, uint8_t y, SPACE *space) {
  uint8_t scr = space->ScrBackV + x;
  if(scr > 127) scr = space->ScrBackV + x - 128;
  return(0xff - back[y * 128 + scr]);
}

uint8_t Vesso(uint8_t x, uint8_t y, SPACE *space) {
  if((x - ShipPos >= 0) && (x - ShipPos < 13) && (y==7)) {
    if(ShipDead == 0) return vesso[x - ShipPos];
    else return vesso[x - ShipPos + (12 * space->oneFrame)];
  }
  return 0;
}

void UFO_Attack_Check(uint8_t x, SPACE *space) {
  if(space->MyShootBall == 0) {
    if((space->MyShootBallxpos >= space->UFOxPos) && (space->MyShootBallxpos <= space->UFOxPos + 14)) {
      for(x=1; x<100; x++) JOY_sound(x, 1);
      if(Live < 3) Live++;
      space->UFOxPos =- 120;
    }
  }
}

uint8_t MyShoot(uint8_t x, uint8_t y, SPACE *space) {
  if((space->MyShootBallxpos == x) && (y == space->MyShootBall)) {
    if(space->MyShootBall > -1) space->MyShootBallFrame = !space->MyShootBallFrame;
    else return 0x00;
    if(space->MyShootBallFrame == 1) space->MyShootBall--;
    Monster_Attack_Check(space);
    UFO_Attack_Check(x, space);
    return SHOOT[space->MyShootBallFrame];
  }
  return 0x00;
}

void Monster_Attack_Check(SPACE *space) {
  int8_t Varx = 0, Vary = 0;
  #define Xmouin   (space->MonsterGroupeXpos) 
  #define Ymouin   (space->MonsterGroupeYpos << 3) //-space->DecalageY8
  #define XPlus    (Xmouin + 84)
  #define YPlus    (Ymouin + (4*8))
  #define MYSHOOTX (space->MyShootBallxpos)
  #define MYSHOOTY ((space->MyShootBall << 3) + ((space->MyShootBallFrame + 1) << 2))
  if((MYSHOOTX >= Xmouin) && (MYSHOOTX <= XPlus) && (MYSHOOTY >= Ymouin) && (MYSHOOTY <= YPlus)) {
    //enter in the monster zone
    Vary = (MYSHOOTY - Ymouin + 4) >> 3;
    Varx = (MYSHOOTX - Xmouin + 7) / 14;
    if(Varx < 0) Varx = 0;
    if(Vary < 0) Vary = 0;
    if(Varx > 5) return;
    if(Vary > 3) return;
    if((space->MonsterGrid[Vary][Varx] > -1) && (space->MonsterGrid[Vary][Varx] < 6)) {
      JOY_sound(50, 10);
      space->MonsterGrid[Vary][Varx] = 8;
      space->MyShootBall = -1;
      SpeedControle(space);
    }
    //fin monster zone
  }
}

int8_t OuDansLaGrilleMonster(uint8_t x, uint8_t y, SPACE *space) {
  if(x < space->MonsterGroupeXpos) return -1;
  if(y < space->MonsterGroupeYpos) return -1;
  space->PositionDansGrilleMonsterX = (x - space->MonsterGroupeXpos) / 14;
  space->PositionDansGrilleMonsterY = (y - space->MonsterGroupeYpos);
  if(space->PositionDansGrilleMonsterX > 5) return -1;
  if(space->PositionDansGrilleMonsterY > 4) return -1;
  return 0;
}

uint8_t SplitSpriteDecalageY(uint8_t Input, uint8_t UPorDOWN, SPACE *space) {
  if(UPorDOWN) return(Input << space->DecalageY8);
  else return(Input >> (8 - space->DecalageY8));
}

uint8_t Murge_Split_UP_DOWN(uint8_t x, SPACE *space) {
  int8_t SpriteType = -1;
  int8_t ANIMs = -1;
  uint8_t Murge1 = 0;
  uint8_t Murge2 = 0;
  if(space->DecalageY8 == 0) {
    SpriteType = space->MonsterGrid[space->PositionDansGrilleMonsterY][space->PositionDansGrilleMonsterX];
    if(SpriteType < 8) ANIMs = space->anim * 14;
    else ANIMs = 0;
    if(SpriteType == -1) return 0x00;
    return Monsters[(WriteMonster14(x - space->MonsterGroupeXpos) + SpriteType * 14) + ANIMs];
  }
  else {
    //debut
    if(space->PositionDansGrilleMonsterY == 0) {
      SpriteType = space->MonsterGrid[space->PositionDansGrilleMonsterY][space->PositionDansGrilleMonsterX];
      if(SpriteType < 8) ANIMs = space->anim * 14;
      else ANIMs = 0;
      if(SpriteType != -1) Murge2 = SplitSpriteDecalageY(Monsters[(WriteMonster14(x - space->MonsterGroupeXpos) + SpriteType * 14) + ANIMs], 1, space);
      else Murge2 = 0x00;
      return Murge2;
    }
    else {
      SpriteType = space->MonsterGrid[space->PositionDansGrilleMonsterY - 1][space->PositionDansGrilleMonsterX];
      if(SpriteType < 8) ANIMs = space->anim * 14;
      else ANIMs = 0;
      if(SpriteType != -1) Murge1 = SplitSpriteDecalageY(Monsters[(WriteMonster14(x - space->MonsterGroupeXpos) + SpriteType * 14) + ANIMs], 0, space);
      else Murge1 = 0x00;
      SpriteType = space->MonsterGrid[space->PositionDansGrilleMonsterY][space->PositionDansGrilleMonsterX];
      if(SpriteType < 8) ANIMs = space->anim * 14;
      else ANIMs = 0;
      if(SpriteType != -1) Murge2 = SplitSpriteDecalageY(Monsters[(WriteMonster14(x - space->MonsterGroupeXpos) + SpriteType * 14) + ANIMs], 1, space);
      else Murge2 = 0x00;
      return(Murge1 | Murge2);
    }  
  } //fin
}

uint8_t WriteMonster14(uint8_t x) {
  while(x >= 14) x -= 14;
  return x;
}

uint8_t Monster(uint8_t x, uint8_t y, SPACE *space) {
  if(OuDansLaGrilleMonster(x, y, space) != -1) return  Murge_Split_UP_DOWN(x, space);
  return 0x00;
}

uint8_t MonsterRefreshMove(SPACE *space) {
  if(space->Direction == 1) {
    if(space->MonsterGroupeXpos < space->MonsterOffsetDroite) {
      space->MonsterGroupeXpos = space->MonsterGroupeXpos + 2;
      return 0;
    }
    else {
      if(space->DecalageY8 < 7) {
        space->DecalageY8 = space->DecalageY8 + 4;
        if(space->DecalageY8 > 7) space->DecalageY8 = 7; 
      }
      else {
        space->MonsterGroupeYpos++;
        space->DecalageY8 = 0;
      }
      space->Direction = 0;
      return 0;
    }
  }
  else {
    if(space->MonsterGroupeXpos > space->MonsterOffsetGauche) {
      space->MonsterGroupeXpos = space->MonsterGroupeXpos - 2;
      return 0;
    }
    else {
      if(space->DecalageY8 < 7) {
        space->DecalageY8 = space->DecalageY8 + 4;
        if(space->DecalageY8 > 7) space->DecalageY8 = 7;
      }
      else {
        space->MonsterGroupeYpos++;
        space->DecalageY8 = 0;
      }
      space->Direction = 1;
      return 0;
    }
  }
}

void VarResetNewLevel(SPACE *space) {
  ShieldRemoved = 0;
  SpeedShootMonster = 0;
  MONSTERrest = 24;
  LoadMonstersLevels(LEVELS, space);
  space->Shield[0] = 255;
  space->Shield[1] = 255;
  space->Shield[2] = 255;
  space->Shield[3] = 255;
  space->Shield[4] = 255;
  space->Shield[5] = 255;
  space->MonsterShoot[0] = 16;
  space->MonsterShoot[1] = 16;
  space->UFOxPos = -120;

  space->MyShootBall = -1;
  space->MyShootBallxpos = 0;
  space->MyShootBallFrame = 0;
  space->anim = 0;
  space->frame = 0;
  space->PositionDansGrilleMonsterX = 0;
  space->PositionDansGrilleMonsterY = 0;
  space->MonsterFloorMax = 3;
  space->MonsterOffsetGauche = 0;
  space->MonsterOffsetDroite = 44;
  space->MonsterGroupeXpos = 20;
  if(LEVELS > 3) space->MonsterGroupeYpos = 1;
  else space->MonsterGroupeYpos = 0;
  space->DecalageY8 = 0;
  space->frameMax = 8;
  space->Direction = 1; //1 right 0 gauche
  space->oneFrame = 0;
}
