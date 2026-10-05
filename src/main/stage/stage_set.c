/* stage_set - SLPM_654.95 0x0015BBE0-0x0015C210 (first function of the
 * f_stage range; the rest is in f_stage.c, not linked yet).
 * stage_set_set: spawns each stage's set objects (props and effects) when a
 * stage is entered. The set*_set functions live in the game.bin overlay, so
 * main calls them by address (func_XXXXXX; names from config/symbols/game.txt
 * in the comments). Jump table 0x0035B870-0x0035B9A0. */
#include "set.h"
#include "game.h"

void clr_flash(void);
void Set13_set(int arg);
void func_6213F0(void);                 /* set00_set: light shafts */
void func_61ED10(void);                 /* Set03_set */
void func_61F2C0(void);                 /* Set04_set */
void func_61F7E0(int stage);            /* Set05_set */
void func_6206D0(void);                 /* set07_set */
void func_620B70(void);                 /* set08_set */
void func_618CA0(void);                 /* set09_set */
void func_621C70(void);                 /* Set10_set */
void func_622180(void);                 /* set11_set */
void func_6229B0(void);                 /* set14_set: UV-scrolled water */
void func_624050(void);                 /* set15_set */
void func_625120(void);                 /* set16_set */
void func_625630(void);                 /* set17_set */
void func_625BB0(void);                 /* Set18_set */
void func_6263B0(void);                 /* Set19_set */
void func_626E70(int kind);             /* Set20_set */
void func_627840(void);                 /* Set22_set */
void func_633B50(f32 *pos, int a, int stage, int b);  /* Shell10_set */
void func_54B8C0(f32 *pos, int kind);   /* Eft14_set2 */

void stage_set_set(int stage) {
    f32 pos[3];
    s16 i;

    clr_flash();
    switch ((u8)stage) {
    case 0:
    case 0x1A:
        func_625120();
        func_620B70();
        func_6229B0();
        func_624050();
        break;
    case 1:
        func_6229B0();
        func_625630();
        break;
    case 2:
        func_625630();
        break;
    case 3:
        func_6229B0();
        func_625630();
        break;
    case 4:
        func_6213F0();
        Set13_set(0);
        func_6229B0();
        break;
    case 5:
        Set13_set(0);
        func_618CA0();
        break;
    case 6:
    case 7:
        func_620B70();
        func_625120();
        break;
    case 0xA:
        func_621C70();
        break;
    case 0xB:
        Set13_set(3);
        func_6263B0();
        func_61F7E0(game_w.stage);
        break;
    case 0xC:
        func_61F7E0(stage);
        Set13_set(3);
        func_6263B0();
        break;
    case 0xE:
        func_61F2C0();
        Set13_set(3);
        break;
    case 0x10:
        func_618CA0();
        break;
    case 0x11:
        for (i = 0; i < 10; i++) {
            pos[0] = 1400.0f + 300.0f * (f32)i;
            pos[1] = 30.0f;
            pos[2] = 2000.0f + 300.0f * (f32)i;
            func_633B50(pos, 0, game_w.stage, 0);
        }
        break;
    case 0x12:
    case 0x16:
    case 0x17:
        Set13_set(0);
        func_625BB0();
        break;
    case 0x13:
        Set13_set(0);
        break;
    case 0x14:
        func_626E70(0);
        break;
    case 0x15:
        pos[0] = 11250.0f;
        pos[1] = 40.0f;
        pos[2] = 9500.0f;
        func_54B8C0(pos, 2);
        func_54B8C0(pos, 3);
        Set13_set(4);
        func_6263B0();
        break;
    case 0x19:
        func_61F7E0(stage);
        func_626E70(1);
        func_627840();
        break;
    case 0x1C:
        func_61F7E0(stage);
        func_61F2C0();
        Set13_set(3);
        func_6263B0();
        func_622180();
        break;
    case 0x1E:
        func_6263B0();
        Set13_set(3);
        func_61F7E0(game_w.stage);
        break;
    case 0x1F:
        Set13_set(3);
        break;
    case 0x20:
        func_618CA0();
        Set13_set(0);
        break;
    case 0x1B:
    case 0x21:
        func_618CA0();
        Set13_set(0);
        break;
    case 0x18:
    case 0x22:
        Set13_set(1);
        func_6263B0();
        break;
    case 0x23:
        Set13_set(6);
        break;
    case 0x24:
        func_618CA0();
        break;
    case 0x25:
        Set13_set(0);
        break;
    case 0x26:
        Set13_set(0);
        break;
    case 0x1D:
    case 0x27:
        Set13_set(0);
        break;
    case 0x28:
        Set13_set(0);
        func_618CA0();
        break;
    case 0x29:
        func_6213F0();
        func_618CA0();
        break;
    case 0x2A:
        func_6213F0();
        func_6206D0();
        Set13_set(0);
        func_6229B0();
        break;
    case 0x2B:
        func_6213F0();
        break;
    case 0x2D:
        Set13_set(7);
        break;
    case 0x2E:
        func_625630();
        break;
    case 0x2F:
        func_61ED10();
        Set13_set(0);
        break;
    case 0x30:
        func_61ED10();
        Set13_set(0);
        func_6263B0();
        break;
    case 0x31:
        func_61ED10();
        Set13_set(0);
        break;
    case 0x32:
        Set13_set(0);
        break;
    case 0x33:
        Set13_set(0);
        func_618CA0();
        func_6229B0();
        break;
    case 0x34:
        Set13_set(0);
        func_6229B0();
        func_6263B0();
        func_618CA0();
        break;
    case 0x35:
        Set13_set(0);
        func_6229B0();
        func_618CA0();
        break;
    case 0x36:
        func_6263B0();
        break;
    case 0x37:
        Set13_set(0);
        func_618CA0();
        break;
    case 0x38:
        Set13_set(7);
        break;
    case 0x39:
        Set13_set(0);
        func_6263B0();
        break;
    case 0x3D:
        pos[0] = 3600.0f;
        pos[2] = 6600.0f;
        pos[1] = 0.0f;
        func_54B8C0(pos, 2);
        func_54B8C0(pos, 3);
        Set13_set(0);
        func_6263B0();
        break;
    case 0x3E:
        Set13_set(0);
        break;
    case 0x43:
        pos[0] = 10296.0f;
        pos[2] = 9429.0f;
        pos[1] = 0.0f;
        func_54B8C0(pos, 2);
        func_54B8C0(pos, 3);
        func_6263B0();
        break;
    case 0x46:
        Set13_set(6);
        func_625BB0();
        break;
    case 0x49:
    case 0x4B:
        Set13_set(6);
        func_6263B0();
        break;
    }
}
