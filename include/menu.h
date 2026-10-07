#ifndef MENU_H
#define MENU_H
/* Pit menu (cockpit menu, f_menu in main.bin). Field names are guesses from
 * how the menu code uses them; offsets are exact. */
#include "types.h"
#include "flow.h"

typedef struct PIT_W {              /* pit_work, 0x90 bytes; lpPit points at it */
    u8 _pad00[0x4 - 0x0];
    u8 x04;           /* 0x04 cleared with 5-7 by a word store (Pit_init) */
    u8 x05;           /* 0x05 cleared by menu_exit */
    u8 x06;           /* 0x06  */
    u8 x07;           /* 0x07 non-zero while a pit menu is open (Cockpit_menu_chk) */
    PLW *pl;         /* 0x08 player the menu belongs to (game_w.master) */
    s16 key;          /* 0x0C keys pressed this frame (pit_key_repeat) */
    s8 rep;           /* 0x0E key repeat countdown */
    u8 yn;            /* 0x0F yes/no cursor (select_yes_no) */
    s16 x10;          /* 0x10  */
    u16 x12;          /* 0x12  */
    s16 x14;          /* 0x14  */
    s16 x16;          /* 0x16  */
    u8 _pad18[0x1C - 0x18];
    s32 time0;        /* 0x1C Quest_time_get(0) */
    s32 time1;        /* 0x20 Quest_time_get(1) */
    s16 x24;          /* 0x24  */
    s16 x26;          /* 0x26  */
    s8 x28;           /* 0x28  */
    s8 x29;           /* 0x29  */
    s8 x2A;           /* 0x2A */
    s8 x2B;           /* 0x2B  */
    f32 map_sx;       /* 0x2C 1 / map width */
    f32 map_sy;       /* 0x30 1 / map height */
    s16 x34;          /* 0x34  */
    s16 x36;          /* 0x36  */
    s16 x38;          /* 0x38  */
    s16 x3A;          /* 0x3A  */
    s16 x3C;          /* 0x3C  */
    u8 x3E;           /* 0x3E  */
    u8 x3F;           /* 0x3F  */
    u8 x40;           /* 0x40  */
    u8 x41;           /* 0x41  */
    u8 x42;           /* 0x42  */
    u8 x43;           /* 0x43  */
    u8 x44;           /* 0x44  */
    u8 x45;           /* 0x45  */
    u8 x46;           /* 0x46  */
    u8 x47;           /* 0x47  */
    u8 x48;           /* 0x48  */
    u8 x49;           /* 0x49  */
    u8 x4A;           /* 0x4A  */
    u8 x4B;           /* 0x4B  */
    s8 x4C;           /* 0x4C  */
    u8 x4D;           /* 0x4D  */
    u8 x4E;           /* 0x4E  */
    u8 x4F;           /* 0x4F */
    s16 x50;          /* 0x50  */
    s16 x52;          /* 0x52  */
    u8 x54;           /* 0x54  */
    u8 x55;           /* 0x55 */
    u8 x56;           /* 0x56 */
    s8 x57;           /* 0x57  */
    s8 x58;           /* 0x58  */
    s8 x59;           /* 0x59  */
    u16 x5A;          /* 0x5A  */
    s8 x5C;           /* 0x5C  */
    s8 x5D;           /* 0x5D  */
    u16 x5E;          /* 0x5E  */
    s16 x60;          /* 0x60  */
    u8 x62;           /* 0x62 */
    u8 x63;           /* 0x63 */
    u8 x64;           /* 0x64  */
    u8 x65;           /* 0x65 */
    s16 x66;          /* 0x66 */
    s32 x68;          /* 0x68  */
    u16 x6C;          /* 0x6C  */
    u16 x6E;          /* 0x6E  */
    u16 x70;          /* 0x70  */
    s16 x72;          /* 0x72 */
    s16 x74;          /* 0x74  */
    s16 x76;          /* 0x76  */
    s16 x78;          /* 0x78  */
    u8 x7A;           /* 0x7A  */
    u8 x7B;           /* 0x7B  */
    s8 x7C;           /* 0x7C  */
    u8 x7D;           /* 0x7D  */
    u8 x7E;           /* 0x7E  */
    u8 x7F;           /* 0x7F  */
    s8 x80;           /* 0x80  */
    s8 x81;           /* 0x81  */
    s8 x82;           /* 0x82  */
    u8 x83;           /* 0x83  */
    s8 x84;           /* 0x84  */
    s8 x85;           /* 0x85 mix effect kind (mix_effect_set) */
    s16 x86;          /* 0x86 mix effect frame counter */
    u8 x88;           /* 0x88  */
    u8 lb;            /* 0x89 1 in the lobby */
    u8 x8A;           /* 0x8A game_w+0x1DD */
    u8 x8B;           /* 0x8B game_w+0x0F */
    u8 x8C;           /* 0x8C option_w+3 */
    u8 x8D;           /* 0x8D  */
    u8 _pad8E[0x90 - 0x8E];
} PIT_W;

/* One chat log entry (PitMenu.log[64], 0x5D bytes; f_chat). */
typedef struct PIT_CHAT {
    char text[2][0x1F]; /* 0x00 message split into two display lines */
    u8 nline;           /* 0x3E number of display lines used */
    u8 who;             /* 0x3F player number (0xFF: plaza) */
    u8 col[4];          /* 0x40 colours */
    char uid[8];        /* 0x44 user id */
    char name[0x11];    /* 0x4C handle */
} PIT_CHAT;

typedef struct PIT_MENU {           /* PitMenu, 0x1764 bytes */
    s32 x00;            /* 0x00 */
    s16 x04;            /* 0x04 (f_chat NPC_Message char count) */
    s8 x06;             /* 0x06 */
    s8 x07;             /* 0x07 (f_chat) */
    s32 x08;            /* 0x08 */
    u16 x0C;            /* 0x0C */
    s8 x0E;             /* 0x0E (f_chat) */
    s8 x0F;             /* 0x0F */
    u8 x10;             /* 0x10 */
    u8 x11;             /* 0x11 */
    u16 x12;            /* 0x12 */
    u8 x14;             /* 0x14 */
    u8 x15;             /* 0x15 */
    u8 x16;             /* 0x16 */
    s8 x17;             /* 0x17 */
    s8 x18;             /* 0x18 */
    u8 x19;             /* 0x19 (f_chat) */
    u8 x1A;             /* 0x1A (u8: Join_pl_chk counter) */
    s8 x1B;             /* 0x1B (s8: lb in Pit_disp_chat_cnfg) */
    u8 _pad1C;
    u8 open;            /* 0x1D non-zero while the menu is open (Cockpit_menu_chk) */
    u8 logtop;          /* 0x1E chat log write index (f_chat) */
    u8 lognum;          /* 0x1F chat log entries used */
    u8 logscr;          /* 0x20 chat log scroll */
    u8 x21;             /* 0x21 */
    u8 x22;             /* 0x22 chat log arrow flags (f_chat) */
    PIT_CHAT log[64];   /* 0x23 */
    u8 _pad1763;
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
