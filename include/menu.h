#ifndef MENU_H
#define MENU_H
/* Pit menu (cockpit menu, f_menu in main.bin). Field names are guesses from
 * how the menu code uses them; offsets are exact. */
#include "types.h"
#include "flow.h"

typedef struct PIT_W {              /* pit_work, 0x90 bytes; lpPit points at it */
    u8 _pad00[4];
    u8 x04;             /* 0x04 (cleared together with 5-7 by a word store in Pit_init) */
    s8 x05;             /* 0x05 cleared by menu_exit */
    u8 x06;             /* 0x06 */
    u8 x07;             /* 0x07 non-zero while a pit menu is open (Cockpit_menu_chk) */
    PLW *pl;            /* 0x08 player the menu belongs to (game_w.master) */
    s16 key;            /* 0x0C keys pressed this frame (pit_key_repeat) */
    s8 rep;             /* 0x0E key repeat countdown */
    u8 yn;              /* 0x0F yes/no cursor (select_yes_no) */
    s16 x10[4];         /* 0x10 */
    u8 _pad18[4];
    s32 time0;          /* 0x1C Quest_time_get(0) */
    s32 time1;          /* 0x20 Quest_time_get(1) */
    u8 _pad24[4];
    s8 x28;             /* 0x28 */
    s8 x29;             /* 0x29 */
    u8 _pad2A;
    s8 x2B;             /* 0x2B */
    f32 map_sx;         /* 0x2C 1 / map width */
    f32 map_sy;         /* 0x30 1 / map height */
    s16 x34[4];         /* 0x34 */
    s16 x3C;            /* 0x3C */
    u8 _pad3E[0x40 - 0x3E];
    s8 x40;             /* 0x40 */
    s8 x41;             /* 0x41 */
    u8 _pad42[0x52 - 0x42];
    s16 x52;            /* 0x52 */
    u8 _pad54[0x5A - 0x54];
    u16 x5A;            /* 0x5A */
    u8 _pad5C[0x5E - 0x5C];
    u16 x5E;            /* 0x5E */
    u8 _pad60[0x80 - 0x60];
    s8 x80;             /* 0x80 */
    u8 _pad81[3];
    s8 x84;             /* 0x84 */
    u8 _pad85[4];
    s8 lb;              /* 0x89 1 in the lobby */
    u8 x8A;             /* 0x8A game_w+0x1DD */
    u8 x8B;             /* 0x8B game_w+0x0F */
    s8 x8C;             /* 0x8C option_w+3 */
    s8 x8D;             /* 0x8D */
    u8 _pad8E[2];
} PIT_W;

typedef struct PIT_MENU {           /* PitMenu, 0x1764 bytes */
    s32 x00;            /* 0x00 */
    u8 _pad04[2];
    s8 x06;             /* 0x06 */
    u8 _pad07;
    s32 x08;            /* 0x08 */
    s16 x0C;            /* 0x0C */
    u8 _pad0E;
    s8 x0F;             /* 0x0F */
    s8 x10;             /* 0x10 */
    u8 _pad11[3];
    u8 x14;             /* 0x14 */
    s8 x15;             /* 0x15 */
    s8 x16;             /* 0x16 */
    s8 x17;             /* 0x17 */
    s8 x18;             /* 0x18 */
    u8 _pad19[4];
    u8 open;            /* 0x1D non-zero while the menu is open (Cockpit_menu_chk) */
    u8 _pad1E[0x1764 - 0x1E];
} PIT_MENU;

typedef struct PIT_PRIM {           /* pit_prim: three display layers */
    u8 _pad00[0x14];
    void (*trans)();    /* 0x14 */
    u8 _pad18[8];
} PIT_PRIM;

extern PIT_W pit_work;
extern PIT_W *lpPit;
extern PIT_MENU PitMenu;
extern PIT_PRIM pit_prim[];

#endif
