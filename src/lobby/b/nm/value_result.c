#include "lobby_a.h"

/* armor upgrade flags: bits 0-3 level, 4/5 = two toggles (exclusive pair), 6 = flag; op 0 = level up (max 4), 1-6 set/clear (near-match: 3 instructions off, the compare lands in at instead of v0) */
s32 value_result(s32 v, s32 op) {
    s32 t;
    s32 lv;
    u16 r;

    switch (op & 0xFFFF) {
    case 0:
        t = v & 0xFFFF;
        lv = t & 0xF;
        r = 4;
        if (lv >= 4) {
        } else {
            r = lv + 1;
        }
        v = ((t & 0x70) | (r & 0xFFFF)) & 0xFFFF;
        break;
    case 1:
        v = (v | 0x10) & 0xFFFF;
        v = v & 0xFFDF & 0xFFFF;
        break;
    case 2:
        v = v & 0xFFEF & 0xFFFF;
        break;
    case 3:
        v = (v | 0x20) & 0xFFFF;
        v = v & 0xFFEF & 0xFFFF;
        break;
    case 4:
        v = v & 0xFFDF & 0xFFFF;
        break;
    case 5:
        v = (v | 0x40) & 0xFFFF;
        break;
    case 6:
        v = v & 0xFFBF & 0xFFFF;
        break;
    }
    return v;
}
