/* Player code (SLPM_654.95 0x0014D1D0-0x0014D318): pl_chr_sub (character frame step) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_chr_sub(PLW *pl) {
    u16 c;
    u8 k;

    if (softdip_ck(0x18) == 0) {
        cpRotMatrix((s32 *)pl->ang, (f32 *)((u8 *)pl + 0x20));
        if (pl->x40A != 0) {
            pl->chr_spd0 = 0.0f;
            pl->chr_spd1 = 0.0f;
        } else if (pl->x610 > 0) {
            pl->x610 = pl->x610 - 1;
            pl->chr_spd0 = 0.2f;
            pl->chr_spd1 = 0.2f;
        } else {
            pl->x610 = 0;
            c = pl->char0;
            if (c != 3) {
                k = pl->kind;
                if (!((k == 1 || k == 5) && c == 0x57C)) {
                    pl->chr_spd0 = 2.0f;
                    pl->chr_spd1 = 2.0f;
                } else if (pl->chr_spd0 <= 1.5f) {
                    pl->chr_spd0 = 1.5f;
                    pl->chr_spd1 = 1.5f;
                }
            }
        }
        if (pl->x2FC[0] == 0) {
            pl->x2FC[0]++;
            frame_init(pl, pl->act_tm0, pl->blend0, 0);
        }
        if (pl->x2FC[1] == 0) {
            pl->x2FC[1]++;
            frame_init(pl, pl->act_tm1, pl->blend1, 1);
        }
        frame_move(pl);
    }
}
