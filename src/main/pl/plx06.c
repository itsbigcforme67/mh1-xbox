/* plx06 - Pl_hold_item_ck (SLPM_654.95 0x00154BD0-0x00154C68): item id of the last held tool in the 20 item slots (one of 0x91-0x95, 0xA3), 0xFFFF if none.
   The permuter's `volatile u16` return type is what gives the original's unmasked return; plf.h declares it s32, so the name is renamed around
   the include (no header edit). Whole file in pl_nm.c. */
#define Pl_hold_item_ck Pl_hold_item_ck_proto_unused
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
#undef Pl_hold_item_ck

volatile u16 Pl_hold_item_ck(PLW *pl) {
    s16 i;
    u16 r = 0xFFFF;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].num != 0) {
            switch (pl->item[i].id) {
            case 0x91:
            case 0x92:
            case 0xA3:
            case 0x94:
            case 0x93:
            case 0x95:
                r = pl->item[i].id;
                break;
            default:
                break;
            }
        }
    }
    return r;
}
