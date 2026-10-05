/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"



void unique_act_set(PLW *pl) {
    f32 sp90[3];
    u16 sp9C;
    u8 sp9F;
    f32 sp80[4];
    f32 sp70[4];
    s32 sp60[4];
    f32 sp20[16];
    int r;

    r = St_unique_ck(pl, sp90, &sp9C, &sp9F) & 0xFFFF;
    switch (r) {
    default:
        if (Sansai_talk_ck(pl) & 0xFF) {
            Pl_act_set2(pl, 0, 0x5B, 0);
        }
        return;
    case 1:
        flvecCopy(&pl->work800, sp90);
        Pl_adj_calc(pl, 0x14);
        pl->ang_y = sp9C;
        Pl_act_set2(pl, 0, 0x4C, 0);
        return;
    case 3:
        if (pl->work88C == 0) {
            Pl_act_set2(pl, 0, 0x57, 0);
            return;
        }
        break;
    case 4:
        pl->ang_y = sp9C;
        Pl_act_set2(pl, 0, 0x5C, 0);
        return;
    case 16:
        flvecCopy(&pl->work800, sp90);
        sp60[0] = 0;
        sp60[2] = 0;
        sp60[1] = sp9C;
        cpRotMatrix(sp60, sp20);
        SetVector(120.0f * bed_ofs[pl->id], 0, 0, sp70);
        flvecApplyMat33(sp80, sp70, sp20);
        pl->work800 = pl->work800 + sp80[0];
        pl->work804 = pl->work804 + sp80[1];
        pl->work808 = pl->work808 + sp80[2];
        pl->ang_y = sp9C;
        Pl_adj_calc(pl, 0x14);
        Pl_act_set2(pl, 0, 0x33, 0);
        return;
    case 25:
        if (game_w.x1B2 == 0) {
            pl->ang_y = (u16)((calc_vec_ang2(pl->pos, sp90) & 0xFFFF) - 0x4000);
            Pl_act_set2(pl, 0, 0x56, 0);
            return;
        }
        break;
    case 21:
        Share_item_conv(pl);
        break;
    }
}

void job_special_com_ck(PLW *pl, u8 mode) {
    u8 t;

    if (pl->sw.now & 0x80) {
        if (mode == 0 || mode == 2) {
            switch (pl->kind) {
            case 0:
                if (mode == 2) {
                    t = pl->work56B;
                    if (t & 0xF) {
                        pl->work56B = t & 0xF0;
                        func_549200(pl, 4);
                    }
                }
                Pl_act_set2(pl, 2, 3, 0);
                return;
            case 4:
            case 3:
                if (mode == 2) {
                    t = pl->work56B;
                    if (t & 0xF) {
                        pl->work56B = t & 0xF0;
                        func_549200(pl, 4);
                    }
                }
                Pl_act_set2(pl, 2, 0x17, 0);
                return;
            case 2:
                if (mode == 2) {
                    t = pl->work56B;
                    if (t & 0xF) {
                        pl->work56B = t & 0xF0;
                        func_549200(pl, 4);
                    }
                }
                Pl_act_set2(pl, 1, 0x18, 0);
                return;
            }
        } else {
            switch (pl->kind) {
            case 0:
            case 4:
            case 3:
                Pl_act_set2(pl, 2, 3, 0);
                return;
            case 2:
                Pl_act_set2(pl, 1, 0x18, 0);
                break;
            }
        }
    }
}

void search_act_set(PLW *pl, u8 mode) {
    u16 sp3E;
    f32 sp30[3];
    u16 id;

    if (Sansai_talk_ck(pl) & 0xFF) {
        if (mode == 2) {
            goto block_3;
        }
    } else {
block_3:
        if ((St_pick_ck(pl, &sp3E, sp30) & 0xFFFF) != 0xFFFF) {
            switch (sp3E) {
            case 0:
                if (mode != 2) {
                    if (mode == 0) {
                        Pl_act_set(pl, 0, 0x47, 0);
                    } else {
                        Pl_act_set(pl, 0, 0x5D, 0);
                    }
                } else {
                    goto block_24;
                }
                break;
            case 3:
                if (mode == 2) {
                    switch (pl->item[pl->work888].id) {
                    case 0x83:
                    case 0x84:
                    case 0x85:
                        pl->ang_y = (s16)((calc_vec_ang2(pl->pos, sp30) & 0xFFFF) - 0x4000);
                        pl->work88A = pl->item[pl->work888].id;
                        Pl_act_set(pl, 0, 0x51, 0);
                        break;
                    case 0x86:
                    case 0x87:
                    case 0x88:
                        se_req(7, 0x15, 0);
                        break;
                    }
                }
                break;
            case 4:
                if (mode == 2) {
                    id = pl->item[pl->work888].id;
                    switch (id) {
                    case 0x86:
                    case 0x87:
                    case 0x88:
                        pl->work88A = id;
                        Pl_act_set(pl, 0, 0x52, 0);
                        break;
                    case 0x83:
                    case 0x84:
                    case 0x85:
                        se_req(7, 0x15, 0);
                        break;
                    }
                }
                break;
            }
        } else {
block_24:
            switch (pl->item[pl->work888].id) {
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
                if (mode == 2) {
                    se_req(7, 0x15, 0);
                }
                break;
            }
        }
        if (mode != 2 && (Ext_pick_point_ck(pl) & 0xFFFF) != 0xFFFF) {
            if (mode == 0) {
                Pl_act_set(pl, 0, 0x4A, 0);
                return;
            }
            Pl_act_set(pl, 0, 0x60, 0);
        }
    }
}

s32 sit_com_ck(PLW *pl) {
    u8 temp_v1;

    if ((Pl_master_ck(pl) == 0) && (Online_ck() == 1)) {
        return 0;
    }
    switch (game_w.x0D5) {
    case 4:
        if (pl->work88D == 0) {
            Pl_act_set2(pl, 4, 2, 0);
            return 0;
        }
    case 6:
    case 5:
        Pl_act_set2(pl, 4, 3, 0);
        return 0;
    case 7:
    case 8:
        Pl_act_set2(pl, 4, 5, 0);
        return 0;
    default:
        pl->work8F0 = 1;
        temp_v1 = stick_pow_get(pl, 0);
        switch (temp_v1) {
        case 1:
        case 2:
            pl->ang_y = stick_dir_set(pl, 0);
            if (act_ck(pl, 0, 0x3B) == 0) {
                if (act_ck(pl, 0, 0x3D) != 0) {
                    Pl_act_set2(pl, 0, 0x3C, 0);
                } else {
                    Pl_act_set(pl, 0, 0x3B, 0);
                }
            }
            break;
        case 3:
            pl->ang_y = stick_dir_set(pl, 0);
            if (act_ck(pl, 0, 0x3D) == 0) {
                Pl_act_set(pl, 0, 0x3D, 0);
            }
            break;
        default:
        case 0:
            if (act_ck(pl, 0, 0x3B) == 0) {
                if (act_ck(pl, 0, 0x3D) != 0) {
                    goto block_31;
                }
            } else {
block_31:
                if (pl->work08 == 0) {
                    Pl_act_set2(pl, 0, 0x3C, 0);
                }
            }
            break;
        }
        if ((pl->sw.trg & 0x40) && (pl->x714 == 0)) {
            Pl_act_set2(pl, 0, 0x39, 0);
        }
        if (Game_clear_ck(1) == 1) {
            return 0;
        }
        if (pl->work8C4 != 0) {
            Pl_chat_act_set(pl);
        }
        if (pl->sw.an_trg & 0x3C) {
            Pl_act_set(pl, 0, 4, 0);
        }
        job_special_com_ck(pl, 0);
        if ((pl->sw.trg & 0x200) && (pl->work886 == 0)) {
            search_act_set(pl, 2);
            item_action_set(pl, 0);
            pl->work8F2 = (u8) (pl->work8F2 | 1);
        }
        if (pl->sw.trg & 0x20) {
            if (trade_get_ck_00139680(pl) == 0) {
                unique_act_set(pl);
                search_act_set(pl, 1);
            }
        } else if (pl->work908 != 0) {
            pl->work90B = 1;
            Pl_act_set2(pl, 0, 0x67, 0x10);
        }
        return 0;
    }
}
