/* Player code (SLPM_654.95 0x00154C70-0x00154CF8): Check_hold_item */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

u8 Check_hold_item(s32 id) {
    switch ((u16)id) {
    case 0x91:
        return 1;
    case 0x92:
        return 2;
    case 0xA3:
        return 3;
    case 0x94:
        return 4;
    case 0x93:
        return 5;
    case 0x95:
        return 6;
    default:
        return 0;
    }
}
