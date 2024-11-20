// ===================================================================================
// Project:   Tiny Tris - Conversion for CH32V003
// Version:   v1.0
// Year:      2023
// ===================================================================================
//
//         >>>>>  T-I-N-Y  T-R-I-S v3 for ATTINY85  GPL v3 <<<<<
//                     Programmer: Daniel C 2019-2021
//              Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                      https://WWW.TINYJOYPAD.COM
//           https://sites.google.com/view/arduino-collection

//  Tiny-tris is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

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
uint8_t Grid_TTRIS[12][3] = {{0}};
const uint8_t MEM_TTTRIS[16] = {0, 2, 0, 4, 3, 7, 6, 9, 9, 12, 11, 15, 14, 17, 17, 19};
uint8_t Level_TTRIS;
uint16_t Scores_TTRIS;
uint16_t Nb_of_line_F_TTRIS;
uint8_t Level_Speed_ADJ_TTRIS;
uint8_t Nb_of_line_TTRIS[3];
uint8_t RND_VAR_TTRIS;
uint8_t LONG_PRESS_X_TTRIS;
uint8_t DOWN_DESACTIVE_TTRIS;
uint8_t DROP_SPEED_TTRIS;
uint8_t SPEED_x_trig_TTRIS;
uint8_t DROP_TRIG_TTRIS;
int8_t xx_TTRIS, yy_TTRIS;
uint8_t Piece_Mat2_TTRIS[5][5];
uint8_t Ripple_filter_TTRIS;
uint8_t PIECEs_TTRIS;
uint8_t PIECEs_TTRIS_PREVIEW;
uint8_t PIECEs_rot_TTRIS;
uint8_t DROP_BREAK_TTRIS;
int8_t OU_SUIS_JE_X_TTRIS;
int8_t OU_SUIS_JE_Y_TTRIS;
uint8_t OU_SUIS_JE_X_ENGAGED_TTRIS;
uint8_t OU_SUIS_JE_Y_ENGAGED_TTRIS;
int8_t DEPLACEMENT_XX_TTRIS;
int8_t DEPLACEMENT_YY_TTRIS;

// ===================================================================================
// Function Prototypes
// ===================================================================================
void reset_Score_TTRIS(void);
uint8_t PSEUDO_RND_TTRIS(void);
void SND_TTRIS(uint8_t Snd_TTRIS);
void INTRO_MANIFEST_TTRIS(void);
void END_DROP_TTRIS(void);
void SETUP_NEW_PREVIEW_PIECE_TTRIS(uint8_t *Rot_TTRIS);
void CONTROLE_TTRIS(uint8_t *Rot_TTRIS);
void Game_Play_TTRIS(void);
uint8_t End_Play_TTRIS(void);
void DELETE_LINE_TTRIS(void);
uint8_t Calcul_of_Score_TTRIS(uint8_t Tmp_TTRIS);
void FLASH_LINE_TTRIS(uint8_t *PASS_LINE);
void PAINT_LINE_TTRIS(uint8_t VISIBLE, uint8_t *PASS_LINE);
void Clean_Grid_TTRIS(uint8_t *PASS_LINE);
uint8_t CHECK_if_Rot_Ok_TTRIS(uint8_t *Rot_TTRIS);
uint8_t Check_collision_x_TTRIS(int8_t x_Axe);
uint8_t Check_collision_y_TTRIS(int8_t y_Axe);
void Move_Piece_TTRIS(void);
void Ou_suis_Je_TTRIS(int8_t xx_, int8_t yy_);
void Select_Piece_TTRIS(uint8_t Piece_);
void rotate_Matrix_TTRIS(uint8_t ROT);
uint8_t Scan_Piece_Matrix_TTRIS(int8_t x_Mat, int8_t y_Mat);
uint8_t GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN);
uint8_t CHANGE_GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN, uint8_t VALUE);
uint8_t blitzSprite_TTRIS(int8_t xPos, int8_t yPos, uint8_t xPASS, uint8_t yPASS, uint8_t FRAME,
                          const uint8_t *SPRITES);
uint8_t H_grid_Scan_TTRIS(uint8_t xPASS);
uint8_t Recupe_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t NEXT_BLOCK_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t RECUPE_BACKGROUND_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t DropPiece_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t SplitSpriteDecalageY_TTRIS(uint8_t decalage, uint8_t Input, uint8_t UPorDOWN);
uint8_t RecupeLineY_TTRIS(uint8_t Valeur);
uint8_t RecupeDecalageY_TTRIS(uint8_t Valeur);
void Tiny_Flip_TTRIS(uint8_t HR_TTRIS);
void Flip_intro_TTRIS(uint8_t *TIMER1);
uint8_t intro_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1);
uint8_t Recupe_Start_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1);
uint8_t recupe_Chateau_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t recupe_SCORES_TTRIS(uint8_t xPASS, uint8_t yPASS);
void Convert_Nb_of_line_TTRIS(void);
uint8_t recupe_Nb_of_line_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t recupe_LEVEL_TTRIS(uint8_t xPASS, uint8_t yPASS);
void INIT_ALL_VAR_TTRIS(void);
void recupe_HIGHSCORE_TTRIS(void);
void Reset_Value_TTRIS(void);
void save_HIGHSCORE_TTRIS(void);
void Check_NEW_RECORD(void);
uint8_t checksum(uint8_t Byte_);

// ===================================================================================
// Main Function
// ===================================================================================
int tiny_tris_run(void) {
    // Setup
    JOY_init();

    // Loop
    while (1) {
        Reset_Value_TTRIS();
        if ((JOY_down_pressed())) {
            JOY_DLY_ms(1000);
            if ((JOY_down_pressed())) {
                save_HIGHSCORE_TTRIS();
            }
        }
    MENU:;
        uint8_t Rot_TTRIS = 0;
        uint8_t SKIP_FRAME = 0;
        INIT_ALL_VAR_TTRIS();
        Game_Play_TTRIS();
        Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
        INTRO_MANIFEST_TTRIS();
        SETUP_NEW_PREVIEW_PIECE_TTRIS(&Rot_TTRIS);
        Tiny_Flip_TTRIS(128);
        JOY_DLY_ms(1000);
        xx_TTRIS = 55;
        yy_TTRIS = 5;
        while (1) {
            CONTROLE_TTRIS(&Rot_TTRIS);
            if (DROP_BREAK_TTRIS == 6) {
                END_DROP_TTRIS();
                if (End_Play_TTRIS()) {
                    Tiny_Flip_TTRIS(128);
                    SND_TTRIS(3);
                    JOY_DLY_ms(2000);
                    Check_NEW_RECORD();
                    goto MENU;
                }
                yy_TTRIS = 2;
                xx_TTRIS = 55;
                PIECEs_TTRIS = PIECEs_TTRIS_PREVIEW;
                SETUP_NEW_PREVIEW_PIECE_TTRIS(&Rot_TTRIS);
                DOWN_DESACTIVE_TTRIS = 1;
                Tiny_Flip_TTRIS(128);
                Game_Play_TTRIS();
                Tiny_Flip_TTRIS(128);
            }

            if ((JOY_act_pressed()) && (Ripple_filter_TTRIS == 0)) {
                PSEUDO_RND_TTRIS();
                Ripple_filter_TTRIS = 1;
            }

            Move_Piece_TTRIS();
            if (SKIP_FRAME == 6) {
                Tiny_Flip_TTRIS(82);
                SKIP_FRAME = 0;
            } else {
                SKIP_FRAME++;
            }

            JOY_idle();
            if (JOY_exit()) {
                goto EXIT;
            }
        }
    }

EXIT:
    return 0;
}

// ===================================================================================
// Functions
// ===================================================================================
void reset_Score_TTRIS(void) {
    for (uint8_t x = 0; x < 3; x++) {
        Nb_of_line_TTRIS[x] = 0;
    }
    Level_TTRIS = 0;
    Scores_TTRIS = 0;
    Nb_of_line_F_TTRIS = 0;
}

uint8_t PSEUDO_RND_TTRIS(void) {
    RND_VAR_TTRIS = (RND_VAR_TTRIS < 6) ? RND_VAR_TTRIS + 1 : 0;
    return RND_VAR_TTRIS;
}

void SND_TTRIS(uint8_t Snd_TTRIS) {
    uint8_t q;
    switch (Snd_TTRIS) {
    case 0:
        JOY_sound(3, 5);
        JOY_sound(10, 10);
        JOY_sound(3, 5);
        break;
    case 1:
        JOY_sound(3, 2);
        break;
    case 2:
        for (q = 1; q < 10; q++) {
            JOY_sound(40, 80);
            JOY_sound(150, 80);
        }
        break;
    case 3:
        for (q = 100; q > 10; q--) {
            JOY_sound(q * 2, 6);
            JOY_sound(q, 12);
        }
        break;
    case 4:
        JOY_sound(20, 150);
        JOY_sound(100, 150);
        break;
    case 5:
        for (q = 0; q < 126; q++) {
            JOY_sound(q * 2, 1);
        }
        break;
    default:
        break;
    }
}

void INTRO_MANIFEST_TTRIS(void) {
    uint8_t TIMER_1 = 0;
    recupe_HIGHSCORE_TTRIS();
    Convert_Nb_of_line_TTRIS();
    Flip_intro_TTRIS(&TIMER_1);
    while (1) {
        PIECEs_TTRIS = PSEUDO_RND_TTRIS();
        if (JOY_act_pressed()) {
            reset_Score_TTRIS();
            break;
        }
        JOY_DLY_ms(33);
        TIMER_1 = (TIMER_1 < 7) ? TIMER_1 + 1 : 0;
        Flip_intro_TTRIS(&TIMER_1);
        JOY_idle();
        if (JOY_exit()) {
            return;
        }
    }
    SND_TTRIS(4);
}

void END_DROP_TTRIS(void) {
    uint8_t x, y;
    DROP_BREAK_TTRIS = 0;
    for (y = 0; y < 5; y++) {
        for (x = 0; x < 5; x++) {
            if (Piece_Mat2_TTRIS[x][y] == 1) {
                CHANGE_GRID_STAT_TTRIS(OU_SUIS_JE_X_TTRIS + (x), OU_SUIS_JE_Y_TTRIS + (y), 1);
            }
        }
    }
    Scores_TTRIS = (OU_SUIS_JE_Y_TTRIS < 9) ? Scores_TTRIS + 2 : Scores_TTRIS + 1;
    yy_TTRIS = 0;
    xx_TTRIS = 0;
    DELETE_LINE_TTRIS();
    Convert_Nb_of_line_TTRIS();
}

void SETUP_NEW_PREVIEW_PIECE_TTRIS(uint8_t *Rot_TTRIS) {
    PIECEs_TTRIS_PREVIEW = PSEUDO_RND_TTRIS();
    Select_Piece_TTRIS(PIECEs_TTRIS);
    *Rot_TTRIS = 0;
    rotate_Matrix_TTRIS(*Rot_TTRIS);
}

void CONTROLE_TTRIS(uint8_t *Rot_TTRIS) {
    if ((OU_SUIS_JE_X_ENGAGED_TTRIS == 0)) {
        if (SPEED_x_trig_TTRIS == 0) {
            if (JOY_right_pressed()) {
                if (LONG_PRESS_X_TTRIS == 0) {
                    SND_TTRIS(1);
                }
                if ((LONG_PRESS_X_TTRIS == 0) || (LONG_PRESS_X_TTRIS == 20)) {
                    DEPLACEMENT_XX_TTRIS = 1;
                    SPEED_x_trig_TTRIS = 2;
                }
                if (LONG_PRESS_X_TTRIS < 20) {
                    LONG_PRESS_X_TTRIS++;
                }
            }
            if (JOY_left_pressed()) {
                if (LONG_PRESS_X_TTRIS == 0) {
                    SND_TTRIS(1);
                }
                if ((LONG_PRESS_X_TTRIS == 0) || (LONG_PRESS_X_TTRIS == 20)) {
                    DEPLACEMENT_XX_TTRIS = -1;
                    SPEED_x_trig_TTRIS = 2;
                }
                if (LONG_PRESS_X_TTRIS < 20) {
                    LONG_PRESS_X_TTRIS++;
                }
            }
        } else {
            SPEED_x_trig_TTRIS = (SPEED_x_trig_TTRIS > 0) ? SPEED_x_trig_TTRIS - 1 : 0;
        }
    }
    if ((JOY_right_pressed() == 0) && (JOY_left_pressed() == 0)) {
        LONG_PRESS_X_TTRIS = 0;
        PSEUDO_RND_TTRIS();
    }

    if (JOY_act_released()) {
        if ((OU_SUIS_JE_X_ENGAGED_TTRIS == 0) && (OU_SUIS_JE_Y_ENGAGED_TTRIS == 0)) {
            Ripple_filter_TTRIS = 0;
        }
    }
    if ((Ripple_filter_TTRIS == 1)) {
        CHECK_if_Rot_Ok_TTRIS(Rot_TTRIS);
        Ripple_filter_TTRIS = 2;
    }
    if (OU_SUIS_JE_Y_ENGAGED_TTRIS == 0) {
        DROP_TRIG_TTRIS--;
        if (DROP_TRIG_TTRIS == 0) {
            DEPLACEMENT_YY_TTRIS = 1;
            DROP_TRIG_TTRIS = Level_Speed_ADJ_TTRIS;
        }
    }
    if (DROP_SPEED_TTRIS > 0) {
        DROP_SPEED_TTRIS--;
    } else {
        DROP_SPEED_TTRIS = Level_Speed_ADJ_TTRIS;
    }
    if (JOY_down_pressed()) {

        // ajouter cest 2 ligne
        if (OU_SUIS_JE_X_ENGAGED_TTRIS == 0) {
            DEPLACEMENT_XX_TTRIS = 0;
            LONG_PRESS_X_TTRIS = 1;
        } // ajouter cest 2 ligne

        PSEUDO_RND_TTRIS();
        if (DOWN_DESACTIVE_TTRIS == 0) {
            DEPLACEMENT_YY_TTRIS = 1;
            DROP_SPEED_TTRIS = 0;
        }
    } else {
        DOWN_DESACTIVE_TTRIS = 0;
    }
}

void Game_Play_TTRIS(void) {
    uint8_t LEVEL_TMP = (Nb_of_line_F_TTRIS / 20);
    if (Level_TTRIS != LEVEL_TMP) {
        Level_TTRIS = LEVEL_TMP;
        SND_TTRIS(2);
    }
    if (Level_TTRIS < 21) {
        Level_Speed_ADJ_TTRIS = map(Level_TTRIS, 0, 20, 11, 1);
    }
}

uint8_t End_Play_TTRIS(void) {
    for (uint8_t t = 0; t < 12; t++) {
        if (GRID_STAT_TTRIS(t, 1) == 1) {
            return 1;
        }
    }
    return 0;
}

void DELETE_LINE_TTRIS(void) {
    uint8_t LOOP, SCAN_Line, ADDs_Block, OK_DELETE = 0;
    uint8_t LINE_MEM[19] = {0};
    uint8_t Nb_of_Line_temp = 0;
    for (LOOP = 0; LOOP < 19; LOOP++) {
        ADDs_Block = 1;
        for (SCAN_Line = 0; SCAN_Line < 12; SCAN_Line++) {
            if (GRID_STAT_TTRIS(SCAN_Line, LOOP) == 0) {
                ADDs_Block = 0;
            }
        }
        if (ADDs_Block) {
            LINE_MEM[LOOP] = 1;
            OK_DELETE = 1;
        }
    }
    if (OK_DELETE == 1) {
        FLASH_LINE_TTRIS(&LINE_MEM[0]);
        Clean_Grid_TTRIS(&LINE_MEM[0]);
    }
    for (LOOP = 0; LOOP < 19; LOOP++) {
        if (LINE_MEM[LOOP] == 1) {
            Nb_of_Line_temp++;
        }
    }
    Nb_of_line_F_TTRIS = Nb_of_line_F_TTRIS + Nb_of_Line_temp;
    Scores_TTRIS = (Scores_TTRIS + Calcul_of_Score_TTRIS(Nb_of_Line_temp));
}

uint8_t Calcul_of_Score_TTRIS(uint8_t Tmp_TTRIS) {
    switch (Tmp_TTRIS) {
    case 0:
        return 0;
        break;
    case 1:
        return 2;
        break;
    case 2:
        return 5;
        break;
    case 3:
        return 8;
        break;
    case 4:
        return 12;
        break;
    default:
        return 0;
        break;
    }
}

void FLASH_LINE_TTRIS(uint8_t *PASS_LINE) {
    uint8_t LOOP;
    for (LOOP = 0; LOOP < 5; LOOP++) {
        PAINT_LINE_TTRIS(1, &PASS_LINE[0]);
        Tiny_Flip_TTRIS(82);

        PAINT_LINE_TTRIS(0, &PASS_LINE[0]);
        Tiny_Flip_TTRIS(82);
    }
    SND_TTRIS(5);
}

void PAINT_LINE_TTRIS(uint8_t VISIBLE, uint8_t *PASS_LINE) {
    uint8_t LOOP, SCAN_Line;
    for (LOOP = 0; LOOP < 19; LOOP++) {
        if (PASS_LINE[LOOP] == 1) {
            for (SCAN_Line = 0; SCAN_Line < 12; SCAN_Line++) {
                CHANGE_GRID_STAT_TTRIS(SCAN_Line, LOOP, VISIBLE);
            }
        }
    }
}

void Clean_Grid_TTRIS(uint8_t *PASS_LINE) {
    uint8_t GRID_2 = 18, GRID_1 = 18;
    uint8_t x;
    while (1) {
        if (PASS_LINE[GRID_1] == 1) {
            GRID_2 = GRID_1;
            GRID_2 = (GRID_2 > 0) ? GRID_2 - 1 : 0;
            break;
        }
        GRID_1 = (GRID_1 > 0) ? GRID_1 - 1 : 0;
    }
    while (1) {
        while (1) {
            if (PASS_LINE[GRID_2] == 1) {
                GRID_2 = (GRID_2 > 0) ? GRID_2 - 1 : 0;
            } else {
                break;
            }
        }
        for (x = 0; x < 12; x++) {
            CHANGE_GRID_STAT_TTRIS(x, GRID_1, (GRID_2 > 0) ? GRID_STAT_TTRIS(x, GRID_2) : 0);
        }
        GRID_1 = (GRID_1 > 0) ? GRID_1 - 1 : 0;
        GRID_2 = (GRID_2 > 0) ? GRID_2 - 1 : 0;
        if (GRID_1 == 0) {
            break;
        }
    }
}

uint8_t CHECK_if_Rot_Ok_TTRIS(uint8_t *Rot_TTRIS) {
    uint8_t Mem_rot = *Rot_TTRIS;
    Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
    *Rot_TTRIS = (*Rot_TTRIS < PIECEs_rot_TTRIS) ? *Rot_TTRIS + 1 : 0;
    rotate_Matrix_TTRIS(*Rot_TTRIS);

    if ((Check_collision_x_TTRIS(OU_SUIS_JE_X_ENGAGED_TTRIS) ||
         (Check_collision_y_TTRIS(OU_SUIS_JE_Y_ENGAGED_TTRIS))) != 0) {
        *Rot_TTRIS = Mem_rot;
        rotate_Matrix_TTRIS(*Rot_TTRIS);
        return 1;
    }
    SND_TTRIS(0);
    return 0;
}

uint8_t Check_collision_x_TTRIS(int8_t x_Axe) {
    uint8_t x, y;
    for (y = 0; y < 5; y++) {
        for (x = 0; x < 5; x++) {
            if (Piece_Mat2_TTRIS[x][y] == 1) {
                if (GRID_STAT_TTRIS((x + OU_SUIS_JE_X_TTRIS) + x_Axe, y + OU_SUIS_JE_Y_TTRIS)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

uint8_t Check_collision_y_TTRIS(int8_t y_Axe) {
    uint8_t x, y;
    for (y = 0; y < 5; y++) {
        for (x = 0; x < 5; x++) {
            if (Piece_Mat2_TTRIS[x][y] == 1) {
                if (GRID_STAT_TTRIS(x + OU_SUIS_JE_X_TTRIS, (y + OU_SUIS_JE_Y_TTRIS) + y_Axe)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void Move_Piece_TTRIS(void) {
    Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
    if (OU_SUIS_JE_X_ENGAGED_TTRIS == 0) {
        if (Check_collision_x_TTRIS(DEPLACEMENT_XX_TTRIS)) {
            DEPLACEMENT_XX_TTRIS = 0;
        }
    }
    if (DEPLACEMENT_XX_TTRIS == 1) {
        xx_TTRIS++;
    }
    if (DEPLACEMENT_XX_TTRIS == -1) {
        xx_TTRIS--;
    }
    Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
    if (OU_SUIS_JE_X_ENGAGED_TTRIS == 0) {
        DEPLACEMENT_XX_TTRIS = 0;
    }

    if (Check_collision_y_TTRIS(DEPLACEMENT_YY_TTRIS)) {
        DEPLACEMENT_YY_TTRIS = 0;
        LONG_PRESS_X_TTRIS = 0;
        Ripple_filter_TTRIS = 0;
        DROP_BREAK_TTRIS = 6;
        Tiny_Flip_TTRIS(82); // add line for refresh screen at drop
    } else {
        DROP_BREAK_TTRIS = 0;
    }
    if (DROP_SPEED_TTRIS == 0) {
        if (DEPLACEMENT_YY_TTRIS == -1) {
            yy_TTRIS--;
        }
        if (DEPLACEMENT_YY_TTRIS == 1) {
            yy_TTRIS++;
        }
    }
    Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
    if (OU_SUIS_JE_Y_ENGAGED_TTRIS == 0) {
        DEPLACEMENT_YY_TTRIS = 0;
    }
}

void Ou_suis_Je_TTRIS(int8_t xx_, int8_t yy_) {
    int8_t xx_t, yy_t;
    xx_t = (((xx_) + 9) - 46);
    yy_t = (((yy_) + 9) - 5);
    OU_SUIS_JE_X_TTRIS = ((xx_t / 3) - 3);
    if ((xx_t % 3) != 0) {
        OU_SUIS_JE_X_ENGAGED_TTRIS = 1;
    } else {
        OU_SUIS_JE_X_ENGAGED_TTRIS = 0;
    }
    OU_SUIS_JE_Y_TTRIS = ((yy_t / 3) - 3);
    if ((yy_t) != ((OU_SUIS_JE_Y_TTRIS + 3) * 3)) {
        OU_SUIS_JE_Y_ENGAGED_TTRIS = 1;
    } else {
        OU_SUIS_JE_Y_ENGAGED_TTRIS = 0;
    }
}

void Select_Piece_TTRIS(uint8_t Piece_) {
    PIECEs_TTRIS = Piece_;
    switch (Piece_) {
    case 0:
        PIECEs_rot_TTRIS = 3;
        break;
    case 1:
        PIECEs_rot_TTRIS = 0;
        break;
    case 2:
    case 3:
    case 4:
        PIECEs_rot_TTRIS = 1;
        break;
    case 5:
    case 6:
        PIECEs_rot_TTRIS = 3;
        break;
    default:
        PIECEs_rot_TTRIS = 0;
        break;
    }
}

void rotate_Matrix_TTRIS(uint8_t ROT) {
    uint8_t a_ = 0, b_ = 0;
    for (uint8_t y = 0; y < 5; y++) {
        for (uint8_t x = 0; x < 5; x++) {
            switch (ROT) {
            case 0:
                a_ = x, b_ = y;
                break;
            case 1:
                a_ = 4 - y, b_ = x;
                break;
            case 2:
                a_ = 4 - x, b_ = 4 - y;
                break;
            case 3:
                a_ = y, b_ = 4 - x;
                break;
            default:
                break;
            }
            Piece_Mat2_TTRIS[a_][b_] = Scan_Piece_Matrix_TTRIS(x, y + (PIECEs_TTRIS * 5));
        }
    }
}

uint8_t Scan_Piece_Matrix_TTRIS(int8_t x_Mat, int8_t y_Mat) {
    uint8_t Result = (0b10000000 >> x_Mat) & (Pieces_TTRIS[y_Mat]);
    if (Result) {
        return 1;
    } else {
        return 0;
    }
    return 0;
}

uint8_t GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN) {
    if (Y_SCAN < 0) return 0;
    if ((X_SCAN < 0) || (X_SCAN > 11)) {
        return 1;
    }
    if (Y_SCAN > 18) {
        return 1;
    }
    uint8_t Y_VAR_SELECT = Y_SCAN >> 3;
    uint8_t Y_VAR_DECALAGE = RecupeDecalageY_TTRIS(Y_SCAN);
    uint8_t COMP_BYTE_DECALAGE = (0b10000000 >> Y_VAR_DECALAGE);
    if ((COMP_BYTE_DECALAGE & Grid_TTRIS[X_SCAN][Y_VAR_SELECT]) == 0) {
        return 0;
    } else {
        return 1;
    }
    return 0;
}

uint8_t CHANGE_GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN, uint8_t VALUE) {
    if ((X_SCAN < 0) || (X_SCAN > 11)) return 0;
    if ((Y_SCAN < 0) || (Y_SCAN > 18)) return 0;
    uint8_t Y_VAR_SELECT = Y_SCAN >> 3;
    uint8_t Y_VAR_DECALAGE = RecupeDecalageY_TTRIS(Y_SCAN);
    uint8_t COMP_BYTE_DECALAGE = (0b10000000 >> Y_VAR_DECALAGE);
    if (VALUE) {
        Grid_TTRIS[X_SCAN][Y_VAR_SELECT] = COMP_BYTE_DECALAGE | Grid_TTRIS[X_SCAN][Y_VAR_SELECT];
    } else {
        Grid_TTRIS[X_SCAN][Y_VAR_SELECT] = (0xff - COMP_BYTE_DECALAGE) & Grid_TTRIS[X_SCAN][Y_VAR_SELECT];
    }
    return 0;
}

uint8_t blitzSprite_TTRIS(int8_t xPos, int8_t yPos, uint8_t xPASS, uint8_t yPASS, uint8_t FRAME,
                          const uint8_t *SPRITES) {
    uint8_t OUTBYTE;
    uint8_t WSPRITE = ((SPRITES[0]));
    uint8_t HSPRITE = ((SPRITES[1]));
    uint8_t Wmax = ((HSPRITE * WSPRITE) + 1);
    uint16_t PICBYTE = FRAME * (Wmax - 1);
    uint8_t RECUPELINEY = RecupeLineY_TTRIS(yPos);
    if ((xPASS > ((xPos + (WSPRITE - 1)))) || (xPASS < xPos) ||
        ((RECUPELINEY > yPASS) || ((RECUPELINEY + (HSPRITE)) < yPASS))) {
        return 0x00;
    }
    uint8_t SPRITEyLINE = (yPASS - (RECUPELINEY));
    uint8_t SPRITEyDECALAGE = (RecupeDecalageY_TTRIS(yPos));
    uint8_t ScanA = (((xPASS - xPos) + (SPRITEyLINE * WSPRITE)) + 2);
    uint8_t ScanB = (((xPASS - xPos) + ((SPRITEyLINE - 1) * WSPRITE)) + 2);
    if (ScanA > Wmax) {
        OUTBYTE = 0x00;
    } else {
        OUTBYTE = SplitSpriteDecalageY_TTRIS(SPRITEyDECALAGE, (SPRITES[ScanA + (PICBYTE)]), 1);
    }
    if ((SPRITEyLINE > 0)) {
        uint8_t OUTBYTE2 = SplitSpriteDecalageY_TTRIS(SPRITEyDECALAGE, (SPRITES[ScanB + (PICBYTE)]), 0);
        if (ScanB > Wmax) {
            return OUTBYTE;
        } else {
            return OUTBYTE | OUTBYTE2;
        }
    } else {
        return OUTBYTE;
    }
}

uint8_t H_grid_Scan_TTRIS(uint8_t xPASS) { return (H_Grid_TTTRIS[xPASS - 46]); }

uint8_t Recupe_TTRIS(uint8_t xPASS, uint8_t yPASS) {
    uint8_t BYTE_TTRIS = 0;
    uint8_t x = 0;
    for (uint8_t y = MEM_TTTRIS[(yPASS << 1)]; y < MEM_TTTRIS[(yPASS << 1) + 1]; y++) {
        if ((xPASS > 45) && (xPASS < 82)) {
            x = H_grid_Scan_TTRIS(xPASS);
        } else {
            return (RECUPE_BACKGROUND_TTRIS(xPASS, yPASS) | NEXT_BLOCK_TTRIS(xPASS, yPASS) |
                    recupe_Nb_of_line_TTRIS(xPASS, yPASS) | recupe_SCORES_TTRIS(xPASS, yPASS) |
                    recupe_LEVEL_TTRIS(xPASS, yPASS));
        }
        if (GRID_STAT_TTRIS(x, y) == 1) {
            BYTE_TTRIS = BYTE_TTRIS | blitzSprite_TTRIS(46 + (x * 3), 5 + (y * 3), xPASS, yPASS, 0, tinyblock_TTTRIS);
        }
    }
    return RECUPE_BACKGROUND_TTRIS(xPASS, yPASS) | BYTE_TTRIS | DropPiece_TTRIS(xPASS, yPASS);
}

uint8_t NEXT_BLOCK_TTRIS(uint8_t xPASS, uint8_t yPASS) {
    if (xPASS > 89) {
        uint8_t Byte_Mem = 0;
        uint8_t x_add = 0;
        uint8_t y_add = 0;
        switch (PIECEs_TTRIS_PREVIEW) {
        case 0:
            x_add = 1;
            y_add = 1;
            break;
        case 1:
            y_add = -1;
            break;
        case 2:
            x_add = 1;
            break;
        case 3:
            break;
        case 4:
            x_add = 1;
            y_add = 1;
            break;
        case 5:
            x_add = 1;
            break;
        case 6:
            break;
        default:
            break;
        }
        for (uint8_t y = 0; y < 5; y++) {
            for (uint8_t x = 0; x < 5; x++) {
                if (Scan_Piece_Matrix_TTRIS(x, y + (PIECEs_TTRIS_PREVIEW * 5)) == 1) {
                    Byte_Mem |= blitzSprite_TTRIS(92 + (x * 2) + x_add, (27 + (y * 2)) - 5 + y_add, xPASS, yPASS, 0,
                                                  tiny_PREVIEW_block_TTTRIS);
                }
            }
        }
        return Byte_Mem;
    }
    return 0;
}

uint8_t RECUPE_BACKGROUND_TTRIS(uint8_t xPASS, uint8_t yPASS) { return (BACKGROUND_TTRIS[xPASS + (yPASS * 128)]); }

uint8_t DropPiece_TTRIS(uint8_t xPASS, uint8_t yPASS) {
    uint8_t Byte_Mem = 0;
    for (uint8_t y = 0; y < 5; y++) {
        for (uint8_t x = 0; x < 5; x++) {
            if (Piece_Mat2_TTRIS[x][y] == 1) {
                Byte_Mem |=
                    blitzSprite_TTRIS(xx_TTRIS + (x * 3), (yy_TTRIS + (y * 3)) - 5, xPASS, yPASS, 0, tinyblock2_TTTRIS);
            }
        }
    }
    return Byte_Mem;
}

uint8_t SplitSpriteDecalageY_TTRIS(uint8_t decalage, uint8_t Input, uint8_t UPorDOWN) {
    if (UPorDOWN) {
        return Input << decalage;
    }
    return Input >> (8 - decalage);
}

uint8_t RecupeLineY_TTRIS(uint8_t Valeur) { return (Valeur >> 3); }

uint8_t RecupeDecalageY_TTRIS(uint8_t Valeur) { return (Valeur - ((Valeur >> 3) << 3)); }

void Tiny_Flip_TTRIS(uint8_t HR_TTRIS) {
    uint8_t y, x;
    for (y = 0; y < 8; y++) {
        JOY_OLED_data_start(y);
        for (x = 0; x < HR_TTRIS; x++) {
            JOY_OLED_send(Recupe_TTRIS(x, y));
        }
        JOY_OLED_end();
    }
}

void Flip_intro_TTRIS(uint8_t *TIMER1) {
    uint8_t y, x;
    for (y = 0; y < 8; y++) {
        JOY_OLED_data_start(y);
        for (x = 0; x < 128; x++) {
            JOY_OLED_send(intro_TTRIS(x, y, TIMER1));
        }
        JOY_OLED_end();
    }
}

uint8_t intro_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1) {
    return (RECUPE_BACKGROUND_TTRIS(xPASS, yPASS) | recupe_Chateau_TTRIS(xPASS, yPASS) |
            Recupe_Start_TTRIS(xPASS, yPASS, TIMER1) | recupe_SCORES_TTRIS(xPASS, yPASS) |
            recupe_Nb_of_line_TTRIS(xPASS, yPASS) | recupe_SCORES_TTRIS(xPASS, yPASS) |
            recupe_LEVEL_TTRIS(xPASS, yPASS));
}

uint8_t Recupe_Start_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1) {
    if (*TIMER1 > 3) {
        return blitzSprite_TTRIS(49, 28, xPASS, yPASS, 0, start_button_1_TTRIS) |
               blitzSprite_TTRIS(49, 36, xPASS, yPASS, 0, start_button_2_TTRIS);
    } else {
        return 0;
    }
}

uint8_t recupe_Chateau_TTRIS(uint8_t xPASS, uint8_t yPASS) {
    if (xPASS < 46) return 0;
    if (xPASS > 81) return 0;
    return (chateau_TTRIS[(xPASS - 46) + (yPASS * 36)]);
}

uint8_t recupe_SCORES_TTRIS(uint8_t xPASS, uint8_t yPASS) {
    if (xPASS < 95) {
        return 0;
    }
    if (xPASS > 119) {
        return 0;
    }
    if (yPASS > 1) {
        return 0;
    }
#define M10000 (Scores_TTRIS / 10000)
#define M1000 (((Scores_TTRIS) - (M10000 * 10000)) / 1000)
#define M100 (((Scores_TTRIS) - (M1000 * 1000) - (M10000 * 10000)) / 100)
#define M10 (((Scores_TTRIS) - (M100 * 100) - (M1000 * 1000) - (M10000 * 10000)) / 10)
#define M1 ((Scores_TTRIS) - (M10 * 10) - (M100 * 100) - (M1000 * 1000) - (M10000 * 10000))
    return (blitzSprite_TTRIS(95, 8, xPASS, yPASS, M10000, police_TTRIS) |
            blitzSprite_TTRIS(99, 8, xPASS, yPASS, M1000, police_TTRIS) |
            blitzSprite_TTRIS(103, 8, xPASS, yPASS, M100, police_TTRIS) |
            blitzSprite_TTRIS(107, 8, xPASS, yPASS, M10, police_TTRIS) |
            blitzSprite_TTRIS(111, 8, xPASS, yPASS, M1, police_TTRIS) |
            blitzSprite_TTRIS(115, 8, xPASS, yPASS, 0, police_TTRIS));
}

void Convert_Nb_of_line_TTRIS(void) {
    Nb_of_line_TTRIS[2] = (Nb_of_line_F_TTRIS / 100);
    Nb_of_line_TTRIS[1] = ((Nb_of_line_F_TTRIS - (Nb_of_line_TTRIS[2] * 100)) / 10);
    Nb_of_line_TTRIS[0] = (Nb_of_line_F_TTRIS - (Nb_of_line_TTRIS[2] * 100) - (Nb_of_line_TTRIS[1] * 10));
}

uint8_t recupe_Nb_of_line_TTRIS(uint8_t xPASS, uint8_t yPASS) {
    if (xPASS < 16) {
        return 0;
    }
    if (xPASS > 28) {
        return 0;
    }
    if (yPASS > 1) {
        return 0;
    }
    return (blitzSprite_TTRIS(16, 8, xPASS, yPASS, Nb_of_line_TTRIS[2], police_TTRIS) |
            blitzSprite_TTRIS(20, 8, xPASS, yPASS, Nb_of_line_TTRIS[1], police_TTRIS) |
            blitzSprite_TTRIS(24, 8, xPASS, yPASS, Nb_of_line_TTRIS[0], police_TTRIS));
}

uint8_t recupe_LEVEL_TTRIS(uint8_t xPASS, uint8_t yPASS) {
    if (xPASS < 109) {
        return 0;
    }
    if (xPASS > 118) {
        return 0;
    }
    if (yPASS != 5) {
        return 0;
    }
    return (blitzSprite_TTRIS(109, 41, xPASS, yPASS, (Level_TTRIS / 10), police_TTRIS) |
            blitzSprite_TTRIS(114, 41, xPASS, yPASS, (Level_TTRIS % 10), police_TTRIS));
}

void INIT_ALL_VAR_TTRIS(void) {
    for (uint8_t y = 0; y < 3; y++) {
        for (uint8_t x = 0; x < 12; x++) {
            Grid_TTRIS[x][y] = 0;
        }
    }
    for (uint8_t y = 0; y < 5; y++) {
        for (uint8_t x = 0; x < 5; x++) {
            Piece_Mat2_TTRIS[x][y] = 0;
        }
    }
    LONG_PRESS_X_TTRIS = 0;
    DOWN_DESACTIVE_TTRIS = 0;
    DROP_SPEED_TTRIS = 0;
    SPEED_x_trig_TTRIS = 0;
    DROP_TRIG_TTRIS = 1;
    xx_TTRIS = 0;
    yy_TTRIS = 0;
    Ripple_filter_TTRIS = 0;
    PIECEs_TTRIS = 0;
    PIECEs_TTRIS_PREVIEW = 0;
    PIECEs_rot_TTRIS = 0;
    DROP_BREAK_TTRIS = 0;
    OU_SUIS_JE_X_TTRIS = 0;
    OU_SUIS_JE_Y_TTRIS = 0;
    OU_SUIS_JE_X_ENGAGED_TTRIS = 0;
    OU_SUIS_JE_Y_ENGAGED_TTRIS = 0;
    DEPLACEMENT_XX_TTRIS = 0;
    DEPLACEMENT_YY_TTRIS = 0;
}

#define Number_of_Backup 4

void recupe_HIGHSCORE_TTRIS(void) {
    /*
    uint8_t RESAVE=0;
    uint8_t t;

    uint8_t Level_SUM[Number_of_Backup]={0};
    uint8_t line_SUM[Number_of_Backup]={0};
    uint8_t Scores_SUM[Number_of_Backup]={0};

    //recupe data eeprom
    #define Level_(tt) EEPROM.read(1+(tt*10))
    #define line_A(tt) EEPROM.read(2+(tt*10))
    #define line_B(tt) EEPROM.read(3+(tt*10))
    #define Scores_A(tt) EEPROM.read(4+(tt*10))
    #define Scores_B(tt) EEPROM.read(5+(tt*10))

    for (t=0;t<Number_of_Backup;t++){
    Level_SUM[t]=EEPROM.read(7+(t*10));
    line_SUM[t]=EEPROM.read(8+(t*10));
    Scores_SUM[t]=EEPROM.read(9+(t*10));
    }
    //fin

    //SUM check
    for (t=0;t<Number_of_Backup;t++){
    if ((checksum(Level_(t)))!=(Level_SUM[t])) {Level_SUM[t]=255;RESAVE=1;}
    if ((checksum(line_A(t))+checksum(line_B(t)))!=(line_SUM[t])) {line_SUM[t]=255;RESAVE=1;}
    if ((checksum(Scores_A(t))+checksum(Scores_B(t)))!=(Scores_SUM[t])) {Scores_SUM[t]=255;RESAVE=1;}
    }
    //SUM check fin

    //Get Level
    for (t=0;t<Number_of_Backup;t++){
    if (Level_SUM[t]!=255) {
    Level_TTRIS=Level_(t);
      goto Next_2;}
    }
    goto ED;
    Next_2:;
    //get level fin

    //Get Lines
    for (t=0;t<Number_of_Backup;t++){
    if (line_SUM[t]!=255) {
    Nb_of_line_F_TTRIS=uint16_t (line_A(t)<<8);
    Nb_of_line_F_TTRIS|=uint16_t (line_B(t));
    goto Next_3;}
    }
    goto ED;
    Next_3:;
    //get Lines fin

    //Get Scores
    for (t=0;t<Number_of_Backup;t++){
    if (Scores_SUM[t]!=255) {
    Scores_TTRIS=uint16_t (Scores_A(t)<<8);
    Scores_TTRIS|=uint16_t (Scores_B(t));
    goto Next_4;}
    }
    goto ED;
    //get Scores fin

    ED:;
    Reset_Value_TTRIS();
    Next_4:;
    if (RESAVE) {save_HIGHSCORE_TTRIS();} //resave if sumcheck not pass
    */
    Reset_Value_TTRIS();
}

void Reset_Value_TTRIS(void) {
    Level_TTRIS = 0;
    Nb_of_line_F_TTRIS = 0;
    Scores_TTRIS = 0;
}

void save_HIGHSCORE_TTRIS(void) {
    /*
    uint8_t LEVEL_=Level_TTRIS&0xff;
    uint8_t LINE_0=(Nb_of_line_F_TTRIS>>8) & 0xFF;
    uint8_t LINE_1=Nb_of_line_F_TTRIS&0xff;
    uint8_t Scores_0=(Scores_TTRIS>>8) & 0xFF;
    uint8_t Scores_1=Scores_TTRIS&0xff;
    uint8_t t;
    for (t=0;t<Number_of_Backup;t++){
    EEPROM.write(1+(t*10),LEVEL_);
    EEPROM.write(2+(t*10),LINE_0);
    EEPROM.write(3+(t*10),LINE_1);
    EEPROM.write(4+(t*10),Scores_0);
    EEPROM.write(5+(t*10),Scores_1);
    EEPROM.write(7+(t*10),checksum(LEVEL_));
    EEPROM.write(8+(t*10),(checksum(LINE_0)+checksum(LINE_1)));
    EEPROM.write(9+(t*10),(checksum(Scores_0)+checksum(Scores_1)));
    }
    */
}

void Check_NEW_RECORD(void) {
    /*
    if (Scores_TTRIS>uint16_t((EEPROM.read(4)<<8)|(EEPROM.read(5)))) {
    save_HIGHSCORE_TTRIS();
    }
    */
}

uint8_t checksum(uint8_t Byte_) {
    uint8_t Total_ = 0;
    uint8_t t = 0;
    for (t = 0; t < 8; t++) {
        if ((Byte_) & (0b00000001 << t)) {
            Total_++;
        }
    }
    return Total_;
}
