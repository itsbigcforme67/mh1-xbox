/* Player code (f_pl.s, 0x134950..): working file; matched functions are moved
 * to plX.c, what is left here is near-match (not built). */
#include "pl.h"
unsigned short Item_get_ck();   /* its real return type: was implicitly int (tools/check_kr_returns.py) */
#include "plf.h"
#include "game.h"
#include "plst.h"
f32 flSqrt(f32);
#ifdef VERSION_MHG
u8 sub_main_func_004F0020(PLW *, int);
#endif
extern u8 Gun_data[][GUN_ROW];



void timer_calc_sub_pl(PLW *pl) {
    s16 temp_v0_5;
    s16 temp_v0_6;
    s16 temp_v1_4;
    s16 temp_v1_5;
    int temp_s1;
    int var_a1;
    s8 temp_v1_10;
    u16 temp_v0_10;
    u16 temp_v0_11;
    u16 temp_v0_12;
    u16 temp_v0_7;
    u16 temp_v0_8;
    u16 temp_v0_9;
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_a2;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 temp_v0_4;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;
    u8 temp_v1_6;
    u8 temp_v1_7;
    u8 temp_v1_8;
    u8 temp_v1_9;

#ifdef VERSION_MHG
    /* G (sub_main 0x4F1BA0): a new timer at +0x93E (the mega armor pill), a stamina drain state at +0x93D and the life-bar recovery
     * interval from the skills (damage recovery speed) instead of MH1's fixed one */
    if (*(s16 *)((u8 *)pl + 0x93E) != 0) {
        *(s16 *)((u8 *)pl + 0x93E) -= 1;
    }
#endif
    temp_v0 = pl->work936;
    if (temp_v0 != 0) {
        pl->work936 = (u8) (temp_v0 - 1);
    }
    temp_v0_2 = pl->work7ED;
    if (temp_v0_2 != 0) {
        pl->work7ED = (u8) (temp_v0_2 - 1);
    }
    temp_v0_3 = pl->work4DD;
    if (temp_v0_3 != 0) {
        pl->work4DD = (u8) (temp_v0_3 - 1);
    }
    temp_v0_4 = pl->x8C6;
    if (temp_v0_4 != 0) {
        pl->x8C6 = (u8) (temp_v0_4 - 1);
    }
    temp_v0_5 = pl->work760;
    if (temp_v0_5 != 0) {
        pl->work760 = (s16) (temp_v0_5 - 1);
    }
    temp_v0_6 = pl->work8CC;
    if (temp_v0_6 != 0) {
        pl->work8CC = (s16) (temp_v0_6 - 1);
        if ((pl->work8CC == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x15, 0);
        }
    }
    temp_v0_7 = pl->work918;
    if (temp_v0_7 != 0) {
        pl->work918 = (u16) (temp_v0_7 - 1);
        if ((pl->work918 == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x12, 0);
        }
    }
    temp_v0_8 = pl->work91A;
    if (temp_v0_8 != 0) {
        pl->work91A = (u16) (temp_v0_8 - 1);
        if ((pl->work91A == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x13, 0);
        }
    }
    temp_v0_9 = pl->work91C;
    if (temp_v0_9 != 0) {
        pl->work91C = (u16) (temp_v0_9 - 1);
        if ((pl->work91C == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x16, 0);
        }
    }
    temp_v0_10 = pl->work930;
    if (temp_v0_10 != 0) {
        pl->work930 = (u16) (temp_v0_10 - 1);
    }
    temp_v0_11 = pl->work6A6;
    if (temp_v0_11 != 0) {
        pl->work6A6 = (u16) (temp_v0_11 - 1);
        if (pl->work6A6 == 0) {
            pl->work6A5 = 0;
        }
    }
    temp_v0_12 = pl->work6AA;
    if (temp_v0_12 != 0) {
        pl->work6AA = (u16) (temp_v0_12 - 1);
        if (pl->work6AA == 0) {
            pl->work6A9 = 0;
        }
    }
#ifdef VERSION_MHG
    if (pl_flag_ck(pl, 0x200) == 0 && pl->work93D == 0) {
#else
    if (pl_flag_ck(pl, 0x200) == 0) {
#endif
        temp_v1 = pl->flag14;
        if ((temp_v1 != 3) && (temp_v1 != 2)) {
            switch (temp_v1) {                      /* irregular */
            case 0:
                temp_v1_2 = pl->flag15;
                if ((temp_v1_2 != 0x6E) && (temp_v1_2 != 0x6D) && (temp_v1_2 != 0x6C) && (temp_v1_2 != 0x6B) && (temp_v1_2 != 0x44) && (temp_v1_2 != 0x32) && (temp_v1_2 != 0x31) && (temp_v1_2 != 0x2D) && (temp_v1_2 != 0x1C)) {
                    Pl_stamina_calc(pl, 2);
                }
                break;
            case 1:
                temp_v1_3 = pl->flag15;
#ifdef VERSION_MHG
                if (temp_v1_3 == 0x67) {
                    break;
                }
#endif
                if ((temp_v1_3 != 0x4C) && (temp_v1_3 != 0x27) && (temp_v1_3 != 0x26) && (temp_v1_3 != 0x25) && (temp_v1_3 != 0x22) && (temp_v1_3 != 0x1B) && (temp_v1_3 != 0x1A) && (temp_v1_3 != 0x18)) {
                    Pl_stamina_calc(pl, 2);
                }
                break;
            default:
                Pl_stamina_calc(pl, 2);
                break;
            }
        }
    }
#ifdef VERSION_MHG
    if (pl->work93D != 0) {
        Pl_stamina_calc(pl, -1);
        if (pl->stamina <= 0 && pl->flag14 != 1) {
            pl->work93D = 0;
        }
    }
#endif
    Pl_stamina_reduce(pl);
    temp_a0 = pl->flag14;
    if ((temp_a0 != 2) && (temp_a0 != 3)) {
#ifdef VERSION_MHG
        if (((((s16)(Stage_env_ck(pl->stg))) != 1) || (pl->work918 != 0) || (Pl_Skill_ck(pl, 0x2C) == 1)) && (pl->x7BA == 0)) {
            int step = 60;

            if (sub_main_func_004F0020(pl, 7) & 0xFF) {
                if (Pl_Skill_ck(pl, 0x1B) == 1) {
                    step = 20;
                }
                if (Pl_Skill_ck(pl, 0x3C) == 1) {
                    step = 15;
                }
                if (Pl_Skill_ck(pl, 0x3D) == 1) {
                    step = 180;
                }
                if (Pl_Skill_ck(pl, 0x3E) == 1) {
                    step = 240;
                }
            }
            var_a1 = pl->work91C != 0 ? 2 : 1;
            if (PU16(&game_w, 0x1E) % (s16)step == 0 && pl->vital < pl->vital_red) {
                temp_s1 = (s16)(pl->vital_red - pl->vital);
                if (temp_s1 < var_a1) {
                    var_a1 = temp_s1;
                }
                Pl_vital_calc(pl, var_a1);
            }
        }
#else
        if (((((s16)(Stage_env_ck(pl->stg))) != 1) || (Pl_Skill_ck(pl, 0x2D) != 0) || (pl->work918 != 0)) && (pl->x7BA == 0) && !(PU16(&game_w, 0x1E) & 0x3F) && (pl->vital < pl->vital_red)) {
            if ((Pl_Skill_ck(pl, 0x1B) == 1) || (pl->work91C != 0)) {
                var_a1 = (s16)((pl->vital_red - pl->vital));
                if (var_a1 >= 4) {
                    var_a1 = 3;
                }
                Pl_vital_calc(pl, var_a1);
            } else {
                Pl_vital_calc(pl, 1);
            }
        }
#endif
        temp_v1_4 = pl->x7AC;
        if (temp_v1_4 > 0) {
            pl->x7AC = (s16) (temp_v1_4 - 1);
        } else {
            pl->x7AC = 0;
            temp_v1_5 = pl->x7AA;
            if (temp_v1_5 != 0) {
                pl->x7AA = (s16) (temp_v1_5 - 1);
            }
        }
    }
    temp_v1_6 = pl->x763;
    if (temp_v1_6 != 0) {
        pl->x763 = (u8) (temp_v1_6 - 1);
    }
    temp_v1_7 = pl->work8BE;
    if (temp_v1_7 != 0) {
        pl->work8BE = (u8) (temp_v1_7 - 1);
    }
    temp_v1_8 = pl->work886;
    if (temp_v1_8 != 0) {
        pl->work886 = (u8) (temp_v1_8 - 1);
    }
    temp_a2 = pl->work56B;
    temp_a1 = temp_a2 & 0xF;
    if ((temp_a1 != 0) && (pl->flag14 == 5)) {
        pl->work56B = temp_a1;
        pl->work56B = (u8) (pl->work56B - 1);
        pl->work56B = (u8) (pl->work56B | (temp_a2 & 0xF0 & 0xFF));
    } else if ((((s16)(act_ck(pl, 0, 0x47))) == 0) && (((s16)(act_ck(pl, 0, 0x4A))) == 0) && (((s16)(act_ck(pl, 0, 0x51))) == 0) && (((s16)(act_ck(pl, 0, 0x5D))) == 0) && (((s16)(act_ck(pl, 0, 0x60))) == 0)) {
        temp_s1 = Pl_hold_item_ck(pl) & 0xFFFF;
        if (temp_s1 != 0xFFFF) {
            if ((Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
                unmei_se(pl);
                set01_set(1, 5, (s16)(temp_s1));
            }
            Pl_item_stack(pl, temp_s1, -0xA);
        }
    }
    temp_v1_9 = pl->x56F;
    if (temp_v1_9 != 0) {
        pl->x56F = (u8) (temp_v1_9 - 1);
    }
    temp_v1_10 = (*(s8 *)&pl->x881);
    if (temp_v1_10 != 0) {
        pl->x881 = (s8) (temp_v1_10 - 1);
    }
}


int stick_pow_get(PLW *pl, int arg1) { /* arg1 unused, callers pass 0 */
    u8 r = 0;
    u16 p;
    switch (pl->kind) {
    case 1:
    case 5:
        if (pl->flag12 != 0 && pl->pch_on == 1) return 0;
    default:
    case 0:
    case 2:
    case 3:
    case 4:
        pl->x8C8 = 0;
        p = pl->sw.pow[0];
        if (p >= 0x78) {
            r = 3;
        } else if (p >= 0x55) {
            r = 1;
        } else if (p >= 0x28) {
            r = 1;
        }
        if (pl->st == 0 && pl->flag12 == 0 && (pl->sw.now & 0x10) && r && (act_ck(pl, 0, 0x24) == 0 || pl->work760 == 0)) return 5;
        return r;
    }
}


u16 calc_vec_ang(f32, f32, f32, f32);




/* pl_demo000 matches since `q = (u8 *)game_w.x28 + (int)i` (linked as pl_demo.c) */
typedef struct { u8 _pad00[0x14]; s32 x14; } PL_QUEST_W_UNUSED;
EMW *pull_enemy_work(void);
void enemy_mv(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void PlComebackCameraRequest(void);




#include "f_game.h"
extern u8 ot0[], ot1[];
void add_prim(void *, void *, int, int);
/* pl_move_sub (0x14C500, 2144 bytes): near-match, 469/536 insns differ only through delay slots.
 * The original never hoists the `daddu a0,s0,zero` argument copy into a branch delay slot (we do:
 * `bne; daddu a0,s0` vs original `bne; nop; jal; daddu a0,s0`), same effect as timer_calc_sub_pl.
 * Stack layout also differs (original: slide at sp+32, pos[3] at sp+48, gy at sp+60). Tried K&R
 * prototypes, switch form of the work8C2 test: no change. Prototypes below are K&R-style guesses. */
void em_ninshiki_ck();
void pl_timer_calc();
void pl_item_sel();
void pl_shell_sel();
void func_63A260();
u8 pl_status_ck();
void Pl_atck_adj_calc();
void Pl_def_adj_calc();
void pl_dm_value_sub();
void pl_move_sub_sub();
void Pl_pos_adj();
void Pl_status_set();
void hit_stop_calc();
void pl_chr_sub();
void hit_timer_calc();
void HitWallPlayer();
void GetFloorSlide();
void St_unique_adr_set();
/* the PC build maps this name to a 4-argument adaptor (rt_abi.c) that supplies a dummy for the fifth
 * argument; pl_move_sub passes the real one (PLW+0x7E4), so call the definition directly */
#undef GetGroundHitStatusAreaPl
int GetGroundHitStatusAreaPl(void *, f32 *, f32 *, f32 *, f32 *);
void GetPlayerMaterialData();
void pl_light_ck();
f32 Get_dist_to_view();
void Pl_act_set();
void Pl_view_reset();
void pl_move_sub(PLW *pl) {
    int pad[3];
    f32 gy;
    f32 pos[3];
    int slide;
    u8 a, b;

    if (PU8(pl, 0) != 0) {
    em_ninshiki_ck(pl); /* a0 = pl passed through */
    pl_timer_calc(pl);
    pl->work8F2 = 0;
    pl->work90B = 0;
    if (pl->work8C2 == 0) {
        pl_item_sel(pl);
        pl_shell_sel(pl);
    }
    pl->work5A0 = pl->pos[0];
    pl->work5A4 = pl->pos[1];
    pl->work5A8 = pl->pos[2];
    pl->work608 = -1;
    pl->work60A = -1;
#ifndef VERSION_MHG
    pl->work601 = 0;
#endif
    pl->work3F4 = 0;
    pl->work8ED = 0;
    pl->work8F0 = 0;
#ifndef VERSION_MHG
    pl->work917 = 0;
#endif
    if (Pl_master_ck(pl) == 1) {
        if (act_ck(pl, 0, 9) == 0 && act_ck(pl, 0, 0x1B) == 0) pl->work937 = 0;
    } else {
        pl->work937 = 0;
    }
    func_63A260(pl);
#ifdef VERSION_MHG
    pl->work940 = 0; /* G (main 0x562CA0): the byte hit_mark_set_new tests; set from work8F0 below */
#define PMS_ACT_SET Pl_act_set
#else
#define PMS_ACT_SET Pl_act_set2
#endif
    switch (pl_status_ck(pl)) {
    case 1:
        PMS_ACT_SET(pl, 2, 0x15, 0);
        break;
    case 2:
        if (act_ck(pl, 2, 0x16) == 0 && act_ck(pl, 2, 0x18) == 0) PMS_ACT_SET(pl, 2, 0x16, 0);
        break;
    }
#undef PMS_ACT_SET
    Pl_atck_adj_calc(pl);
    Pl_def_adj_calc(pl);
    if (pl->x40A == 0) {
        pl_dm_value_sub(pl);
        pl_move_sub_sub(pl);
        if (pl->work6FF != 0) {
            pl_move_sub_sub(pl);
            pl->work6FF = 0;
        }
        pl_turn_sub(pl);
        pl_horm_sub(pl);
        if (pl->flag14 != 4) Pl_pos_adj(pl);
    }
#ifdef VERSION_MHG
    pl->work940 = pl->work8F0;
    if (pl->flag12 == 0) pl->work93D = 0;
#endif
    Pl_status_set(pl);
    pl->work8C4 = 0;
    pl->work908 = 0;
    hit_stop_calc(pl);
    if (act_ck(pl, 0, 0x27) == 0) pl_chr_sub(pl);
    hit_timer_calc(pl);
    pl->prog->init2(pl);
    if ((act_ck(pl, 0, 0x1B) == 0 || act_ck(pl, 0, 0x1E) == 0) && (Pl_stg_ck(pl) & 0xFF)) HitWallPlayer(pl, 0);
    if (pl->flag14 == 0) {
        if ((u32)(pl->flag15 - 1) < 3U || pl->flag15 == 0x13) {
            if (Pl_stg_ck(pl) & 0xFF) GetFloorSlide(pl, &slide, 1);
        }
    }
    St_unique_adr_set(pl);
    if (GetGroundHitStatusAreaPl(pl, pl->pos, (f32 *)((u8 *)pl + 0x70C), &gy, (f32 *)((u8 *)pl + 0x7E4)) == 1 && act_ck(pl, 4, 0) == 0) pl->x5AC = gy;
    if (Pl_stg_ck(pl) & 0xFF) GetPlayerMaterialData(pl);
    if (pl->st != 2 && pl->flag604 == 0 && act_ck(pl, 0, 0x3A) == 0 && pl->flag14 != 4 && (Pl_stg_ck(pl) & 0xFF)) {
        f32 fy = pl->x5AC;
        f32 py = pl->pos[1];
        if (py < fy) {
            pl->pos[1] = fy;
            pl->flag604 = 0;
        } else if (py - fy < 30.0f) {
            pl->pos[1] = fy;
            pl->flag604 = 0;
        } else if (pl->vital > 0) {
            if (pl->flag14 != 5) Pl_act_set(pl, 0, 9, 0);
            else Pl_act_set(pl, 5, 6, 0);
        }
    }
    pl->work615 = 1;
    World_calc(pl);
    a = pl->flag14;
    if (a != 2 && a != 3) {
        b = pl->flag12;
        if (b != 0 && (pl->kind == 1 || pl->kind == 5)) {
            if (pl->sw.trg & 0x80) {
                if (a == 0) {
                    u8 f = pl->flag15;
                    if (f != 0x6A && f != 0x69 && f != 0x68 && f != 0x67 && f != 0x1C) pl->pch_on ^= 1;
                } else if (
#ifdef VERSION_MHG
                    a != 6 &&
#endif
                    act_ck(pl, 1, 0x3D) == 0) {
                    pl->pch_on ^= 1;
                }
            }
        } else if (b == 0) {
            if (act_ck(pl, 0, 0x36) == 0 && act_ck(pl, 0, 0x48) == 0) {
                if (act_ck(pl, 0, 0x65) != 0 || act_ck(pl, 0, 0x66) != 0) pl->pch_on = 1;
                else Pl_view_reset(pl);
            } else if (pl->sw.trg & 0x80) {
                pl->pch_on ^= 1;
            }
        } else if (pl->kind != 1 && pl->kind != 5) {
            Pl_view_reset(pl);
        }
    }
    if (pl->pch_on == 0) pl->x763 = 0;
    if (pl->stg == game_w.stage) {
        pl_light_ck(pl);
        PF32(pl->work564, 8) = pl->pos[0];
        PF32(pl->work564, 0xC) = pl->pos[1];
        PF32(pl->work564, 0x10) = pl->pos[2];
        if (Pl_master_ck(pl) == 1) {
            pl->work739 = 0;
        } else {
            pos[0] = pl->pos[0];
            pos[1] = pl->pos[1] + 80.0f;
            pos[2] = pl->pos[2];
            if (Get_dist_to_view(pos) <= 200.0f || act_ck(pl, 4, 4) != 0) pl->work739 = 1;
            else pl->work739 = 0;
        }
        if (Pl_master_ck(pl) == 0 && pl_flag_ck(pl, 0x02001600) != 0)
            pl->ang[1] = ((calc_vec_ang2(pl->pos, &pl->work5A0) & 0xFFFF) + 0x4000) & 0xFFFF;
        if (pl->work739 == 0) add_prim(ot1, pl->work564, 0x20, 0);
        else add_prim(ot0, pl->work564, 0x40, 0);
        if (pl->work90E != 0) pl->work90E--;
        if (pl->work90E == 0 && System_timer % 20 == game_w.master) net_send_pl(pl, 2, 0);
    }
    }
}

#include "flow.h"





s32 pl_flag_ck(PLW *pl, int f) {
    if ((f & 0x80000000) == 0) {
        return pl->act_flag & f;
    } else {
        return pl->work394 & (f & 0x7FFFFFFF);
    }
}




extern u8 Equip_Bonus[0x630];
s16 Get_equip_value(u8 kind);
s16 Pl_item_num_ck(PLW *, int);


void pad_timer_calc_sub(PLW *pl, int mask);
f32 GetGroundHit(f32 *);
extern u16 for_pad_timer_tbl[4];





void pad_timer_calc_sub(PLW *pl, int mask) {
    u16 *t = &pl->work5B8;
    u16 *tbl = for_pad_timer_tbl;

    if (pl && pl && pl) { /* no-op test found by the permuter: it makes the table address load as addiu from gp */
    }
    if ((u16)mask & *tbl) {
        *t = 0;
    } else if (*t < 0xFFFF) {
        (*t)++;
    }
}


int rate_g_calc(PLW *pl, int t) {
    f32 a;
    f32 v;
    int m = (s16)t;
    t = (s16)(m / 2);
    v = pl->vel[1];
    a = -1.0f * v;
    if (t <= 1 || v < 0.0f) {
        pl->acc[1] = a;
        return 1;
    }
    a /= (f32)t;
    pl->acc[1] = a;
    return 0;
}

f32 *Stage_data_get(int stg);
void flmatGetTrans(f32 *, u8 *);

#include "hit.h"
u8 Get_hit_id(void);
typedef struct W24 { s32 a, b, c, d, e, f; } W24;


void pl_atck_data_set_shl2(HSHL *sh, u16 *hd, int idx) {
    u8 amask = sh->ailment & 0xF7;
    int aval;
    s32 *src;
    s32 *dst;
    int i;
    if (amask == 0) {
        aval = 0;
    } else {
        aval = sh->ailment_val;
    }
    aval = (u8)aval;
    src = (s32 *)(**(u8 ***)((u8 *)sh + 0x90) + idx * 0x18);
    dst = (s32 *)&sh->hit_time;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
        *dst++ = *src++;
    }
    sh->x08 = hd[6];
    sh->hit_mode = 1;
    sh->x1E = 0;
    sh->hit_chr = 0;
    sh->hit_body = 0;
    if (aval != 0) {
        sh->ailment |= amask;
        sh->ailment_val = aval;
    }
    sh->hit_time >>= 1;
    sh->hit_wait >>= 1;
    if (sh->x75 != 0xFF) {
        sh->x75 >>= 1;
    }
    sh->x76 >>= 1;
    sh->hit_id = Get_hit_id();
}


void atck_data_set_shl2(HSHL *sh, int idx) {
    s32 *src;
    s32 *dst;
    int i;

    src = (s32 *)(**(u8 ***)((u8 *)sh + 0x90) + idx * 0x18);
    dst = (s32 *)&sh->hit_time;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
        *dst++ = *src++;
    }
    sh->x08 = 0xFF;
    sh->hit_mode = 1;
    sh->x1E = 0;
    sh->hit_chr = 0;
    sh->hit_body = 0;
    sh->hit_time >>= 1;
    sh->hit_wait >>= 1;
    if (sh->x75 != 0xFF) {
        sh->x75 >>= 1;
    }
    sh->x76 >>= 1;
    sh->hit_id = Get_hit_id();
}

ST_ITEM *Stage_item_data_get(u8);
ST_UNIQ *Stage_unique_data_get(u8);
u16 *Stage_item_probability_get(int);






long Pl_item_num_ck2(PLW *pl, u16 id) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id == id) {
            if (Item_data[id][3] == 0xFF) {
                if ((pl && pl) && pl) {
                }
                return 0xFF;
            }
            return (s16)(Item_data[id][3] - pl->item[i].num);
        }
    }
    return Item_data[id][3];
}

long Pl_item_num_ck3(PLW *pl, u16 id) {
    s16 i;
    s16 free = 0;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id == id) {
            if (Item_data[id][3] == 0xFF) {
                if ((pl && pl) && pl) { /* permuter no-op: changes only instruction scheduling */
                }
                return 0xFF;
            }
            return (s16)(Item_data[id][3] - pl->item[i].num);
        }
        if (pl->item[i].id == 0) {
            free++;
        }
    }
    if (free == 0) {
        return -1;
    }
    return Item_data[id][3];
}


s16 Get_Use_itemnum(PLW *pl) {
    s16 i;
    s16 n;
    i = 0;
    n = 0;
    for (; i < 20; i++) {
        if (pl->item[i].num > 0 && (s16)pl->item[i].id != 0 && Item_data[(s16)pl->item[i].id][1] == 1) {
            n++;
        }
    }
    return n;
}



int Share_item_stack();

int Pl_item_stack(PLW *pl, int id, int num) {
    u8 *typ = Item_data[(u16)id];
    u8 *mx;
    int ret;
    s16 i;
    s16 j;
    s16 cnt;
    s16 m;
    u16 sv;
    if (typ[0] == 5) {
        return Share_item_stack(pl, id, num);
    }
    mx = &Item_data[(u16)id][3];
    if (*mx == 0xFF) {
        num = (s16)0xFF;
    }
    if (Pl_item_num_ck(pl, id) == 0) {
        ret = 5;
        i = 0;
        do {
            if (pl->item[i].id == 0 && (s16)num > 0) {
                pl->item[i].id = id;
                if ((s16)*mx < (s16)num) {
                    num = (s16)*mx;
                }
                pl->item[i].num = (s16)num;
                if (typ[0] == 4) {
                    sv = pl->work88E;
                    if (i == sv) {
                        pl->work88E = Pl_shell_set(pl, (u16)i, 2);
                        Shell_type_set(pl, 1);
                        pl->work8CE = pl->work8BC;
                        pl->work8D2 = pl->work01D;
                        pl->work8D1 = 0;
                    } else if (pl->kind == 1 || pl->kind == 5) {
                        if (sv == 0xFF) {
                            pl->work88E = Pl_shell_set(pl, 0, 2);
                            Shell_type_set(pl, 0);
                        } else if (Item_data[pl->item[sv].id][0] != 4) {
                            pl->work88E = Pl_shell_set(pl, sv, 0);
                            Shell_type_set(pl, 0);
                        }
                    }
                }
                sv = pl->work888;
                ret = 0;
                if (Item_data[pl->item[sv].id][1] != 1) {
                    pl->work888 = item_sel_sub(pl, sv, 0);
                }
                break;
            }
            i++;
        } while (i < 20);
    } else {
        j = 0;
        do {
            if (pl->item[j].id == (u16)id) {
                cnt = (s16)num;
                m = *mx;
                if (cnt > 0 && pl->item[j].num >= m) {
                    pl->item[j].num = m;
                    ret = 3;
                    goto post;
                }
                if (cnt < 0 && *mx == 0xFF) {
                    ret = 1;
                    break;
                }
                i = j;
                pl->item[i].num += (s16)num;
                if (pl->item[i].num <= 0) {
                    Pl_item_erase(pl, j);
                    ret = 4;
                    pl->item[i].id = 0;
                    pl->item[i].num = 0;
                } else if (m < pl->item[i].num) {
                    pl->item[i].num = m;
                    ret = 2;
                } else {
                    ret = 1;
                }
post:
                if (typ[0] == 4 && i == pl->work88E) {
                    pl->work8BC = pl->item[pl->work88E].num;
                    pl->work8CE = pl->work8BC;
                    if (pl->work8BC < pl->work01C) {
                        pl->work01C = pl->work8BC;
                        pl->work8D1 = pl->work01C;
                    }
                }
                break;
            }
            j++;
        } while (j < 20);
    }
    return ret;
}

int St_pick_ck2(PLW *pl) {
    ST_ITEM *d = Stage_item_data_get(pl->stg);
    f32 dx;
    f32 dz;
    int r;
    u16 rn;
    if (d == 0) {
        return 0xFFFF;
    }
    while (d->pos[0] != -1.0f) {
        if (!(pl->pos[1] < d->pos[1] - 200.0f) && pl->pos[1] < 100.0f + d->pos[1]) {
            dx = pl->pos[0] - d->pos[0];
            dz = pl->pos[2] - d->pos[2];
            if (flSqrt(dx * dx + dz * dz) <= d->r) {
                if (d->num > 0) {
                    r = Item_get_ck(d->id & 0x7FFF) & 0xFFFF;
                    if (r != 0 && r != 0xFFFF && d->num != 0xFF) {
                        rn = ran_suu(1);
                        if ((rn & 7) != 0 || Pl_Skill_ck(pl, 0x2F) == 1) {
                            d->num--;
                        } else {
                            d->num = 0;
                        }
                    }
                } else {
                    return 0xFFFE;
                }
                return r;
            }
        }
        d++;
    }
    return 0;
}

#include "flow.h"
#define ANG2RAD(a) (2.0f * (3.1415927f * (((360.0f * (f32)(a)) / 65536.0f) / 360.0f)))
void *get_joint_mat(PLW *, int, int);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
int Ana_ok_ck();
extern s16 *stg_eft_mdl_no[0x58];

void Pl_horm_adj(PLW *pl, int part) {
    void *mat;
    switch ((s16)part) {
    case 9:
        flmatRotY33(mat = get_joint_mat(pl, 9, pl->work81A), 0.3f * ANG2RAD(pl->work81A));
        flmatRotZ33(mat, ANG2RAD(pl->work750));
        break;
    case 10:
        flmatRotY33(get_joint_mat(pl, 10, pl->work81A), 0.3f * ANG2RAD(pl->work81A));
        break;
    case 19:
        flmatRotY33(get_joint_mat(pl, 19, pl->work81A), 0.3f * ANG2RAD(pl->work81A));
        break;
    case 20:
        flmatRotY33(get_joint_mat(pl, 20, pl->work81A), 0.1f * ANG2RAD(pl->work81A));
        break;
    }
}






void Pl_vital_calc_item(PLW *pl, int dv) {
    int n;
    if (Pl_Skill_ck(pl, 0x1A) == 1 && (n = (s16)dv, n > 0)) {
        n = (s16)(n + n / 4);
    } else {
        n = (s16)dv;
    }
    Pl_vital_calc(pl, n);
}


extern s16 *Pl_slash_tbl[6];

u8 Pl_slash_lv_ck(PLW *pl) {
    s16 *t;
    s16 cur;
    u8 k = pl->kind;
    if (k == 1 || k == 5) {
        return 0;
    }
    cur = pl->work87E;
#ifdef VERSION_MHG
    {   /* G sub_main 0x4CCAB0: six sharpness levels, rows of 6 shorts (5 band ends); MH1's 4-level, 8-byte rows read
         * the wrong row and capped the level at 3 (owner, Xbox: a full blade at quest start looked low, 21 Oct 2026) */
        int i;
        t = (s16 *)((u8 *)Pl_slash_tbl[k] + Ken_data[pl->wpn_kind][2] * 12);
        for (i = 0; i < 5; i++)
            if (t[i] >= cur)
                return (u8)i;
        return 5;
    }
#endif
    t = (s16 *)((u8 *)Pl_slash_tbl[k] + Ken_data[pl->wpn_kind][2] * 8);
    if (t[0] >= cur) {
        return 0;
    }
    if (t[1] >= cur) {
        return 1;
    }
    if (t[2] >= cur) {
        return 2;
    }
    return 3;
}


#ifdef VERSION_MHG
/* G sub_main 0x4CD030 (unnamed in G): the shot types a gunner can load: the gun's mask (Gun_data
 * +0x14) plus the ones skills 0x8B-0x98 add (three levels for each of four groups). */
u32 sub_main_func_004CD030(PLW *pl) {
    u32 m;

    if (pl->kind != 1 && pl->kind != 5) {
        return 0;
    }
    m = *(u32 *)&Gun_data[pl->wpn_kind][0x14];
    if (Pl_Skill_ck(pl, 0x8B) == 1) {
        m |= 0x1;
    } else if (Pl_Skill_ck(pl, 0x8C) == 1) {
        m |= 0x7;
    }
    if (Pl_Skill_ck(pl, 0x8D) == 1) {
        m |= 0x8;
    } else if (Pl_Skill_ck(pl, 0x8E) == 1) {
        m |= 0x18;
    } else if (Pl_Skill_ck(pl, 0x8F) == 1) {
        m |= 0x38;
    }
    if (Pl_Skill_ck(pl, 0x90) == 1) {
        m |= 0x40;
    } else if (Pl_Skill_ck(pl, 0x91) == 1) {
        m |= 0xC0;
    } else if (Pl_Skill_ck(pl, 0x92) == 1) {
        m |= 0x1C0;
    }
    if (Pl_Skill_ck(pl, 0x93) == 1) {
        m |= 0x200;
    } else if (Pl_Skill_ck(pl, 0x94) == 1) {
        m |= 0x600;
    } else if (Pl_Skill_ck(pl, 0x95) == 1) {
        m |= 0xE00;
    }
    if (Pl_Skill_ck(pl, 0x96) == 1) {
        m |= 0x1000;
    } else if (Pl_Skill_ck(pl, 0x97) == 1) {
        m |= 0x3000;
    } else if (Pl_Skill_ck(pl, 0x98) == 1) {
        m |= 0x7000;
    }
    return m;
}
#endif

u16 Pl_shell_set(PLW *pl, int slot, int dir) {
    s16 n;
    u16 s;
    u8 d;
#ifdef VERSION_MHG
    u32 mask;
#endif
    if (pl->kind != 1 && pl->kind != 5) {
        return 0xFF;
    }
    if (pl->work35F != 7) {
        return 0xFF;
    }
    s = slot;
    if (s >= 20) {
        s = 0;
    }
    d = dir;
    switch (d) {
    case 0:
        s = (s + 1) % 20;
        break;
    case 1:
        if (s == 0) {
            s = 19;
        } else {
            s = s - 1;
        }
        break;
    case 3:
    case 2:
        break;
    }
#ifdef VERSION_MHG
    mask = sub_main_func_004CD030(pl);      /* G sub_main 0x4CCD30 */
#endif
    for (n = 0; n < 20; n++) {
        if (pl->item[s].id != 0 && pl->item[s].num > 0 && Item_data[pl->item[s].id][1] == 2
#ifdef VERSION_MHG
            && (mask & (1 << *(s16 *)&Item_data[pl->item[s].id][8]))) {
#else
            && (*(s32 *)&Gun_data[pl->wpn_kind][0x10] & (1 << *(s16 *)&Item_data[pl->item[s].id][8]))) {
#endif
            return s;
        }
        switch (d) {
        case 2:
        case 0:
            s = (s + 1) % 20;
            break;
        case 3:
        case 1:
            if (s == 0) {
                s = 19;
            } else {
                s = s - 1;
            }
            break;
        }
    }
    return 0xFF;
}

extern u8 chat_act_tbl_002F1860[0xD];
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

#include "flow.h"

s32 Pl_hold_item_ck(PLW *pl) {
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

typedef struct TUTO_REC {
    u16 id;     /* 0x00 */
    u16 _02;
    f32 x;      /* 0x04 */
    f32 y;      /* 0x08 */
    f32 z;      /* 0x0C */
    f32 r;      /* 0x10 */
} TUTO_REC;
s16 Pl_item_num_ck(PLW *, int);
void adx_se_set(PLW *, int);
void init_set_work();
void init_eft_work();
void init_shell_work();
void init_item_work();
void clr_set_work();
void clr_eft_work();
void clr_shell_work();
void clr_item_work();
void clr_used_heap(int, int);

s32 Pl_scope_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x40) != 0;
}

s32 Pl_silencer_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x10) != 0;
}

s32 Pl_barrel_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x20) != 0;
}





s32 Sansai_talk_ck(PLW *pl) {
    EMW *e = em_work;
    s16 i;
    for (i = 0; i < 20; i++, e++) {
        if (e->kind == 0xA && flvecCalcDistance(pl->pos, e->pos) <= 300.0f) {
            return 1;
        }
    }
    return 0;
}

#include "flow.h"
void Pl_vital_calc_item(PLW *, int);
void Pl_max_vital_calc(PLW *, int);
void func_639DF0(PLW *, int);
void set01_set(int, int, int);

#include "flow.h"
void flmatGetTrans(f32 *, u8 *);
void RotMatVec(f32 *, f32 *, int);
f32 flSqrt(f32);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);


void pl_light_ck(PLW *pl) {
    int i;
    EMW *e;
    f32 d;
    f32 dx;
    f32 dz;
    f32 lim;
    f32 hlim;
    if (pl->flag604 != 0 || !(pl->pos[1] - pl->x5AC <= 100.0f)) {
        pl->work613 = 0;
        return;
    }
    e = em_work;
    for (i = 0; i < 20; i++, e++) {
        if (e->be_flag != 0 && e->x01 != 0 && PU8(e, 0x612) >= 2) {
            dx = pl->pos[0] - e->pos[0];
            dz = pl->pos[2] - e->pos[2];
            d = flSqrt(dx * dx + dz * dz);
            if (*(u8 *)((int)e + 0x612) == 2) {
                lim = 300.0f;
                hlim = 300.0f;
            } else {
                lim = 500.0f;
                hlim = 300.0f;
            }
            if (d <= lim && pl->pos[1] - e->pos[2] < hlim) {
                pl->work613 = 1;
                return;
            }
        }
    }
    pl->work613 = 0;
}

#include "flow.h"
void pl_body_make(PLW *pl, f32 *out, f32 radius);
void hit_cap_pk(void *, void *);
unsigned char hit_cap_cap3_m(void *, void *, f32 *);


extern f32 *D_63FC50[];
extern f32 *D_6103A0[];
void flvecNormalize(f32 *);
unsigned char hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb);

/* Pushes two monsters apart (sphere lists from the per-kind push tables). The inner list pointer is
 * never rewound for the 2nd and later spheres of the first monster: faithful to the original. */
void body_hit_sub_em(PLW *pl, PLW *o) {
    f32 pa[3];
    f32 pb[3];
    f32 len[2];
    f32 v[2][3];
    f32 *s4;
    f32 *s3;
    s16 n = 0;
    s16 k;
    int cnt;
    f32 ra;
    f32 *pv;
    f32 *pl_len;
    if (game_w.x1DC == 0) {
#if defined(VERSION_MHG) && !defined(__MWERKS__)
        /* G (sub_main 0x4C9B00): its own monster-to-monster sphere table (game.bin 0x673260), not the wall table MH1
         * shares here (the PC pushed small monsters off the Rathian's 500-unit wall sphere; the lockstep referee) */
        extern f32 *em_body_push_tbl[];
        s3 = em_body_push_tbl[o->kind];
        s4 = em_body_push_tbl[pl->kind];
#else
        s3 = D_63FC50[o->kind];
        s4 = D_63FC50[pl->kind];
#endif
    } else {
        s3 = D_6103A0[o->kind];
        s4 = D_6103A0[pl->kind];
    }
    pv = v[0];
    pl_len = len;
    while (s4[3] != -1.0f) {
        pa[0] = pl->pos[0] + s4[0] * pl->scl[0];
        pa[1] = pl->pos[1] + s4[1] * pl->scl[1];
        pa[2] = pl->pos[2] + s4[2] * pl->scl[2];
        ra = s4[3] * pl->scl[0];
        if (s3[3] != -1.0f) {
            do {
                pb[0] = o->pos[0] + s3[0] * pl->scl[0];
                pb[1] = o->pos[1] + s3[1] * pl->scl[1];
                pb[2] = o->pos[2] + s3[2] * pl->scl[2];
                if (hit_sphr_sphr3(pa, pb, pv, ra, s3[3] * o->scl[0]) != 0) {
                    *pl_len = flvecCalcLength(pv);
                    pv += 3;
                    pl_len++;
                    n++;
                }
                if (n >= 2) {
                    goto done;
                }
                s3 += 4;
            } while (s3[3] != -1.0f);
        }
        s4 += 4;
    }
done:
    cnt = n;
    if (cnt != 0) {
        for (k = 1; k < cnt; k++) {
            v[0][0] += v[k][0];
            v[0][1] += v[k][1];
            v[0][2] += v[k][2];
            if (len[0] < len[k]) {
                len[0] = len[k];
            }
        }
        if (pl->st != 2) {
            v[0][1] = 0;
        }
        flvecNormalize(v[0]);
        v[0][0] *= len[0];
        v[0][1] *= len[0];
        v[0][2] *= len[0];
        if (pl->work612 == o->work612) {
            pl->pos[0] += 0.5f * v[0][0];
            if (pl->st == 2) {
                pl->pos[1] += 0.5f * v[0][1];
            }
            pl->pos[2] += 0.5f * v[0][2];
            o->pos[0] += -0.5f * v[0][0];
            if (o->st == 2) {
                o->pos[1] += -0.5f * v[0][1];
            }
            o->pos[2] += -0.5f * v[0][2];
            pl->work7EC = 1;
            o->work7EC = 1;
        } else if (pl->work612 < o->work612) {
            pl->pos[0] += v[0][0];
            if (pl->st == 2) {
                pl->pos[1] += v[0][1];
            }
            pl->pos[2] += v[0][2];
            pl->work7EC = 1;
        } else {
            o->pos[0] -= v[0][0];
            if (o->st == 2) {
                o->pos[1] -= v[0][1];
            }
            o->pos[2] -= v[0][2];
            o->work7EC = 1;
        }
    }
}

extern s16 *D_63FA10[];
extern s16 *D_610370[];
void body_ptr_ck2(PLW *, s16 **);
int hit_data_expand(PLW *, s16 *, f32 *, f32 *);
int hit_data_expand2(f32 *, s16 *, f32 *, f32 *);
unsigned char hit_cap_sphr_m(void *, f32 *, f32 *, f32);

/* Pushes the player against one monster's body list (capsule/sphere hit data, 0x28 bytes per entry). */
void body_hit_sub_new(PLW *pl, PLW *o) {
    s16 *rec;
    f32 len[2];
    u8 pkp[0x40];
    u8 pkb[0x40];
    f32 body[8];
    f32 cap[8];
    f32 sph[4];
    f32 v[2][3];
    f32 *pv;
    f32 *plen;
    f32 *prad;
    s16 n = 0;
    int cnt;
    s16 k;
    int r;
    pl_body_make(pl, body, 40.0f);
    hit_cap_pk(body, pkp);
    if (game_w.x1DC == 0) {
        rec = D_63FA10[o->kind];
    } else {
        rec = D_610370[o->kind];
    }
    pv = v[0];
    plen = len;
    prad = &sph[3];
    goto test;
    for (;;) {
        switch (*rec) {
        case 126:
        case 127:
            r = (s16)hit_data_expand2(o->pos, rec, cap, sph);
            goto got;
        case 125:
            body_ptr_ck2(o, &rec);
            goto test;
        default:
            r = (s16)hit_data_expand(o, rec, cap, sph);
got:
            if (r == 0) {
                r = (u8)hit_cap_sphr_m(pkp, sph, pv, *prad);
            } else {
                hit_cap_pk(cap, pkb);
                r = (u8)hit_cap_cap3_m(pkb, pkp, pv);
            }
            if ((u8)r != 0) {
                *plen = flvecCalcLength(pv);
                if (pl->st != 2) {
                    pv[1] = 0;
                }
                n++;
                pv += 3;
                plen++;
            }
            if (n >= 2) {
                goto done;
            }
            rec += 0x14;
test:
            if (*rec == -1) {
                goto done;
            }
#ifdef VERSION_MHG
            if (((u8 *)rec)[8] & 4) { /* G: body entries flagged 4 are switched off (broken parts) */
                rec += 0x14;
                goto test;
            }
#endif
            break;
        }
    }
done:
    cnt = n;
    if (cnt != 0) {
        for (k = 1; k < cnt; k++) {
            v[0][0] += v[k][0];
            v[0][1] += v[k][1];
            v[0][2] += v[k][2];
            if (len[0] < len[k]) {
                len[0] = len[k];
            }
        }
        flvecNormalize(v[0]);
        v[0][0] *= len[0];
        v[0][1] *= len[0];
        v[0][2] *= len[0];
        if (o->work612 <= 0) {
            pl->pos[0] += 0.5f * v[0][0];
            pl->pos[1] += 0.5f * v[0][1];
            pl->pos[2] += 0.5f * v[0][2];
            o->pos[0] += -0.5f * v[0][0];
            o->pos[1] += -0.5f * v[0][1];
            o->pos[2] += -0.5f * v[0][2];
            pl->work7EC = 1;
            o->work7EC = 1;
        } else {
            pl->pos[0] += v[0][0];
            pl->pos[1] += v[0][1];
            pl->pos[2] += v[0][2];
            pl->work7EC = 1;
        }
#ifdef VERSION_MHG
        if (pl->pos[1] < pl->x5AC) { /* G: the push never sinks the hunter below the ground line */
            pl->pos[1] = pl->x5AC;
        }
#endif
    }
}

s32 Pl_stg_ck_tw(PLW *, PLW *);

/* Per-frame body pushing: players vs players (when softdip 0xA9), players vs monsters, monsters vs monsters. */
void body_hit(void) {
    f32 d[3];
    int i;
    int j;
    int k;
    PLW *pl;
    PLW *p2;
    PLW *e;
    PLW *f;
    int busy;
    for (i = 0; i < 4; i++) {
        player_work[i].work7EC = 0;
    }
    for (i = 0; i < 20; i++) {
        ((PLW *)&em_work[i])->work7EC = 0;
    }
    for (i = 0; i < 4; i++) {
        pl = &player_work[i];
        if (pl->be_flag != 0 && pl->x01 != 0 && Pl_master_ck(pl) != 0 && pl->work40E == 0 && (u8)Pl_stg_ck(pl)) {
            if (softdip_ck(0xA9) != 0) {
                for (j = 0; j < 4; j++) {
                    p2 = &player_work[j];
                    if (p2->be_flag != 0 && p2->x01 != 0 && p2->work40E == 0 && (u8)Pl_stg_ck_tw(pl, p2)) {
                        body_hit_sub_pl(pl, p2);
                    }
                }
            }
            for (j = 0; j < 20; j++) {
                e = (PLW *)&em_work[j];
                if (e->be_flag != 0 && e->x01 != 0 && e->work40E == 0 && (u8)Pl_stg_ck_tw(pl, e)) {
                    body_hit_sub_new(pl, e);
                    d[0] = pl->pos[0] - e->pos[0];
                    d[1] = pl->pos[1] - e->pos[1];
                    d[2] = pl->pos[2] - e->pos[2];
                    PF32(pl, 0x3AC) = flvecCalcLength(d);
                }
            }
        }
    }
    for (i = 0; i < 19; i++) {
        e = (PLW *)&em_work[i];
        if (e->be_flag != 0 && e->x01 != 0 && e->work40E == 0 && (u8)Pl_stg_ck(e)) {
            busy = PU16(e, 0x7EA) != 0;
            for (k = i + 1; k < 20; k++) {
                f = (PLW *)&em_work[k];
                if (f->be_flag != 0 && f->x01 != 0) {
                    if (!(e->kind != 0x1D && f->kind != 0x1D) || (PU8(e, 0x8C3) == 0 && PU8(f, 0x8C3) == 0)) {
                        if (f->work40E == 0 && (u8)Pl_stg_ck_tw(e, f)) {
                            if ((busy == 0 && PU16(f, 0x7EA) == 0) || e->kind == 0x1D || f->kind == 0x1D) {
                                body_hit_sub_em(e, f);
                            }
                        }
                    }
                }
            }
        }
    }
}


#include "flow.h"
extern u16 Psw[];
void Item_box_get_efct();
void Item_box_get_item(u16, u8);
void net_send_host(int, u8);
s16 Pl_item_num_ck(PLW *, int);

/* Item box (village storage): game_w+0x128 holds {u16 item, s16 count} per slot (32 slots), game_w+0x1A8 a taken-bitmask. */
void box_get(PLW *pl) {
    pl->work8F3 = 0x1E;
    Item_box_get_efct(pl);
    if (Online_ck() == 1) {
        pl->work932 = 0x384;
        pl->work91F = 1;
        net_send_host(1, game_w.master);
        return;
    }
    pl->work932 = 0;
    pl->work91F = 0;
    Pl_item_stack(pl, PU16(&game_w, 0x128 + pl->work8C3 * 4), PS16(&game_w, 0x12A + pl->work8C3 * 4));
    PU32(&game_w, 0x1A8 + (pl->work8C3 >> 5) * 4) |= 1 << (pl->work8C3 % 32);
    Item_box_get_item(PU16(&game_w, 0x128 + pl->work8C3 * 4), pl->work8C3);
}

void Pl_box_select(PLW *pl) {
    s16 trg;
    s16 pad;
    int a;
    u8 idx;
    int bit;
    if (Pl_master_ck(pl) != 0) {
        trg = Psw[2];
        pad = Psw[2] | Psw[12];
        if (pl->work8F3 != 0) {
            pl->work8F3--;
        }
        if (pl->work932 != 0) {
            pl->work932--;
            if (pl->work932 == 0) {
                pl->work91F = 0;
            }
        }
        if (pl->work91F != 0 || pl->work8F3 != 0) {
            return;
        }
        a = pad;
        if (a & 0x40) {
            se_req(7, 0x14, 0);
            pl->work8C2 = 0;
            return;
        }
        if (a & 0x800) {
            se_req(7, 0x16, 0);
            if (pl->work8C3 & 7) {
                pl->work8C3 = pl->work8C3 - 1;
            } else {
                pl->work8C3 = pl->work8C3 + 7;
            }
        }
        if (a & 0x400) {
            se_req(7, 0x16, 0);
            if ((pl->work8C3 & 7) != 7) {
                pl->work8C3 = pl->work8C3 + 1;
            } else {
                pl->work8C3 = pl->work8C3 - 7;
            }
        }
        if (a & 0x2000) {
            se_req(7, 0x16, 0);
            if (pl->work8C3 < 8) {
                pl->work8C3 = pl->work8C3 + 0x18;
            } else {
                pl->work8C3 = pl->work8C3 - 8;
            }
        }
        if (a & 0x1000) {
            se_req(7, 0x16, 0);
            if (pl->work8C3 >= 0x18) {
                pl->work8C3 = pl->work8C3 - 0x18;
            } else {
                pl->work8C3 = pl->work8C3 + 8;
            }
        }
        idx = pl->work8C3;
        bit = 1 << (idx % 32);
        if (!(PU32(&game_w, 0x1A8 + (idx >> 5) * 4) & bit) && (trg & 0x20) && PU16(&game_w, 0x128 + idx * 4) != 0 && pl->work8F3 == 0) {
            if (Pl_item_num_ck(pl, PU16(&game_w, 0x128 + idx * 4)) == 0) {
                if (Pl_item_search_space(pl) != 0) {
                    se_req(7, 0x19, 0);
                    box_get(pl);
                }
            } else if (Pl_item_num_ck2(pl, PU16(&game_w, 0x128 + pl->work8C3 * 4)) >= PS16(&game_w, 0x12A + pl->work8C3 * 4)) {
                se_req(7, 0x19, 0);
                box_get(pl);
            }
        }
    }
}
