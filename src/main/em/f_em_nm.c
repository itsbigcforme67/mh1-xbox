/* f_em_nm - main 0x10B7F0-0x10CB30 and 0x10EBF0-0x10EC18 (f_em, the monster
 * loop): em_init_sub, em_status_init, em_init, em_move, em_die, em_erase,
 * enemy_mv, em_effect_move, common_local_init, dummy_em_prog.
 * Written from an m2c draft and the asm for the PC port (agent A); not
 * compared with check.py yet, not in config/c_files.txt.
 *
 * enemy_mv is the per-monster task: em_work[n].x04 is its step (0 init,
 * 1 alive: em_move, 2 dying: em_die, 3 erase). em_move runs the shared
 * monster upkeep (sight, smell, hate, anger, status) from game.bin's em_core
 * / em_master / em_taisei, then the monster's own main (em01_main for kinds
 * 0, 1 and 11, ...), then the motion step (frame_move) and the stage
 * collision. EMW+0x3CC points at the monster's program table
 * (em_prog_tbl[kind]): [0] local init, [3] per-frame sound/effect script.
 * Field meanings are guesses; raw offsets are used for fields that em.h
 * does not name yet (several agents are carving em.h). */
#include "em.h"
#include "game.h"

#define EB(o) (*(u8 *)((u8 *)em + (o)))
#define ESB(o) (*(s8 *)((u8 *)em + (o)))
#define EH(o) (*(s16 *)((u8 *)em + (o)))
#define EW(o) (*(s32 *)((u8 *)em + (o)))
#define EF(o) (*(f32 *)((u8 *)em + (o)))
#define EP(o) (*(void **)((u8 *)em + (o)))

typedef void (*EM_PROG)(EMW *);

extern EM_PROG *em_prog_tbl[];
extern void enemy_trans();
extern u8 enemy_size[];
extern f32 enemy_scale[];
extern struct EM_AREA *em_pos_tbl[];
extern s32 *em_hungry_tbl[];
extern s32 *em_thirst_tbl[];
extern s32 *em_suimin_tbl[];
extern f32 *em_ikari_data_tbl[];
extern s16 em_atk_mode_timer_tbl[];
extern u8 ot0[];
extern u8 ot1[];
extern u8 player_work[];

typedef struct QUEST_W_EM {
    u8 _pad00[8];
    s16 no;             /* 0x08 quest number (0: free hunt) */
} QUEST_W_EM;
extern QUEST_W_EM quest_w;

s16 get_prim(void);
void *get_prim_ptr(s16);
void add_prim(void *ot, void *prim, int pri, int sub);
void em_work_set(EMW *);
void em_cmd_init(EMW *);
void Em_hagi_point_clr(EMW *);
void Em_Taisei_Set(EMW *);
f32 em_def_attack_set(EMW *);
f32 em_def_defence_set(EMW *);
void em_wall_bit_set(EMW *);
void em_search_data_set(EMW *, u8);
void em_range_set(EMW *, s8);
void Ikari_Data_Set(EMW *);
void em_tsuushin_set(EMW *);
int Em_max_parts_get(int);
void em_dur_init(EMW *);
void World_calc(void *);
void eft01_set(void *, int);
void Quest_enemy_die(EMW *);
void push_em_work(EMW *);

void Em_Master_Change(void);
int Online_ck(void);
void net_receive_em_act(EMW *);
void pl_timer_calc(void *);
void hit_stop_calc(void *);
void GetGroundHitStatusAreaEm(EMW *, f32 *, void *, f32 *);
void kehai_set(EMW *);
void Stage_Hate_Add(EMW *);
void kehai_ck(EMW *);
s8 em_eye_search_set(EMW *);
int em_cancel_act_ck(EMW *, u8);
int smell_ck(EMW *, int);
void ikari_flag_set(EMW *);
void em_escape_action_ck(EMW *);
u8 pl_ninshiki_ck(EMW *);
int em_pl_pos_set(EMW *, u8, f32 *);
void SetVector(f32 *, f32, f32, f32);
int Em_Unko_Smoke_Ck(EMW *);
void em_eye_dmg_reset_act_set(EMW *);
void Em_Taisei_Ck(EMW *);
void cpRotMatrixYXZ2(s32 *, f32 *);
void em_neck_move(EMW *);
void Em_Damage_Hate_Set(EMW *);
void Em_Hate_Ck(EMW *);
void pl_flag_set(void *, int);
void pl_flag_clr(void *, int);
void frame_move(void *);
void HitWallPlayer(void *, int);
void GetEmMaterialData(EMW *);
f32 flArcTan2(f32, f32);
int flConvertRtoS(f32);

void em01_init(EMW *); void em01_main(EMW *);
void em02_init(EMW *); void em02_main(EMW *);
void em03_init(EMW *); void em03_main(EMW *);
void em04_init(EMW *); void em04_main(EMW *);
void em07_init(EMW *); void em07_main(EMW *);
void em08_init(EMW *); void em08_main(EMW *);
void em09_init(EMW *); void em09_main(EMW *);
void em10_init(EMW *); void em10_main(EMW *);
void em12_init(EMW *); void em12_main(EMW *);
void em14_init(EMW *); void em14_main(EMW *);
void em15_init(EMW *); void em15_main(EMW *);
void em16_init(EMW *); void em16_main(EMW *);
void em17_init(EMW *); void em17_main(EMW *);
void em18_init(EMW *); void em18_main(EMW *);
void em19_init(EMW *); void em19_main(EMW *);
void em20_init(EMW *); void em20_main(EMW *);
void em21_init(EMW *); void em21_main(EMW *);
void em27_init(EMW *); void em27_main(EMW *);
void em29_init(EMW *); void em29_main(EMW *);
void em33_init(EMW *); void em33_main(EMW *);

void em_die(EMW *em);
void em_erase(EMW *em);
void em_effect_move(EMW *em);

/* 0x10B7F0: program table, the local init, the draw prim, the monster's init. */
void em_init_sub(EMW *em) {
    EP(0x3CC) = em_prog_tbl[em->kind];
    (*(EM_PROG *)EP(0x3CC))(em);
    if (em->x01 != 0) {
        EH(0x568) = get_prim();
        if (EH(0x568) != -1) {
            EP(0x564) = get_prim_ptr(EH(0x568));
            *(EMW **)((u8 *)EP(0x564) + 0x18) = em;
            *(void **)((u8 *)EP(0x564) + 0x14) = (void *)enemy_trans;
        } else {
            EP(0x564) = 0;
        }
    }
    switch (em->kind) {
    case 1: em01_init(em); break;
    case 2: em02_init(em); break;
    case 3: em03_init(em); break;
    case 4: em04_init(em); break;
    case 5: em04_init(em); break;
    case 6: em20_init(em); break;
    case 7: em07_init(em); break;
    case 8: em08_init(em); break;
    case 9: em09_init(em); break;
    case 10: em10_init(em); break;
    case 11: em01_init(em); break;
    case 12: em12_init(em); break;
    case 13: em16_init(em); break;
    case 14: em14_init(em); break;
    case 15: em15_init(em); break;
    case 16: em16_init(em); break;
    case 17: em17_init(em); break;
    case 18: em18_init(em); break;
    case 19: em19_init(em); break;
    case 20: em20_init(em); break;
    case 21: em21_init(em); break;
    case 22: em17_init(em); break;
    case 23: em09_init(em); break;
    case 24: em19_init(em); break;
    case 25: em12_init(em); break;
    case 26: em14_init(em); break;
    case 27: em27_init(em); break;
    case 28: em27_init(em); break;
    case 29: em29_init(em); break;
    case 30: em16_init(em); break;
    case 31: em27_init(em); break;
    case 32: em04_init(em); break;
    case 33: em33_init(em); break;
    case 34: em08_init(em); break;
    }
}

/* 0x10BAD0: clears the status/hate/anger work and loads the per-kind
 * values (size, scale, hunger/thirst/sleep limits, area data). */
void em_status_init(EMW *em) {
    s8 i;
    u8 *p;
    s32 *hu, *th, *su;

    ESB(0x9ED) = 0;
    for (i = 0, p = (u8 *)em; i < 2; i++, p += 0x34) {
        *(s16 *)(p + 0x960) = 0;
        *(s32 *)(p + 0x964) = 0;
        *(s32 *)(p + 0x968) = 0;
        *(s32 *)(p + 0x96C) = 0;
        *(s16 *)(p + 0x970) = 0;
        *(s16 *)(p + 0x972) = 0;
        *(s16 *)(p + 0x974) = 0;
        p[0x976] = 0;
        p[0x977] = 0;
        p[0x979] = 0;
        p[0x97A] = 0;
        p[0x97B] = 0;
        p[0x97E] = 0;
        *(s16 *)(p + 0x97C) = 0;
        *(s16 *)(p + 0x980) = 0;
    }
    em_cmd_init(em);
    EW(0x878) = 0;
    EH(0x70E) = 1;
    EB(0x412) = 0;
    em->x01 = 1;
    EF(0x1A0) = 2.0f;
    EF(0x1F0) = 2.0f;
    EF(0x240) = 2.0f;
    EF(0x290) = 2.0f;
    em->scale[0] = enemy_scale[em->kind];
    em->scale[1] = enemy_scale[em->kind];
    em->scale[2] = enemy_scale[em->kind];
    if (quest_w.no == 0) {
        em->ang[0] = 0;
        em->ang[1] = 0;
        em->ang[2] = 0;
    }
    EF(0x798) = 1.0f;
    EB(0x56A) = 0;
    EW(0x39C) = 0;
    Em_hagi_point_clr(em);
    EB(0x612) = enemy_size[em->kind];
    EW(0x6E8) = 0;
    EW(0x6EC) = 0;
    EW(0x6F0) = 0;
    EW(0x6F4) = 0;
    EF(0x930) = 1.0f;
    EB(0x87F) = 0;
    EB(0x88E) = 0;
    EB(0x884) = 0;
    EB(0x885) = 0;
    EH(0x886) = 0;
    EB(0x888) = 0;
    EB(0x889) = 0;
    EB(0x88A) = 0;
    EB(0x88C) = 0;
    EB(0x88F) = 0;
    EH(0x890) = 0;
    EH(0x892) = 0;
    EH(0x894) = 0;
    EH(0x896) = 0;
    hu = em_hungry_tbl[em->kind];
    su = em_suimin_tbl[em->kind];
    th = em_thirst_tbl[em->kind];
    EW(0x8A4) = th[0];
    EW(0x8A8) = hu[0];
    EW(0x8AC) = su[0];
    if (EW(0x898) <= 0) {
        EW(0x898) = th[1];
    }
    if (EW(0x89C) <= 0) {
        EW(0x89C) = hu[1];
    }
    if (EW(0x8A0) <= 0) {
        EW(0x8A0) = su[1];
    }
    EB(0x8B6) = 0;
    EH(0x8B4) = 0;
    EH(0x8B2) = 0;
    EB(0x8B7) = 0;
    EB(0x8B8) = 0;
    EB(0x8B9) = 0;
    EB(0x8BB) = 0;
    EB(0x8BC) = 0;
    EB(0x8BD) = 0;
    EB(0x8BE) = 0;
    EB(0x8C0) = 0;
    EB(0x8C1) = 0;
    EB(0x8BF) = 0;
    EB(0x8C2) = 0;
    EW(0x8F4) = 0;
    EW(0x8F8) = 0;
    EW(0x8FC) = 0;
    EW(0x900) = 0;
    EB(0x914) = 0;
    EB(0x915) = 0;
    EB(0x916) = 1;
    EB(0x917) = 0;
    EW(0x918) = 0;
    EW(0x91C) = 0;
    EW(0x920) = 0;
    EW(0x924) = 0;
    EP(0x940) = em_pos_tbl[em->kind];
    EH(0x94E) = 0;
    EB(0x957) = 0;
    EB(0x958) = 0;
    EB(0x959) = 0;
    EB(0x95A) = 0;
    EB(0x95C) = 0;
    EB(0x9E1) = 0;
    EB(0x9E3) = 0;
    EB(0x9E4) = 0;
    EB(0x9E5) = 0;
    EB(0x9E7) = 0;
    EB(0x9E6) = 0;
    EB(0x9E8) = 0;
    EB(0x9EA) = 0;
    EB(0x9F1) = 0;
    Em_Taisei_Set(em);
    EH(0x7B2) = 0;
    EH(0x7B6) = 0;
    EH(0x7CC) = 0;
    EH(0x7D0) = 0;
    EH(0x7C4) = 0;
    EH(0x7C8) = 0;
    EH(0x7BA) = 0;
    EH(0x7BE) = 0;
    EH(0x7C0) = 0;
    EB(0x7D2) = 0;
    EB(0x7D3) = 0;
    EB(0x7D6) = 0;
    EF(0x7D8) = em_def_attack_set(em);
    EF(0x7DC) = em_def_defence_set(em);
    EB(0x4D4) = 1;
    EW(0x9D4) = 0;
    EB(0x8BA) = 0;
    EB(0x877) = 0;
}

/* 0x10BE30: step 0 of enemy_mv. */
void em_init(EMW *em) {
    int i;

    em_work_set(em);
    em->x04++;
    em->x10 = 1;
    EB(0x1E) = 0;
    EB(0x88E) = *((u8 *)&game_w + 0xD1);
    em_status_init(em);
    EH(0x2DC) = 0;
    EH(0x2DE) = 0;
    EH(0x2E0) = 0;
    EH(0x2E2) = 0;
    em_wall_bit_set(em);
    em_search_data_set(em, 0);
    em_range_set(em, 0);
    Em_Taisei_Set(em);
    Ikari_Data_Set(em);
    em_tsuushin_set(em);
    for (i = 0; i < 0x20; i++) {
        EB(0x4E6 + i) = 1;
    }
    EH(0x300) = Em_max_parts_get(em->kind) & 0xFF;
    em_init_sub(em);
    em_dur_init(em);
    World_calc(em);
}

/* 0x10BF30: step 1 of enemy_mv, one frame of a live monster. */
void em_move(EMW *em) {
    int flag_was_set = 0;
    u8 k;
    u8 c;
    f32 pos[3];
    f32 *ikari;

    Em_Master_Change();
    if (Online_ck() == 1 && EB(0x9E2) != 0) {
        net_receive_em_act(em);
    }
    pl_timer_calc(em);
    hit_stop_calc(em);
    if (EB(0x877) == 0) {
        if (game_w.stage == em->stg) {
            EB(0x877) = 1;
            GetGroundHitStatusAreaEm(em, em->pos, (u8 *)em + 0x70C, &EF(0x5AC));
            if (em->x388 == 0) {
                em->pos[1] = EF(0x5AC);
            }
        }
    } else if (game_w.stage != em->stg) {
        EB(0x877) = 0;
    }
    if (EB(0x9F1) == 0) {
        EF(0x5A0) = em->pos[0];
        EF(0x5A4) = em->pos[1];
        EF(0x5A8) = em->pos[2];
    }
    if (game_w.stage != em->stg) {
        ESB(0x8BA) = 2;
    }
    if (em->mode != 5) {
        if (ESB(0x9E1) != 0) {
            ESB(0x9E1)--;
        } else {
            kehai_set(em);
            Stage_Hate_Add(em);
            if (EB(0x734) == 3 && EB(0x8C2) != 1) {
                kehai_ck(em);
            }
            em_eye_search_set(em);
            k = em->kind;
            if (k == 0x14 || k == 0xF || k == 0xB || k == 8 || k == 1) {
                if (em_cancel_act_ck(em, 2) == 0 && EB(0x8C3) == 0 && EB(0x889) == 0) {
                    c = smell_ck(em, 0x23);
                } else {
                    c = 0;
                }
                if (EB(0x888) == 0 && c == 1 && em_cancel_act_ck(em, 2) == 0 && EB(0x8BD) == 0) {
                    EB(0x917) |= 2;
                }
            }
            if (EB(0x8B8) != 0 && em->mode != 4 && EB(0x8BD) == 0 && em_cancel_act_ck(em, 8) == 0) {
                ikari_flag_set(em);
            }
            em_escape_action_ck(em);
            k = em->kind;
            c = pl_ninshiki_ck(em);
            if ((k == 9 || k == 4) && EB(0x888) == 0) {
                c = 0;
            }
            if (EB(0x888) == 0 && c != 0 && em_cancel_act_ck(em, 0x20) == 0 && EB(0x8C2) != 1
                && EB(0x8BD) == 0) {
                EB(0x839) = 1;
                EB(0x917) |= 0x20;
                EB(0x917) &= 0xFD;
            }
            if (EB(0x888) == 1 && c != 0) {
                EH(0x886) = em_atk_mode_timer_tbl[em->kind];
            }
            if (EB(0x881) == 1 && EB(0x882) == 0 && ESB(0x617) != -1) {
                em_pl_pos_set(em, (u8)ESB(0x617), pos);
                SetVector(&EF(0x934), pos[0], pos[1], pos[2]);
            }
        }
    }
    if (ESB(0x884) > 0) {
        ESB(0x884)--;
    }
    if (ESB(0x885) > 0) {
        ESB(0x885)--;
    }
    if (ESB(0x9EA) > 0) {
        ESB(0x9EA)--;
    }
    if (ESB(0x9EF) > 0) {
        ESB(0x9EF)--;
    }
    if (ESB(0x8BB) != 0) {
        if (--ESB(0x8BB) <= 0) {
            ESB(0x8BB) = 0;
        }
    }
    if (EB(0x8B6) != 0) {
        ikari = em_ikari_data_tbl[em->kind];
        if (ikari != 0) {
            EF(0x930) = ikari[1];
        }
        if (--EH(0x8B4) <= 0) {
            EH(0x8B4) = 0;
            EB(0x8B6) = 0;
            EH(0x8B2) = 0;
            EF(0x7D8) = em_def_attack_set(em);
            EF(0x7DC) = em_def_defence_set(em);
            EF(0x930) = 1.0f;
        }
    }
    switch (em->kind) {
    case 1:
    case 6:
    case 11:
    case 20:
        c = Em_Unko_Smoke_Ck(em);
        if (EB(0x888) == 0 && c == 1 && em_cancel_act_ck(em, 0x40) == 0 && EB(0x8BD) == 0) {
            EB(0x839) = 1;
            EB(0x917) |= 0x40;
        }
        break;
    }
    if (EB(0x888) == 1 && EH(0x94A) > 0) {
        EH(0x94A)--;
    }
    if (EB(0x888) == 1 && EH(0x94C) > 0) {
        EH(0x94C)--;
    }
    if (em->mode != 5) {
        if (EH(0x94E) != 0) {
            if (--EH(0x94E) <= 0) {
                EH(0x94E) = 0;
                if (EB(0x7D2) != 2 && EB(0x7D2) != 1) {
                    EB(0x88B) = 1;
                    if (EB(0x8C3) == 0 && em->mode != 4 && em->x388 == 0) {
                        em_eye_dmg_reset_act_set(em);
                    }
                }
            }
        }
        Em_Taisei_Ck(em);
    }
    if (EW(0x74C) != 0) {
        if (em->mode == 2 && !(EF(0x3B8) < 0.0f) && !(EF(0x3BC) <= 0.0f)) {
            em->pos[1] += 30.0f;
        }
        EH(0x7EA) = 2;
    } else {
        EH(0x7EA) = 0;
    }
    switch (em->kind) {
    case 0: case 1: case 11: em01_main(em); break;
    case 17: case 22: em17_main(em); break;
    case 6: case 20: em20_main(em); break;
    case 2: em02_main(em); break;
    case 3: em03_main(em); break;
    case 4: case 5: case 32: em04_main(em); break;
    case 7: em07_main(em); break;
    case 8: case 34: em08_main(em); break;
    case 9: case 23: em09_main(em); break;
    case 10: em10_main(em); break;
    case 12: case 25: em12_main(em); break;
    case 15: em15_main(em); break;
    case 14: case 26: em14_main(em); break;
    case 13: case 16: case 30: em16_main(em); break;
    case 27: case 28: case 31: em27_main(em); break;
    case 18: em18_main(em); break;
    case 21: em21_main(em); break;
    case 19: case 24: em19_main(em); break;
    case 29: em29_main(em); break;
    case 33: em33_main(em); break;
    }
    em->ang[0] = (u16)em->ang[0];
    em->ang[1] = (u16)em->ang[1];
    em->ang[2] = (u16)em->ang[2];
    cpRotMatrixYXZ2(em->ang, &em->mat[0][0]);
    em_neck_move(em);
    Em_Damage_Hate_Set(em);
    Em_Hate_Ck(em);
    EF(0x1A0) = 2.0f * EF(0x930);
    EF(0x1F0) = 2.0f * EF(0x930);
    EF(0x240) = 2.0f * EF(0x930);
    EF(0x290) = 2.0f * EF(0x930);
    if (EB(0x9F1) != 0) {
        if (EW(0x390) & 0x20000) {
            flag_was_set = 1;
        } else {
            pl_flag_set(em, 0x20000);
        }
    }
    frame_move(em);
    if (EB(0x9F1) != 0 && flag_was_set == 0) {
        pl_flag_clr(em, 0x20000);
    }
    HitWallPlayer(em, 0);
    GetGroundHitStatusAreaEm(em, em->pos, (u8 *)em + 0x70C, &EF(0x5AC));
    if (ESB(0x95C) == 0) {
        if (EB(0x7D7) != 0 || em->x388 == 4) {
            EB(0x7D6) = 1;
        } else {
            EB(0x7D6) = 0;
        }
    } else if (--ESB(0x95C) <= 0) {
        ESB(0x95C) = 0;
    }
    if (em->x388 != 2 && em->x388 != 4) {
        if (EB(0x7E9) != 0) {
            k = em->kind;
            if (k != 8 && k != 0xE && k != 0x1A && k != 0x22) {
                EF(0x5AC) = EF(0x7E4);
            }
        }
        if (EB(0x8C3) == 0 || game_w.stage == em->stg) {
            em->pos[1] = EF(0x5AC);
            em->ang[0] = 0;
            em->ang[2] = 0;
        }
    }
    GetEmMaterialData(em);
}

/* 0x10C990: step 2, the dying monster (motion and ground only). */
void em_die(EMW *em) {
    pl_timer_calc(em);
    hit_stop_calc(em);
    EF(0x5A0) = em->pos[0];
    EF(0x5A4) = em->pos[1];
    EF(0x5A8) = em->pos[2];
    if (ESB(0x884) != 0) {
        ESB(0x884)--;
    }
    if (ESB(0x885) != 0) {
        ESB(0x885)--;
    }
    em->x04++;
    em->x01 = 0;
    em->ang[0] = (u16)em->ang[0];
    em->ang[1] = (u16)em->ang[1];
    em->ang[2] = (u16)em->ang[2];
    cpRotMatrixYXZ2(em->ang, &em->mat[0][0]);
    em_neck_move(em);
    Em_Damage_Hate_Set(em);
    Em_Hate_Ck(em);
    EF(0x1A0) = 2.0f * EF(0x930);
    EF(0x1F0) = 2.0f * EF(0x930);
    EF(0x240) = 2.0f * EF(0x930);
    EF(0x290) = 2.0f * EF(0x930);
    frame_move(em);
    HitWallPlayer(em, 0);
    GetGroundHitStatusAreaEm(em, em->pos, (u8 *)em + 0x70C, &EF(0x5AC));
    if (em->x388 != 2 && em->x388 != 4) {
        em->pos[1] = EF(0x5AC);
    }
    GetEmMaterialData(em);
}

/* 0x10CAE0: step 3. */
void em_erase(EMW *em) {
    Quest_enemy_die(em);
    Em_hagi_point_clr(em);
    push_em_work(em);
}

/* 0x10CB20: the monster task. Returns 1 when the work was released. */
int enemy_mv(EMW *em) {
    void *prim;

    switch (em->x04) {
    case 0:
        em_init(em);
        em_effect_move(em);
        World_calc(em);
        return 0;
    case 1:
        em_move(em);
        break;
    case 2:
        em_die(em);
        break;
    case 3:
        em_erase(em);
        return 1;
    }
    em_effect_move(em);
    if (em->x01 != 0 && em->stg == game_w.stage) {
        EW(0x3A8) = ((flConvertRtoS(flArcTan2(-(*(f32 *)(player_work + 0xB4) - em->pos[2]),
                                              *(f32 *)(player_work + 0xAC) - em->pos[0])) & 0xFFFF)
                     + 0x4000) - em->ang[1];
        *(f32 *)((u8 *)EP(0x564) + 0x8) = em->pos[0];
        *(f32 *)((u8 *)EP(0x564) + 0xC) = em->pos[1];
        *(f32 *)((u8 *)EP(0x564) + 0x10) = em->pos[2];
        prim = EP(0x564);
        if (prim != 0) {
            if (EF(0x798) == 1.0f) {
                if (em->kind == 2 || em->kind == 7) {
                    add_prim(ot1, prim, 0x20, 1);
                } else {
                    add_prim(ot1, prim, 0x20, 0);
                }
            } else if (em->kind == 2 || em->kind == 7) {
                add_prim(ot0, prim, 0x40, 1);
            } else {
                add_prim(ot0, prim, 0x40, 0);
            }
        }
    }
    World_calc(em);
    return 0;
}

/* 0x10EBF0: the monster's per-frame sound/effect script, program entry 3. */
void em_effect_move(EMW *em) {
    (*(EM_PROG *)((u8 *)EP(0x3CC) + 0xC))(em);
}

/* 0x10EC00 */
void common_local_init(EMW *em) {
    eft01_set(em, 0);
}

/* 0x10EC10 */
void dummy_em_prog(EMW *em) {
}
