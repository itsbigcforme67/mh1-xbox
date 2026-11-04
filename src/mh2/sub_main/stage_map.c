/* Monster Hunter 2: the stage maps (sub_main 0x3799A0, 0x379A30-0x379BB4, main 0x18DA70). Written from our own disassembly of the
   owner's disc (docs/mh2/em_cmd.md section 3). MH2 has two forms of many areas (two stage numbers); two per-stage tables
   of 200 entries tie them: D_00440E30 (s16) is the stage's form class (0, 1, or 2 = one form only) and D_00440FC0 (u16)
   the other form's stage, or 0xFFFF. What the forms are (day / night?) is a guess, not verified. 0xFFFF is "no stage".
   Names are the symbol files' (no real names known). The PC builds this file for MH2 (tools/build_pc.sh); the tables
   and the byte come from config/mh2/pc_data.txt there. The MH2 build takes sub_main 0x379A30-0x379BB8 from it (config/mh2/c_files.txt). */
#include "types.h"
#include "game.h"

u8 func_0018DA70(void);

extern s16 D_00440E30[];        /* per stage: form class 0 / 1 / 2 */
extern u16 D_00440FC0[];        /* per stage: the other form's stage, 0xFFFF = none */
extern u16 D_0043ED80[];        /* the rank table: u16 per em_cmd level, 0xFFFF = the level itself */
extern u8 D_005D4910[];         /* bits 3 / 4: which form is in use (setter main 0x18DAA0) */

/* 1 when s is no stage or not a class-1 stage */
int sub_main_func_00379A30(u16 s) {
    if (s == 0xFFFF) {
        return 1;
    }
    if (D_00440E30[s] != 1) {
        return 1;
    }
    return 0;
}

/* the other form's stage, or s when it has none */
u16 sub_main_func_00379A70(u16 s) {
    u16 o;

    if (s == 0xFFFF) {
        return 0xFFFF;
    }
    o = D_00440FC0[s];
    if (o != 0xFFFF) {
        return o;
    }
    return s;
}

/* the group: a class-1 stage counts as its other form, every other stage as itself */
u16 sub_main_func_00379AC0(u16 s) {
    if (s == 0xFFFF) {
        return 0xFFFF;
    }
    if (sub_main_func_00379A30(s) == 0) {
        return sub_main_func_00379A70(s);
    } else {
        return s;
    }
}

/* the form of stage s that is in use now: class d (0 when game_w.x1DC, MH2 game_w+0x41, is set) or the other form */
u16 sub_main_func_00379B20(u16 s) {
    s16 d;

    if (s == 0xFFFF) {
        return 0xFFFF;
    }
    d = game_w.x1DC == 0 ? func_0018DA70() : 0;
    if (d != D_00440E30[s]) {
        return sub_main_func_00379A70(s);
    }
    return s;
}

/* main 0x18DA70 and sub_main 0x3799A0 are outside this file's range in the MH2 build (their bytes stay in the asm there) */
#ifndef __MWERKS__
/* main 0x18DA70: the form in use, 0 (bit 3, or neither bit) or 1 (bit 4) */
u8 func_0018DA70(void) {
    if (D_005D4910[0] & 8) {
        return 0;
    }
    return (D_005D4910[0] & 0x10) != 0;
}

/* sub_main 0x3799A0: the em_cmd level n (game_w+0x30) as a rank */
int sub_main_func_003799A0(u16 n) {
    u16 v = D_0043ED80[n];

    if (v != 0xFFFF) {
        return v;
    }
    return n;
}
#endif
