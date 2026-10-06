/* Player code (SLPM_654.95 0x0014AD80-0x0014AF14): pl_egg03, the pick-up/throw/put action of an egg or item (arg1 0 pick up,
   1 and 2 variants): starts the motion, waits `frame` frames (0x72 or 4), then takes the carried item out of the stack (arg1 0 and master). */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];

void pl_egg03(PLW *pl, s32 arg1) {
    s32 v;
    s32 id;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 1) {
            pl_chr_set2(pl, 0x37, 4, 0);
        } else {
            pl_chr_set2(pl, 0x36, 4, 0);
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        v = (arg1 == 1) ? 4 : 0x72;
        if (frame_check((f32)v, pl, 0) != 0) {
            pl->x05++;
            pl->work56B = pl->work56B & 0xF0;
            if (arg1 != 2) {
                func_549200(pl, 4);
            }
            if ((arg1 == 0) && (Pl_master_ck(pl) == 1)) {
                id = Pl_hold_item_ck(pl) & 0xFFFF;
                if (id != 0xFFFF) {
                    set01_set(1, 0xC, (s16)id);
                    Pl_item_stack(pl, id, -0x64);
                }
            }
            break;
        }
        egg_set(pl);
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}
