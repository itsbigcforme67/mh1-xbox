/* Player code (SLPM_654.95 0x0014A6B0-0x0014AA44): egg_com_ck, the player's command checks while holding an egg/item
   (stage 5 = carrying): action changes by stick, buttons (0x40 dash/roll, d-pad, 0x200) and the unique-spot button.
   The two `return;` after the dash branch (not `break;`) keep the original's shared exit stub. */
/* Player code (f_pl.s, 0x134950..): working file; matched functions are moved
 * to plX.c, what is left here is near-match (not built). */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];


void egg_com_ck(PLW *pl, int arg1) {
    u8 sp3F;
    u16 sp3C;
    f32 sp30[3];
    s32 u;
    u16 v;

    if (Pl_master_ck(pl) != 0) {
        switch (game_w.x0D5) {
        case 6:
        case 5:
            Pl_act_set2(pl, 4, 3, 0);
            break;
        case 7:
        case 8:
            Pl_act_set2(pl, 4, 5, 0);
            break;
        default:
            if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xFFFF) {
                Pl_act_set2(pl, 5, 3, 0);
                break;
            }
            if (pl->sw.pow[0] >= 0x28) {
                pl->ang_y = stick_dir_set(pl, 0);
                if ((pl->sw.now & 0x10) && (pl->work760 == 0)) {
                    if (act_ck(pl, 5, 2) == 0) {
                        Pl_act_set2(pl, 5, 2, 0);
                    }
                } else if ((act_ck(pl, 5, 1) == 0) && (pl->work760 == 0)) {
                    Pl_act_set2(pl, 5, 1, 0);
                }
            } else if (pl_flag_ck(pl, 0x02001200) != 0) {
                if ((s16)arg1 == 1) {
                    if (pl->work08 == 0) {
                        goto go;
                    }
                } else {
go:
                    Pl_act_set2(pl, 5, 0, 0);
                }
            }
            if (Game_clear_ck(1) != 1) {
                v = pl->sw.trg;
                if (v & 0x40) {
                    if (pl->sw.pow[0] >= 0x55) {
                        if (pl->stamina >= 0x4B) {
                            Pl_act_set2(pl, 0, 0x1C, 4);
                        }
                    } else {
                        Pl_act_set2(pl, 0, 8, 0);
                    }
                    return;
                }
                if (pl->sw.an_trg & 0x3C) {
                    Pl_act_set2(pl, 0, 4, 0);
                    break;
                }
                if (v & 0x200) {
                    Pl_act_set2(pl, 5, 3, 0);
                }
                job_special_com_ck(pl, 2);
                if (pl->sw.trg & 0x20) {
                    u = St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF;
                    switch (u) {
                    case 21:
                        Share_item_conv(pl);
                        if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xFFFF) {
                            Pl_act_set2(pl, 5, 0xA, 0);
                            return;
                        }
                        break;
                    case 24:
                        if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xA3) {
                            pl->work56B = pl->work56B & 0xF0;
                            Pl_item_stack(pl, 0xA3, -0xA);
                            pl->ang_y = sp3C;
                            pl->cnt39A = sp3F;
                            Pl_act_set2(pl, 0, 0x37, 0x20);
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
}

