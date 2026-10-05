/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

EMW *pull_enemy_work(void);
void enemy_mv(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void PlComebackCameraRequest(void);















void pl_sw_set(void);
void hit_timer_calc_shl(void);
void pl_move_sub(PLW *);





void pl_move_sub(PLW *pl) {
    f32 sp3C;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    int sp20;
    f32 temp_f0;
    f32 temp_f2;
    s32 temp_v1;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a1;
    u8 temp_a2;
    u8 temp_v1_2;
    u8 temp_v1_3;

    if (PU8(pl, 0) != 0) {
        em_ninshiki_ck();
        pl_timer_calc(pl);
        pl->work8F2 = 0;
        pl->work90B = 0;
        if (pl->work8C2 == 0) {
            pl_item_sel(pl);
            pl_shell_sel(pl);
        }
        pl->work5A0 = (f32) pl->pos[0];
        pl->work5A4 = (f32) pl->pos[1];
        pl->work5A8 = (f32) pl->pos[2];
        pl->work608 = -1;
        pl->work60A = -1;
        pl->work601 = 0;
        pl->work3F4 = 0;
        pl->work8ED = 0;
        pl->work8F0 = 0;
        pl->work917 = 0;
        if (Pl_master_ck(pl) == 1) {
            if ((((int) (act_ck(pl, 0, 9) << 0x30) >> 0x30) == 0) && (act_ck(pl, 0, 0x1B) == 0)) {
                pl->work937 = 0;
            }
        } else {
            pl->work937 = 0;
        }
        func_63A260(pl);
        temp_v1 = pl_status_ck(pl) & 0xFF;
        switch (temp_v1) {                          /* irregular */
        case 1:
            Pl_act_set2();
            break;
        case 2:
            if ((((int) (act_ck() << 0x30) >> 0x30) == 0) && (((int) (act_ck(pl, 2, 0x18) << 0x30) >> 0x30) == 0)) {
                Pl_act_set2(pl, 2, 0x16, 0);
            }
            break;
        }
        Pl_atck_adj_calc(pl);
        Pl_def_adj_calc(pl);
        if (pl->x40A == 0) {
            pl_dm_value_sub(pl);
            pl_move_sub_sub(pl);
            if (pl->work6FF != 0) {
                pl_move_sub_sub(pl);
                pl->work6FF = 0U;
            }
            pl_turn_sub(pl);
            pl_horm_sub(pl);
            if (pl->flag14 != 4) {
                Pl_pos_adj(pl);
            }
        }
        Pl_status_set(pl);
        pl->work8C4 = 0;
        pl->work908 = 0;
        hit_stop_calc(pl);
        if (act_ck(pl, 0, 0x27) == 0) {
            pl_chr_sub(pl);
        }
        hit_timer_calc(pl);
        (*(int (**)(void *) *)((u8 *)(*(void * *)((u8 *)((u8 *)pl + 0x3CC))) + 0xC))(pl);
        if (((act_ck(pl, 0, 0x1B) == 0) || (act_ck(pl, 0, 0x1E) == 0)) && (Pl_stg_ck(pl) & 0xFF)) {
            HitWallPlayer(pl, 0);
        }
        if (pl->flag14 == 0) {
            temp_v1_2 = pl->flag15;
            if ((u32) (temp_v1_2 - 1) >= 3U) {
                if (temp_v1_2 == 0x13) {
                    goto block_33;
                }
            } else {
block_33:
                if (Pl_stg_ck(pl) & 0xFF) {
                    GetFloorSlide(pl, &sp20, 1);
                }
            }
        }
        St_unique_adr_set(pl);
        if ((GetGroundHitStatusAreaPl(pl, ((u8 *)pl + 0xAC), ((u8 *)pl + 0x70C), &sp3C) == 1) && (((int) (act_ck(pl, 4, 0) << 0x30) >> 0x30) == 0)) {
            pl->x5AC = sp3C;
        }
        if (Pl_stg_ck(pl) & 0xFF) {
            GetPlayerMaterialData(pl);
        }
        if ((pl->st != 2) && (pl->flag604 == 0) && (act_ck(pl, 0, 0x3A) == 0) && (pl->flag14 != 4) && (Pl_stg_ck(pl) & 0xFF)) {
            temp_f2 = pl->x5AC;
            temp_f0 = pl->pos[1];
            if (temp_f0 < temp_f2) {
                pl->pos[1] = temp_f2;
                pl->flag604 = 0U;
            } else if (!(temp_f0 < temp_f2)) {
                if ((temp_f0 - temp_f2) < 30.0f) {
                    pl->pos[1] = temp_f2;
                    pl->flag604 = 0U;
                } else if (pl->vital > 0) {
                    if (pl->flag14 != 5) {
                        Pl_act_set(pl, 0, 9, 0);
                    } else {
                        Pl_act_set(pl, 5, 6, 0);
                    }
                }
            }
        }
        pl->work615 = 1;
        World_calc(pl);
        temp_a2 = pl->flag14;
        if ((temp_a2 != 2) && (temp_a2 != 3)) {
            temp_a1 = pl->flag12;
            if (temp_a1 != 0) {
                temp_a0 = pl->kind;
                if (temp_a0 != 1) {
                    if (temp_a0 == 5) {
                        goto block_61;
                    }
                    goto block_71;
                }
block_61:
                if (pl->sw.trg & 0x80) {
                    if (temp_a2 == 0) {
                        temp_a0_2 = pl->flag15;
                        if ((temp_a0_2 != 0x6A) && (temp_a0_2 != 0x69) && (temp_a0_2 != 0x68) && (temp_a0_2 != 0x67) && (temp_a0_2 != 0x1C)) {
                            pl->pch_on = (u8) (pl->pch_on ^ 1);
                        }
                    } else if (((int) (act_ck(pl, 1, 0x3D) << 0x30) >> 0x30) == 0) {
                        pl->pch_on = (u8) (pl->pch_on ^ 1);
                    }
                } else {
                    goto block_71;
                }
            } else {
block_71:
                if (!(temp_a1 & 0xFF)) {
                    if (act_ck(pl, 0, 0x36) == 0) {
                        if (act_ck(pl, 0, 0x48) != 0) {
                            goto block_75;
                        }
                        if ((act_ck(pl, 0, 0x65) != 0) || (act_ck(pl, 0, 0x66) != 0)) {
                            pl->pch_on = 1U;
                        } else {
                            Pl_view_reset(pl);
                        }
                    } else {
block_75:
                        if (pl->sw.trg & 0x80) {
                            pl->pch_on = (u8) (pl->pch_on ^ 1);
                        }
                    }
                } else {
                    temp_a0_3 = pl->kind;
                    if ((temp_a0_3 != 1) && (temp_a0_3 != 5)) {
                        Pl_view_reset(pl, temp_a1, temp_a2);
                    }
                }
            }
        }
        if (pl->pch_on == 0) {
            pl->x763 = 0;
        }
        if (pl->stg == game_w.stage) {
            pl_light_ck(pl);
            PF32((*(void * *)((u8 *)((u8 *)pl + 0x564))), 8) = (f32) pl->pos[0];
            PF32((*(void * *)((u8 *)((u8 *)pl + 0x564))), 0xC) = (f32) pl->pos[1];
            PF32((*(void * *)((u8 *)((u8 *)pl + 0x564))), 0x10) = (f32) pl->pos[2];
            if (Pl_master_ck(pl) == 1) {
                pl->work739 = 0U;
            } else {
                sp30 = pl->pos[0];
                sp34 = 80.0f + pl->pos[1];
                sp38 = pl->pos[2];
                if ((Get_dist_to_view(&sp30) <= 200.0f) || (((int) (act_ck(pl, 4, 4) << 0x30) >> 0x30) != 0)) {
                    pl->work739 = 1U;
                } else {
                    pl->work739 = 0U;
                }
            }
            if ((Pl_master_ck(pl) == 0) && (pl_flag_ck(pl, 0x02001600) != 0)) {
                pl->ang[1] = (s32) (((calc_vec_ang2(((u8 *)pl + 0xAC), ((u8 *)pl + 0x5A0)) & 0xFFFF) + 0x4000) & 0xFFFF);
            }
            if (pl->work739 == 0) {
                add_prim(&ot1, (*(void * *)((u8 *)((u8 *)pl + 0x564))), 0x20, 0);
            } else {
                add_prim(&ot0, (*(void * *)((u8 *)((u8 *)pl + 0x564))), 0x40, 0);
            }
            temp_v1_3 = pl->work90E;
            if (temp_v1_3 != 0) {
                pl->work90E = (u8) (temp_v1_3 - 1);
            }
            if ((pl->work90E == 0) && (((s32) System_timer % 20) == PU8(&game_w, 0xD1))) {
                net_send_pl(pl, 2, 0);
            }
        }
    }
}
