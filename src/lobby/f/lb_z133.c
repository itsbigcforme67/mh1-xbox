/* lb_z133 - auto-drafted 0x005D78C0-0x005D7950: Lb_reset (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char D_3E5468[];

void Lb_reset(void) {
    u8 temp_v1;

    temp_v1 = game_w.stage;
    if (temp_v1 != 0x4C) {
        if (temp_v1 == 0x4D) {
            goto block_3;
        }
    } else {
block_3:
        Info_Initialization();
    }
    push_em_work_all();
    stage_free();
    clr_move_work();
    ot_init();
    prim_init();
    flCompact();
    *(int *)((u8 *)&D_3E5468 + (game_w.master * 0xA00)) = 0;
}
