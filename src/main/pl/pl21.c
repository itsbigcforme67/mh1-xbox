/* Player code (SLPM_654.95 0x0013F1B0-0x0013F568): pl_mv068 (jump over an obstacle), pick_set_sub (pick up an item/point: stock request and pouch selection) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv068(PLW *pl) {
    f32 sp30[3];
    f32 sp20[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x2E, 4, 0);
        Pl_basic_flagset(pl, 2, 0, 0);
        Pl_stamina_calc(pl, -0x4B);
        pl->work40C = 3;
        break;
    case 1:
        pl->work40C = 3;
        if (pl->work194 == 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x2F, 0, 0);
            pl->pos[1] = pl->pos[1] + 100.0f;
            sp30[2] = 16.0f;
            sp30[0] = 0.0f;
            sp30[1] = 0.0f;
            flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x60));
            pl->vel[0] = sp20[0];
            pl->vel[1] = 3.0f;
            pl->vel[2] = sp20[2];
            rate_clear_g(pl);
            rate_g_calc(pl, 8);
        }
        break;
    case 2:
        pl->work40C = 3;
        rate_add_g(pl);
        if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= pl->x5AC)) {
            pl->x05++;
            pl->st = 0;
            pl->pos[1] = pl->x5AC;
            pl_chr_set2(pl, 0x30, 0, 0);
            vib_set_pl(pl, 1);
            Pl_set_quake_sub(pl, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl->work40C = 0;
            if (pl->sw.now & 0x10) {
                Pl_act_set2(pl, 0, 0x45, 0);
                break;
            }
            pl->work8F0 = 1;
            Pl_act_set2(pl, 0, 0x2C, 0);
            break;
        }
        if (frame_check2(30.0f, pl, 0) == 0) {
            pl->work40C = 2;
        }
        break;
    }
}

void pick_set_sub(PLW *pl, u16 arg1) {
    s32 r;
    s16 q;

    if (Game_clear_ck(1) != 1) {
        if (arg1 == 0) {
            r = St_pick_ck2(pl);
        } else {
            r = Ext_pick_point_ck2(pl);
        }
        r &= 0xFFFF;
        if ((r == 0) || (r == 0xFFFE) || (r == 0xFFFF)) {
            if (r != 0xFFFE) {
                if (arg1 == 0) {
                    set01_set(0, 0, 0);
                    return;
                }
                set01_set(0, 0xE, 0);
                return;
            }
            adx_se_set(pl, 8);
            set01_set2(lit_3557);
            return;
        }
        q = ItemStockRequest(pl, r, pl->x8E6, 3);
        switch (q) {
        case 0:
        case 1:
        case 2:
            if (Check_hold_item(r) & 0xFF) {
                pl->x07 = 1;
            }
            if (pl->item[pl->work888].id != 0 && Item_data[pl->item[pl->work888].id][1] != 1) {
                pl->work888 = item_sel_sub(pl, pl->work888, 0);
            }
            break;
        }
    }
}
