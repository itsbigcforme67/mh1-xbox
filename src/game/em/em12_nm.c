/* em12 - game.bin 0x005AF530-0x005B5290: per-monster AI for monster kind 12.
 * Same layout as the other monsters (em09, em33...): em12_init, em12_act_set,
 * action steps em_act00-13, move states, damage reactions, death and revival,
 * demo, em12_main, the sound/effect script. Field meanings are guesses. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM12W {
    u8 eff;             /* 0x00 em12_effect_move step */
    u8 _pad01[5];
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    u8 _pad08[2];
    u8 x0A;             /* 0x0A */
    u8 _pad0B[5];
    f32 tgt[3];         /* 0x10 target position (flvecCopy source in act 2 / act_set) */
    u8 _pad1C[4];
    f32 dist;           /* 0x20 distance to the target */
    f32 yobi[3];        /* 0x24 position handed to push_em_yobi */
    f32 x30;            /* 0x30 range */
    u8 _pad34[2];
    u8 x36;             /* 0x36 */
    u8 _pad37;
    u8 x38;             /* 0x38 stage */
    u8 _pad39[3];
    u8 x3C;             /* 0x3C */
    u8 x3D;             /* 0x3D */
    u16 x3E;            /* 0x3E */
    u8 _pad40[0x90 - 0x40];
    s8 x90;             /* 0x90 */
} EM12W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;
extern f32 em12_scale_tbl[];
extern EMW em_work[];
extern u8 em_boss_tbl[];
void em12_act_set(EMW *em, int kind, u16 no, int unused);
int em_mode_timer_sub(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
int Em_Yobi_Ck(EMW *, f32 *);
void push_em_yobi(f32 *);
void SetVector(f32 *, f32, f32, f32);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
u8 Em_Smoke_Ck(EMW *);
void em_cmd_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
void Shell11_set(EMW *, int);
void Eft13_set_em(EMW *, int, int);
void Eft20_set(f32, EMW *, int, int);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void eft01_set(PLW *, int);
int Code_Make(int, int, int, int);
void em12_main_sub(EMW *em);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
void Eft02_set4(f32, int, int, int, f32 *);
int Event_flag_ck(int);
extern f32 st45_pos_tbl[][6];
extern f32 st49_pos_tbl_0065BBC0[][6];
extern f32 st51_pos_tbl_0065BC20[][6];
extern f32 st53_pos_tbl_0065BC80[][6];
extern f32 st56_pos_tbl[][6];
void Em_Mode_Chg(EMW *, int, int);
void Tuto_flag_set(int);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void flvecCopy(f32 *, f32 *);
f32 CalcDistanceXZ(f32 *, f32 *);
int calc_vec_ang(f32, f32, f32, f32);
void flvecRotY(f32 *, f32);
void em_cmd_reset(EMW *);
void Eft16_set_ex(EMW *, int, int);
void em_escape_mind_set(EMW *, u8, u8);
void Em_Mahi_Start(EMW *);
void Em_Mahi_End(EMW *);
void em_mahi_eff_set(EMW *, int);
void Quest_enemy_die(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void em12_init(EMW *);
int em_frame_check2(EMW *, int, f32);
int em_frame_check(EMW *, f32, int);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void Quest_enemy_escape(EMW *);
void pull_em_yobi(f32 *);
void em12_next_act_set(EMW *em);
int em_frame_check3(EMW *, int, f32, f32);
void mot_miration_ret(EMW *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
s8 em_search_set(EMW *, int, f32, f32);

void em12_init(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    if (quest_w.x08 == 0) {
        if (game_w.stage == 0x1B) {
            em->pos[0] = 12400.0f + 250.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 7600.0f;
        } else if (game_w.stage == 0x1D) {
            if (em->type & 1) {
                em->pos[0] = 4500.0f + 250.0f * (f32)(u32)em->x13;
                em->pos[1] = -983.0f;
                em->pos[2] = 4500.0f;
            } else {
                em->pos[0] = 18500.0f + 250.0f * (f32)(u32)em->x13;
                em->pos[1] = -875.0f;
                em->pos[2] = 5300.0f;
            }
        } else {
            em->pos[0] = 2400.0f + 250.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 3600.0f;
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em_act_set(em, 0, 1);
    if (em->kind == 12) {
        if (em->type == 0 || em->type == 5) {
            em->x792 = em->x302 = em_hp_vital_set(em, 0x58);
        } else {
            em->x792 = em->x302 = em_hp_vital_set(em, 0x40);
        }
        em->scale[0] = em12_scale_tbl[em->type];
        em->scale[1] = em12_scale_tbl[em->type];
        em->scale[2] = em12_scale_tbl[em->type];
    } else {
        em->x792 = em->x302 = em_hp_vital_set(em, 0xA0);
        em->scale[0] = 1.0f;
        em->scale[1] = 1.0f;
        em->scale[2] = 1.0f;
    }
    em->work08 = 0;
    em->x839 = 1;
    em->x88B = 1;
    w->tgt[0] = em->pos[0];
    w->tgt[1] = em->pos[1];
    w->tgt[2] = em->pos[2];
    em->x8C3 = 0;
    w->x3C = 0;
    em->x734 = 3;
}

void em12_act_set(EMW *em, int kind, u16 no, int unused) {
    EM12W *w = (EM12W *)em->ex;

    switch ((u16)kind) {
    case 1:
        switch (no) {
        case 0:
            flvecCopy(em->tgt_pos, w->tgt);
            w->x3D = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            break;
        case 4:
            break;
        case 5:
        case 8:
            if (em->x881 == 0) {
                w->x3D = 0;
                w->dist = 1000.0f;
                em->work08 = 0xB4;
            } else {
                w->x3D = 1;
                w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
                em->work08 = (int)w->dist / 7;
            }
            if (em->work08 > 0xB4) {
                em->work08 = 0xB4;
            }
            if (em->x881 == 1 && em->x882 == 0) {
                w->dist -= 200.0f;
                if (w->dist < 0.0f) {
                    w->dist = 0.0f;
                }
            }
            break;
        case 9:
            flvecCopy(em->pos, em->tgt_pos);
            flvecCopy(em->x5A0, em->tgt_pos);
            break;
        }
        break;
    }
    em_act_set(em, kind, no);
}

void em12_next_act_set(EMW *em) {
    em->x839 = 1;
    if ((em->mode == 0 && em->x15 == 10) || (em->mode == 0 && em->x15 == 13) || (em->mode == 1 && em->x15 == 6)
        || (em->mode == 1 && em->x15 == 7) || (em->mode == 3 && em->x15 == 2)) {
        em_act_set(em, 0, 13);
    } else {
        em_act_set(em, 0, 1);
    }
}

void em_act00_005AFB20(EMW *em) {
}

static void em_act01_005AFB30(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act02_005AFBB0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;
    f32 v[3];

    flvecCopy(v, w->tgt);
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = (em->x39A & 3) * 30 + 120;
        if (em->char0 != 0x3F3) {
            em_char_set(em, 0xB, 0, 0);
        }
        if ((u32)(u16)((u16)((u16)calc_vec_ang(em->pos[0], em->pos[2], v[0], v[2]) + 0x4000) - em->ang[1]) < 0x8001) {
            em->x07 = 0;
        } else {
            em->x07 = 1;
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (--em->work08 <= 0) {
                em12_next_act_set(em);
            } else {
                int d;

                if (em->x07 != 0) {
                    d = 0x40;
                } else {
                    d = -0x40;
                }
                em->ang[1] += d;
                em->ang[1] = (u16)em->ang[1];
                cpRotMatrix(em->ang, (f32 *)em->mat);
            }
        }
        break;
    }
}

static void em_act03_005AFD10(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = (em->x39A & 7) * 30 + 0x168;
        if (em->char0 != 0x3EB) {
            em_char_set(em, 3, 0, 0);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act04_005AFDB0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0;
        if (em->char0 != 0x3EA) {
            em_char_set(em, 2, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em_char_set(em, 7, 0, 0);
            em->x05++;
            em->work08 = 0;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act05_005AFE70(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = (em->x39A & 3) * 30 + 0xB4;
        em_char_set(em, 4, 0, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act06_005AFF00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 5, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act07_005AFF70(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 6, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act08_005AFFE0(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3F, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) == 0 && em->x1C4 == 0) {
            v[0] = 21.428572f * em->scale[0];
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
            em->pos[0] += v[0];
            em->pos[2] += v[2];
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x44, 0, 0);
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em_act_set(em, 0, 9);
        }
        break;
    }
}

static void em_act09_005B0180(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x42, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act10_005B0210(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1E, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act11_005B0280(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x23, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_act12_005B02F0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    em->x40C = 10;
    em->x40E = 10;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x64;
        if (em->char0 != 0x3F5) {
            em_char_set(em, 0xD, 0, 0);
        }
        break;
    case 1:
        em->work08--;
        em->x798 = (f32)em->work08 / 100.0f;
        if (em->work08 <= 0) {
            if (w->x3C == 1) {
                pull_em_yobi(w->yobi);
                w->x3C = 0;
            }
            Quest_enemy_escape(em);
            em->x04++;
            em->x01 = 0;
        }
        break;
    }
}

static void em_act13_005B03F0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x20, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

void em_move00_005B0460(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 1: em_act01_005AFB30(em); break;
    case 2: em_act02_005AFBB0(em); break;
    case 3: em_act03_005AFD10(em); break;
    case 4: em_act04_005AFDB0(em); break;
    case 5: em_act05_005AFE70(em); break;
    case 6: em_act06_005AFF00(em); break;
    case 7: em_act07_005AFF70(em); break;
    case 8: em_act08_005AFFE0(em); break;
    case 9: em_act09_005B0180(em); break;
    case 10: em_act10_005B0210(em); break;
    case 11: em_act11_005B0280(em); break;
    case 12: em_act12_005B02F0(em); break;
    case 13: em_act13_005B03F0(em); break;
    }
}

void em_move02_005B0580(EMW *em) {
    em_act00_005AFB20(em);
}

void em_mov00_005B0590(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = (em->x39A & 3) * 30 + 0xD2;
        if (em->char0 != 0x3F3) {
            em_char_set(em, 0xB, 0, 0);
        }
        if ((u32)(u16)((u16)((u16)calc_vec_ang(em->pos[0], em->pos[2], em->tgt_pos[0], em->tgt_pos[2]) + 0x4000) - em->ang[1]) < 0x8001) {
            em->x07 = 0;
        } else {
            em->x07 = 1;
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (--em->work08 <= 0) {
                em12_next_act_set(em);
            } else {
                int d;

                if (em->x07 != 0) {
                    d = 0x40;
                } else {
                    d = -0x40;
                }
                em->ang[1] += d;
                em->ang[1] = (u16)em->ang[1];
                cpRotMatrix(em->ang, (f32 *)em->mat);
            }
        }
        break;
    }
}

void em_mov01_005B06E0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x406) {
            *(f32 *)&em->x3AC = 400.0f;
        }
        em_char_set(em, 0x1E, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            if (em_search_set(em, 0x78E4, 700.0f, 300.0f) != -1) {
                if ((u16)(em->horm_ang - em->ang[1]) < 0x3000) {
                    em->x05 = 4;
                    em_char_set(em, 0x22, 0, 0);
                    em->x07 = 0;
                    w->x0A = 0;
                } else if ((u16)(em->horm_ang - em->ang[1]) > 0xD000) {
                    em->x05 = 4;
                    em_char_set(em, 0x21, 0, 0);
                    em->x07 = 1;
                    w->x0A = 0;
                } else if ((u16)(em->horm_ang - em->ang[1]) > 0x5400 && (u16)(em->horm_ang - em->ang[1]) < 0xA600) {
                    em_act_set(em, 3, 0);
                } else {
                    em->x05++;
                    em_char_set(em, 0x20, 0, 0);
                }
            } else {
                em12_next_act_set(em);
            }
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05++;
            em->work08 = (em->x39A & 3) * 30 + 0xD2;
        }
        break;
    case 3:
        if (em_search_set(em, 0x8000, 500.0f, 200.0f) != -1) {
            int d = (u16)(em->horm_ang - em->ang[1]);

            if (d > 0x5400 && d < 0xA600) {
                em_act_set(em, 3, 0);
            } else {
                em->work08--;
                if (em_frame_check2(em, 0, 14.0f) != 0) {
                    if (d < 0x8000) {
                        if (em->work08 <= 0) {
                            em->x05++;
                            em_char_set(em, 0x22, 0, 0);
                            em->x07 = 0;
                            w->x0A = 0;
                        }
                        em->ang[1] -= 0x42;
                    } else {
                        if (em->work08 <= 0) {
                            em->x05++;
                            em_char_set(em, 0x21, 0, 0);
                            em->x07 = 1;
                            w->x0A = 0;
                        }
                        em->ang[1] += 0x42;
                    }
                }
            }
        } else {
            em->x05 = 5;
            em_char_set(em, 0x23, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05 = 2;
            em_char_set(em, 0x20, 0, 0);
        } else {
            if (em_frame_check(em, 6.0f, 0) != 0) {
                w->x0A++;
            } else if (em_frame_check(em, 122.0f, 0) != 0) {
                w->x0A = 0;
            }
            if (w->x0A != 0) {
                if (em->x07 == 0) {
                    em->ang[1] -= 0x1D7;
                } else {
                    em->ang[1] += 0x1D7;
                }
            }
        }
        break;
    case 5:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_mov02_005B0AE0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x15, 0, 0);
        w->x0A = 0;
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
            w->x0A = 0;
        } else {
            if (em_frame_check(em, 42.0f, 0) != 0) {
                w->x0A++;
            } else if (em_frame_check(em, 170.0f, 0) != 0) {
                w->x0A = 0;
            }
            if (w->x0A != 0) {
                em->ang[1] += 0x100;
            }
        }
        break;
    }
}

static void em_mov03_005B0BD0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x16, 0, 0);
        w->x0A = 0;
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
            w->x0A = 0;
        } else {
            if (em_frame_check(em, 42.0f, 0) != 0) {
                w->x0A++;
            } else if (em_frame_check(em, 170.0f, 0) != 0) {
                w->x0A = 0;
            }
            if (w->x0A != 0) {
                em->ang[1] -= 0x100;
            }
        }
        break;
    }
}

static void em_mov04_005B0CC0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->x3E = em->horm_ang - em->ang[1];
        if (w->x3E < 0x8000) {
            em_char_set(em, 0x15, 0, 0);
        } else {
            em_char_set(em, 0x16, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        } else if (em_frame_check3(em, 0, 42.0f, 170.0f) != 0) {
            em->ang[1] += (u16)(u32)((f32)(s16)w->x3E / 66.0f);
        }
        break;
    }
}

static void em_mov05_005B0E40(EMW *em) {
    EM12W *w = (EM12W *)em->ex;
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x12C;
        if (em->char0 != 0x3F5) {
            em_char_set(em, 0xD, 0, 0);
        }
        em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        if (em->x889 == 1) {
            em->x95E |= 0x80;
        }
        break;
    case 1:
        if (w->x3D != 0) {
            em->ang[1] += (s16)(u16)((s16)Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]) / 16;
        }
        mot_miration_ret(em, v);
        w->dist -= v[2];
        if (w->dist <= 0.0f) {
            em->work08 = 1;
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_mov06_005B0FB0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->x3E = em->horm_ang - em->ang[1] + 0x8000;
        em_char_set(em, 0x20, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        } else if (em_frame_check3(em, 0, 14.0f, 332.0f) != 0) {
            em->ang[1] += (u16)(u32)((f32)(s16)w->x3E / 159.0f);
        }
        break;
    }
}

static void em_mov07_005B1100(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->x3E = em->horm_ang - em->ang[1] + 0x8000;
        if (w->x3E < 0x8000) {
            em_char_set(em, 0x21, 0, 0);
        } else {
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        } else if (em_frame_check3(em, 0, 6.0f, 122.0f) != 0) {
            em->ang[1] += (u16)(u32)((f32)(s16)w->x3E / 58.0f);
        }
        break;
    }
}

static void em_mov08_005B1280(EMW *em) {
    EM12W *w = (EM12W *)em->ex;
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3F3) {
            em_char_set(em, 0xB, 0, 0);
        }
        em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (w->x3D != 0) {
            em->ang[1] += (s16)(u16)((s16)Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]) / 16;
        }
        mot_miration_ret(em, v);
        w->dist -= v[2];
        if (w->dist <= 0.0f) {
            em->work08 = 1;
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_mov09_005B13C0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em12_next_act_set(em);
        break;
    }
}

void em_move01_005B1400(EMW *em) {
    switch (em->x15) {
    case 0: em_mov00_005B0590(em); break;
    case 1: em_mov01_005B06E0(em); break;
    case 2: em_mov02_005B0AE0(em); break;
    case 3: em_mov03_005B0BD0(em); break;
    case 4: em_mov04_005B0CC0(em); break;
    case 5: em_mov05_005B0E40(em); break;
    case 6: em_mov06_005B0FB0(em); break;
    case 7: em_mov07_005B1100(em); break;
    case 8: em_mov08_005B1280(em); break;
    case 9: em_mov09_005B13C0(em); break;
    }
}

static void em_atk00_005B14E0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_atk01_005B1550(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 6, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 102.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0x24, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_atk02_005B1610(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x25, 0, 0);
        break;
    case 1:
        if (em_frame_check2(em, 0, 128.0f) == 0 && em->x1C4 == 0) {
            em->ang[1] += 0x1C7;
        }
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_atk03_005B16C0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x24, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_atk04_005B1730(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 7, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

void em_move03_005B17E0(EMW *em) {
    switch (em->x15) {
    case 0: em_atk00_005B14E0(em); break;
    case 1: em_atk01_005B1550(em); break;
    case 2: em_atk02_005B1610(em); break;
    case 3: em_atk03_005B16C0(em); break;
    case 4: em_atk04_005B1730(em); break;
    }
}

static void em_dm00_005B1880(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x424) {
            em_char_set(em, 0x3C, 0, 0);
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em_act_set(em, 0, 1);
            em->x302 = 0x40;
        } else if (em_frame_check(em, 2.0f, 0) != 0) {
            Eft16_set_ex(em, 3, 0xD);
            Eft16_set_ex(em, 4, 0xE);
        }
        break;
    }
}

static void em_dm01_005B1950(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3E, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 22.0f) == 0 && em->x1C4 == 0) {
            v[0] = -10.909091f * em->scale[0];
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
            em->pos[0] += v[0];
            em->pos[2] += v[2];
        }
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_dm02_005B1A90(EMW *em) {
    EM12W *w = (EM12W *)em->ex;
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3D, 0, 0);
        w->x0A = 0;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 22.0f) == 0 && em->x1C4 == 0) {
            v[0] = 10.909091f * em->scale[0];
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
            em->pos[0] += v[0];
            em->pos[2] += v[2];
        }
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_dm03_005B1BE0(EMW *em, int arg) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        if (arg == 0) {
            em->x05++;
            em_char_set(em, 0x3E, 0, 0);
        } else {
            em->x05 = 2;
            if (em->char0 != 0x427) {
                em_char_set(em, 0x3F, 0, 0);
            }
        }
        em_cmd_reset(em);
        break;
    case 1:
        v[0] = -10.909091f * em->scale[0];
        v[1] = 0.0f;
        v[2] = 0.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
        em->pos[0] += v[0];
        em->pos[2] += v[2];
        if (em_frame_check(em, 12.0f, 0) == 0) {
            em->x05 = 2;
            em_char_set(em, 0x43, 0, 0);
        }
        break;
    case 2:
        if (arg == 0) {
            if (em_frame_check2(em, 0, 8.0f) == 0 && em->x1C4 == 0) {
                v[0] = -10.909091f * em->scale[0];
                v[1] = 0.0f;
                v[2] = 0.0f;
                flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
                em->pos[0] += v[0];
                em->pos[2] += v[2];
            }
        } else if (em_frame_check2(em, 0, 30.0f) == 0 && em->x1C4 == 0) {
            v[0] = 21.428572f * em->scale[0];
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
            em->pos[0] += v[0] * em->scale[0];
            em->pos[2] += v[2];
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0xC8;
            em_char_set(em, 0x40, 0, 0);
        }
        break;
    case 3:
        if (--em->work08 <= 0 && em_frame_check(em, 98.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0x42, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_dm04_005B1FA0(EMW *em) {
    EM12W *w = (EM12W *)em->ex;
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->x39A & 1) {
            em_char_set(em, 0x3D, 0, 0);
            w->x0A = 0;
        } else {
            em_char_set(em, 0x3E, 0, 0);
            w->x0A = 1;
        }
        if (em->x889 != 1) {
            em_escape_mind_set(em, 4, 0x90);
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 22.0f) == 0 && em->x1C4 == 0) {
            if (w->x0A == 0) {
                v[0] = 10.909091f * em->scale[0];
            } else {
                v[0] = -10.909091f * em->scale[0];
            }
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
            em->pos[0] += v[0];
            em->pos[2] += v[2];
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = em->x94E;
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_dm05_005B2180(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_cmd_reset(em);
        em_char_set(em, 0x3D, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        break;
    case 1:
        if (em_frame_check2(em, 0, 22.0f) == 0 && em->x1C4 == 0) {
            v[0] = 10.909091f * em->scale[0];
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
            em->pos[0] += v[0];
            em->pos[2] += v[2];
        }
        if (em_frame_check(em, 30.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0x45, 0, 0);
        }
        break;
    case 2:
        em_mahi_eff_set(em, 2);
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x3D, 6, 0x1E);
            Em_Mahi_End(em);
        }
        break;
    case 3:
        if (em_frame_check2(em, 0, 22.0f) == 0 && em->x1C4 == 0) {
            v[0] = 10.909091f * em->scale[0];
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1])));
            em->pos[0] += v[0] * em->scale[0];
            em->pos[2] += v[2];
        }
        if (em->x194 == 0) {
            em->x05++;
            em12_next_act_set(em);
        }
        break;
    }
}

void em_move04_005B2440(EMW *em) {
    em->ex[0x90] = 1;
    switch (em->x15) {
    case 0: em_dm00_005B1880(em); break;
    case 1: em_dm01_005B1950(em); break;
    case 2: em_dm02_005B1A90(em); break;
    case 3: em_dm03_005B1BE0(em, 0); break;
    case 4: em_dm03_005B1BE0(em, 1); break;
    case 5: em_dm04_005B1FA0(em); break;
    case 6: em_dm05_005B2180(em); break;
    }
}

#define EM12_WALK(em, v, k) \
    do { \
        v[0] = (k) * em->scale[0]; \
        v[1] = 0.0f; \
        v[2] = 0.0f; \
        flvecRotY(v, DEG2RAD(ANG2DEG(em->ang[1]))); \
        em->pos[0] += v[0]; \
        em->pos[2] += v[2]; \
    } while (0)

static void em_die00_005B24F0(EMW *em, int arg) {
    EM12W *w = (EM12W *)em->ex;
    f32 v[3];

    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        Quest_enemy_die(em);
        switch (em->char0) {
        case 0x427:
        case 0x42B:
            em->x05 = 2;
            break;
        case 0x428:
            em->work08 = 0x28;
            em->x05 = 3;
            break;
        case 0x429:
            em->work08 = 0x1E;
            em->x05 = 4;
            break;
        default:
            if (arg == 0) {
                em->x05++;
                em_char_set(em, 0x3E, 0, 0);
            } else {
                em->x05 = 2;
                em_char_set(em, 0x3F, 0, 0);
            }
            break;
        }
        break;
    case 1:
        EM12_WALK(em, v, -10.909091f);
        if (em_frame_check(em, 12.0f, 0) == 0) {
            em->x05 = 2;
            em_char_set(em, 0x43, 0, 0);
        }
        break;
    case 2:
        if (arg == 0) {
            if (em_frame_check2(em, 0, 8.0f) == 0 && em->x1C4 == 0) {
                EM12_WALK(em, v, -10.909091f);
            }
        } else if (em_frame_check2(em, 0, 30.0f) == 0 && em->x1C4 == 0) {
            EM12_WALK(em, v, 21.428572f);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x28;
            em_char_set(em, 0x40, 0, 0);
        }
        break;
    case 3:
        if (--em->work08 <= 0 && em_frame_check(em, 98.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0x41, 0, 0);
        }
        break;
    case 4:
        if (em_frame_check2(em, 0, 50.0f) != 0) {
            em->x05++;
            em->act_spd = 0.0f;
            Tuto_flag_set(0);
            if (em->kind == 12) {
                em->work08 = 0x960;
            } else {
                em->work08 = 0x708;
            }
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 5:
        em->act_spd = 0.0f;
        if (Em_hagi_point_cnt_ck(em) <= 0 || --em->work08 <= 0) {
            Em_hagi_point_clr(em);
            em->x05++;
            em->work08 = 0x6E;
        }
        break;
    case 6:
        em->act_spd = 0.0f;
        em->work08--;
        if (em->work08 < 100) {
            em->x798 = (f32)em->work08 / 100.0f;
        }
        if (em->work08 <= 0) {
            if (w->x3C == 1) {
                pull_em_yobi(w->yobi);
                w->x3C = 0;
            }
            em->x01 = 0;
            em_act_set(em, 5, 3);
        }
        break;
    }
}

static void em_die01_005B2A60(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    em->x40C = 10;
    em->x40E = 10;
    switch (em->x05) {
    case 0:
        Quest_enemy_die(em);
        em->x05++;
        em->work08 = 0x64;
        if (em->char0 != 0x3F3) {
            em_char_set(em, 0xB, 0, 0);
        }
        break;
    case 1:
        em->work08--;
        em->x798 = (f32)em->work08 / 100.0f;
        if (em->work08 <= 0) {
            if (w->x3C == 1) {
                pull_em_yobi(w->yobi);
                w->x3C = 0;
            }
            em->x01 = 0;
            em_act_set(em, 5, 3);
        }
        break;
    }
}

#define EM12_REV_POS(em, tbl, v, act) \
    do { \
        flvecCopy(em->pos, tbl[em->type]); \
        flvecCopy(em->x5A0, em->pos); \
        flvecCopy(v, &tbl[em->type][3]); \
        em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF; \
        em_act_set(em, 7, act); \
        em->x798 = 0.0f; \
    } while (0)

static void em_die_rev_005B2B70(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck(em) == 1) {
            em->x05++;
        } else {
            em->x04++;
        }
        break;
    case 1:
        em_status_init(em);
        em12_init(em);
        Quest_enemy_revival_set(em);
        em_cmd_reset(em);
        em->x839 = 0;
        switch (em->stg) {
        default:
            Quest_enemy_escape(em);
            em->x04++;
            em->x01 = 0;
            break;
        case 0x2D:
            EM12_REV_POS(em, st45_pos_tbl, v, 1);
            break;
        case 0x31:
            EM12_REV_POS(em, st49_pos_tbl_0065BBC0, v, 0);
            break;
        case 0x33:
            EM12_REV_POS(em, st51_pos_tbl_0065BC20, v, 0);
            break;
        case 0x35:
            EM12_REV_POS(em, st53_pos_tbl_0065BC80, v, 0);
            break;
        case 0x38:
            EM12_REV_POS(em, st56_pos_tbl, v, 1);
            break;
        }
        break;
    }
}

void em_move05_005B2F10(EMW *em) {
    em->x40C = 10;
    em->x40E = 10;
    switch (em->x15) {
    case 0: em_die00_005B24F0(em, 0); break;
    case 1: em_die00_005B24F0(em, 1); break;
    case 2: em_die01_005B2A60(em); break;
    case 3: em_die_rev_005B2B70(em); break;
    }
}

void em12_blood_req(EMW *em, int unused, f32 f) {
    f32 v[3];
    f32 p[3];
    FLMAT m;
    int a;

    v[0] = -55.0f * em->scale[0];
    v[1] = em->scale[1] * (f * (f32)(((u16)ran_suu(1) & 0x3F) - 0x20));
    v[2] = em->scale[2] * (f * (f32)(((u16)ran_suu(1) & 0x7F) - 0x20));
    flmatCopy(&m, get_joint_wmat_em(em, 2));
    flvecApplyMat33_2(v, &m);
    p[0] = m[3][0] + v[0];
    p[1] = m[3][1] + v[1];
    p[2] = m[3][2] + v[2];
    v[0] = -1.0f;
    v[1] = -0.2f;
    v[2] = 0.0f;
    flvecApplyMat33_2(v, &m);
    a = (u16)-(u16)(s32)(0.5f + 65536.0f * flArcTan2(v[1], flSqrt(v[0] * v[0] + v[2] * v[2])) / 6.2831855f);
    Eft02_set4(f, a, (u16)(s32)(0.5f + 65536.0f * flArcTan2(v[0], v[2]) / 6.2831855f), 3, p);
}

static void em_demo00_005B3180(EMW *em) {
    em->x40E = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x425) {
            em_char_set(em, 0x3D, 0, 0);
        }
        em->x8BD = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            em_act_set(em, 6, 1);
        }
        break;
    }
}

static void em_demo01_005B3220(EMW *em) {
    f32 v[3];

    if (!(*(u16 *)&game_w.x1E & 3)) {
        em12_blood_req(em, 0, 0.8f);
    }
    em->x40E = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x427) {
            em_char_set(em, 0x3F, 0, 0);
        }
        em->x8BD = 1;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) == 0 && em->x1C4 == 0) {
            EM12_WALK(em, v, 21.428572f);
        }
        if (em->x194 == 0) {
            em->x05++;
        }
        break;
    case 2:
        break;
    }
}

static void em_demo02_005B33B0(EMW *em) {
    EMW *t;

    em->x40E = 5;
    if (!(*(u16 *)&game_w.x1E & 3)) {
        em12_blood_req(em, 0, 0.6f);
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x428) {
            em_char_set(em, 0x40, 0, 0);
        }
        em->x8BD = 1;
        break;
    case 1:
        t = em->x944;
        if (t != 0) {
            if (t->kind == 1 || t->kind == 0xB) {
                if (t->mode != 6) {
                    em_act_set(em, 6, 3);
                }
            }
        }
        break;
    }
}

static void em_demo03_005B34A0(EMW *em) {
    EMW *t;

    em->x40E = 5;
    em->x839 = 0;
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x429) {
            em_char_set(em, 0x41, 0, 0);
        }
        em->x8BD = 1;
        break;
    case 1:
        if (!(*(u16 *)&game_w.x1E & 3)) {
            em12_blood_req(em, 0, 0.3f);
        }
        t = em->x944;
        if (t != 0) {
            if (t->kind == 1 || t->kind == 0xB) {
                if (t->mode != 6) {
                    em->act_spd = 0.0f;
                }
            }
            if (Event_flag_ck(0xE) == 1) {
                em->x05++;
                em->x302 = 0;
                em->act_spd = 1.0f;
                em_act_set(em, 5, 0);
            }
        }
        break;
    }
}

static void em_demo04_005B35D0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->act_spd = 1.0f;
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        if (Event_flag_ck(0xE) == 1) {
            em12_next_act_set(em);
        }
        break;
    }
}

static void em_demo05_005B3660(EMW *em) {
    em->x40E = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->act_spd = 1.0f;
        em_char_set(em, 1, 0, 0);
        em->pos[0] = 17788.5f;
        em->pos[1] = 0.0f;
        em->pos[2] = 17468.301f;
        em->ang[1] = 0x63FF;
        break;
    case 1:
        break;
    }
}

void em_demo06(EMW *em) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 1, 0, 0);
        em->x01 = 0;
        em->x839 = 0;
        break;
    case 1:
        if (Event_flag_ck(0x10) == 1) {
            em->x05++;
            em->x01 = 1;
            em12_next_act_set(em);
        }
        break;
    }
}

void em_move06_005B3790(EMW *em) {
    switch (em->x15) {
    case 0: em_demo00_005B3180(em); break;
    case 1: em_demo01_005B3220(em); break;
    case 2: em_demo02_005B33B0(em); break;
    case 3: em_demo03_005B34A0(em); break;
    case 4: em_demo04_005B35D0(em); break;
    case 5: em_demo05_005B3660(em); break;
    case 6: em_demo06(em); break;
    }
}

static void em_revival00_005B3840(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0xD, 0, 0);
        em->work08 = 0x64;
        break;
    case 1:
        em->x798 += 0.01f;
        if (!(em->x798 <= 1.0f)) {
            em->x798 = 1.0f;
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em12_next_act_set(em);
            em->x9E1 = 0;
            em->x798 = 1.0f;
        }
        break;
    }
}

static void em_revival01_005B3910(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0xB, 0, 0);
        em->work08 = 0x64;
        break;
    case 1:
        em->x798 += 0.01f;
        if (!(em->x798 <= 1.0f)) {
            em->x798 = 1.0f;
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em12_next_act_set(em);
            em->x9E1 = 0;
            em->x798 = 1.0f;
        }
        break;
    }
}

void em_move07_005B39E0(EMW *em) {
    em->x40C = 10;
    em->x40E = 10;
    em->x9E1 = 5;
    switch (em->x15) {
    case 0: em_revival00_005B3840(em); break;
    case 1: em_revival01_005B3910(em); break;
    }
}

void em12_main(EMW *em) {
    EM12W *w = (EM12W *)em->ex;
    EMW *e = em_work;
    u8 dmg[4];
    u8 boss_hit = 0;
    u8 boss_idle = 0;
    u8 revived = 0;
    u8 hit = 0;
    s16 i;
    int yobi;

    yobi = (u8)Em_Yobi_Ck(em, w->yobi);
    if (w->x3C == 0 && yobi != 0) {
        em->x917 |= 4;
        w->x3C = 2;
        if (em->x889 != 1) {
            em_escape_mind_set(em, 2, 0x90);
        }
    }
    if (w->x3C == 1) {
        pull_em_yobi(w->yobi);
        w->x3C = 2;
    }
    if (em->x889 != 1 && em->mode != 6 && em->mode != 7) {
        for (i = 0; i < 20; i++, e++) {
            if (e->be_flag != 0 && e->x01 != 0 && (u8)Pl_stg_ck_tw(em, (PLW *)e) != 0) {
                if (em_boss_tbl[e->kind] != 0) {
                    if (e->x888 == 1) {
                        boss_hit = em_boss_tbl[e->kind];
                        break;
                    } else if (e->x888 == 0) {
                        boss_idle = em_boss_tbl[e->kind];
                    }
                } else if (e->kind == 12 && e->type == 0 && e->mode == 6) {
                    revived = 1;
                }
            }
        }
        if ((boss_hit & 0xFF) != 0 || ((boss_idle & 0xFF) != 0 && (revived & 0xFF) != 0)) {
            em_escape_mind_set(em, 1, 0x90);
            em->x839 = 1;
        }
    }
    em_mode_timer_sub(em);
    if (em->mode != 6) {
        switch (Em_Dmg_Sys(em, dmg)) {
        case 1:
        case 2: {
            int a = (u16)(em->dm_ang - em->ang[1]);

            if (em->mode != 5) {
                if (a < 0x8001) {
                    em_act_set(em, 5, 1);
                } else {
                    em_act_set(em, 5, 0);
                }
            }
            em->x839 = 0;
            break;
        }
        case 6:
            if (em->mode != 4 || em->x15 != 6) {
                em_mahi_dmg_timer_set(em);
                em_act_set(em, 4, 6);
            }
            hit = 1;
            break;
        case 8:
            if (em->mode != 0 || em->x15 != 8) {
                em_sleep_dmg_timer_set(em);
                em_act_set(em, 0, 8);
                em->x839 = 0;
            }
            hit = 1;
            break;
        case 10:
            switch (em->x15) {
            case 8:
                em_act_set(em, 0, 9);
                em->x839 = 0;
                break;
            }
            hit = 1;
            break;
        case 13:
            hit = 1;
            break;
        case 12:
            if (em->kind != 0x19) {
                int a = (u16)(em->dm_ang - em->ang[1]);

                hit = 1;
                if (em->kind == 12 && (f32)em->x302 > 0.2f * (f32)em->x792) {
                    if (a < 0x8001) {
                        em_act_set(em, 4, 0);
                    } else {
                        em_act_set(em, 4, 2);
                    }
                } else if (!(em->mode == 4 && (em->x15 == 3 || em->x15 == 4))) {
                    if (a < 0x8001) {
                        em_act_set(em, 4, 3);
                    } else {
                        em_act_set(em, 4, 4);
                    }
                }
                em->x839 = 0;
            }
            break;
        }
        if (hit != 0) {
            if (em->x889 != 1) {
                em_escape_mind_set(em, 2, 0x90);
            }
            if (w->x3C == 0) {
                SetVector(w->yobi, em->pos[0], em->pos[1], em->pos[2]);
                w->x30 = 1000.0f;
                w->x38 = em->stg;
                w->x36 = 2;
                push_em_yobi(w->yobi);
                w->x3C = 1;
            }
        }
    }
    if (em->x889 != 1 && em->mode != 7 && em->mode != 6 && Em_Smoke_Ck(em) == 1) {
        em_escape_mind_set(em, 3, 0x90);
        em->x839 = 1;
    }
    switch (quest_w.x08) {
    case 0x8B:
        if (em->stg == 0x21 && Event_flag_ck(0xE) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                if (em->type == 0) {
                    em12_act_set(em, 6, 5, 0);
                } else {
                    em12_act_set(em, 6, 4, 0);
                }
            }
            break;
        }
        goto cmd;
    case 0x94:
        if (em->stg == 0x2E && Event_flag_ck(0x10) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                em12_act_set(em, 6, 6, 0);
            }
            break;
        }
        goto cmd;
    default:
    cmd:
        switch (em->x734) {
        case 3:
            if (em->x839 != 0) {
                em_cmd_ck(em);
                em->x839 = 0;
            }
            break;
        }
        break;
    }
    em12_main_sub(em);
    if (em->x6FF != 0) {
        em12_main_sub(em);
        em->x6FF = 0;
    }
}

void em12_main_sub(EMW *em) {
    switch (em->mode) {
    case 0: em_move00_005B0460(em); break;
    case 1: em_move01_005B1400(em); break;
    case 2: em_move02_005B0580(em); break;
    case 3: em_move03_005B17E0(em); break;
    case 4: em_move04_005B2440(em); break;
    case 5: em_move05_005B2F10(em); break;
    case 6: em_move06_005B3790(em); break;
    case 7: em_move07_005B39E0(em); break;
    }
}

/* Sound and effect script per animation (sound_call(em, frame, se, joint)
 * plays a sound at the joint once the animation reaches the frame). */
static void sound_call_005B5150(EMW *em, int frame, int se, int joint);

static void ef_move_sub_005B4190(EMW *em, EM12W *w) {
    f32 v[3];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_005B5150(em, 0x100, 9, 0x10);
        break;
    case 0x3EA:
        sound_call_005B5150(em, 0x20, 0xA, 0x10);
        sound_call_005B5150(em, 0x86, 0xB, 0x10);
        sound_call_005B5150(em, 0xA0, 8, 0x10);
        sound_call_005B5150(em, 0x26, 0x14, 0);
        sound_call_005B5150(em, 0x52, 5, 0x14);
        sound_call_005B5150(em, 0xCE, 4, 0x14);
        break;
    case 0x3EC:
        sound_call_005B5150(em, 0x60, 0xB, 0x10);
        sound_call_005B5150(em, 0xCC, 0xB, 0x10);
        break;
    case 0x3ED:
        sound_call_005B5150(em, 0x46, 7, 0x10);
        sound_call_005B5150(em, 0x6A, 8, 0x10);
        break;
    case 0x3EE:
        sound_call_005B5150(em, 0x20, 8, 0x10);
        break;
    case 0x3EF:
        sound_call_005B5150(em, 0x2C, 7, 0x10);
        sound_call_005B5150(em, 0x28, 3, 6);
        sound_call_005B5150(em, 0x26, 2, 0xA);
        if (em_frame_check(em, 40.0f, 0) != 0) {
            Shell11_set(em, 0xC);
            break;
        }
        break;
    case 0x3F3:
        sound_call_005B5150(em, 0x20, 1, 6);
        sound_call_005B5150(em, 0x6A, 1, 0xA);
        sound_call_005B5150(em, 0xBA, 1, 6);
        sound_call_005B5150(em, 0x2E, 4, 0x14);
        sound_call_005B5150(em, 0x84, 4, 0x17);
        sound_call_005B5150(em, 0xD0, 4, 0x14);
        break;
    case 0x3F4:
        sound_call_005B5150(em, 0x26, 3, 6);
        sound_call_005B5150(em, 0x30, 3, 0xA);
        sound_call_005B5150(em, 0xC, 6, 0x17);
        sound_call_005B5150(em, 0x12, 6, 0x14);
        break;
    case 0x3F5:
        sound_call_005B5150(em, 0x14, 3, 0xA);
        sound_call_005B5150(em, 0x4E, 3, 0xA);
        sound_call_005B5150(em, 0x34, 3, 6);
        sound_call_005B5150(em, 0x1A, 6, 0x14);
        sound_call_005B5150(em, 0x3A, 6, 0x17);
        if (em_frame_check(em, 24.0f, 0) != 0) {
            Eft13_set_em(em, 0x17, 0x14);
        }
        if (em_frame_check(em, 52.0f, 0) != 0) {
            Eft13_set_em(em, 0x14, 0x14);
            break;
        }
        break;
    case 0x3FD:
        sound_call_005B5150(em, 0x40, 1, 6);
        sound_call_005B5150(em, 0x60, 1, 0xA);
        sound_call_005B5150(em, 0x8A, 4, 0x17);
        sound_call_005B5150(em, 0xAE, 4, 0x14);
        break;
    case 0x3FE:
        sound_call_005B5150(em, 0x40, 1, 0xA);
        sound_call_005B5150(em, 0x60, 1, 6);
        sound_call_005B5150(em, 0x8A, 4, 0x14);
        sound_call_005B5150(em, 0xAE, 4, 0x17);
        break;
    case 0x406:
        sound_call_005B5150(em, 0x46, 7, 0x10);
        sound_call_005B5150(em, 0x46, 4, 0x14);
        sound_call_005B5150(em, 0x20, 4, 0x17);
        sound_call_005B5150(em, 0x36, 2, 6);
        sound_call_005B5150(em, 0x18, 2, 0xA);
        break;
    case 0x407:
        sound_call_005B5150(em, 4, 0xE, 0);
        sound_call_005B5150(em, 0x2E, 0x14, 0x1B);
        sound_call_005B5150(em, 0x3C, 7, 0x10);
        sound_call_005B5150(em, 0x34, 4, 0x14);
        sound_call_005B5150(em, 0x7A, 1, 0x17);
        sound_call_005B5150(em, 0xA6, 5, 0x14);
        sound_call_005B5150(em, 0xD2, 2, 0xA);
        if (em_frame_check(em, 30.0f, 0) != 0) {
            if (em->kind == 12) {
                Shell11_set(em, 0);
                break;
            }
            Shell11_set(em, 8);
            break;
        }
        break;
    case 0x408:
        sound_call_005B5150(em, 0x46, 0x13, 0x10);
        sound_call_005B5150(em, 0xE6, 0x13, 0x10);
        sound_call_005B5150(em, 0xCE, 4, 0x14);
        sound_call_005B5150(em, 0x144, 4, 0x14);
        sound_call_005B5150(em, 0x38, 4, 0x17);
        sound_call_005B5150(em, 0xA2, 4, 0x17);
        sound_call_005B5150(em, 0x4C, 2, 6);
        sound_call_005B5150(em, 0x7A, 2, 6);
        sound_call_005B5150(em, 0xFC, 2, 6);
        sound_call_005B5150(em, 0x128, 2, 6);
        sound_call_005B5150(em, 0x62, 2, 0xA);
        sound_call_005B5150(em, 0x90, 2, 0xA);
        sound_call_005B5150(em, 0xE6, 2, 0xA);
        sound_call_005B5150(em, 0x118, 2, 0xA);
        break;
    case 0x409:
    case 0x40A:
        sound_call_005B5150(em, 0x2C, 4, 0x14);
        sound_call_005B5150(em, 0x70, 4, 0x14);
        sound_call_005B5150(em, 0x1C, 4, 0x17);
        sound_call_005B5150(em, 0x5C, 4, 0x17);
        sound_call_005B5150(em, 0x3A, 2, 6);
        sound_call_005B5150(em, 0x7C, 2, 6);
        sound_call_005B5150(em, 0x26, 2, 0xA);
        sound_call_005B5150(em, 0x4A, 2, 0xA);
        break;
    case 0x40B:
        sound_call_005B5150(em, 0x2A, 4, 0x14);
        sound_call_005B5150(em, 0x38, 1, 6);
        sound_call_005B5150(em, 0x5A, 4, 0x17);
        sound_call_005B5150(em, 0x6A, 1, 0xA);
        sound_call_005B5150(em, 0x5A, 0xA, 0x10);
        break;
    case 0x40C:
        if (em_frame_check(em, 20.0f, 0) != 0) {
            Shell11_set(em, 1);
        }
        sound_call_005B5150(em, 0x1A, 0xE, 0x14);
        sound_call_005B5150(em, 0x1A, 0x14, 0x10);
        sound_call_005B5150(em, 0x14, 0x13, 0x10);
        sound_call_005B5150(em, 0x14, 3, 6);
        sound_call_005B5150(em, 0x18, 6, 0x17);
        sound_call_005B5150(em, 0x32, 3, 6);
        sound_call_005B5150(em, 0x44, 4, 0x14);
        break;
    case 0x40D:
        if (em_frame_check(em, 24.0f, 0) != 0) {
            if (em->kind == 12) {
                Shell11_set(em, 2);
            } else {
                Shell11_set(em, 9);
            }
        }
        sound_call_005B5150(em, 0x14, 7, 0x10);
        sound_call_005B5150(em, 0x5C, 7, 0x10);
        sound_call_005B5150(em, 0x1E, 0x14, 0x1B);
        sound_call_005B5150(em, 0x5C, 0x14, 0x1B);
        sound_call_005B5150(em, 0xA, 4, 0x14);
        sound_call_005B5150(em, 0x50, 4, 0x14);
        sound_call_005B5150(em, 0x1E, 4, 0x17);
        sound_call_005B5150(em, 0x5E, 4, 0x17);
        sound_call_005B5150(em, 0x2C, 1, 0xA);
        sound_call_005B5150(em, 0x74, 1, 0xA);
        sound_call_005B5150(em, 0x18, 1, 6);
        sound_call_005B5150(em, 0x42, 1, 6);
        break;
    case 0x424:
        if (em_frame_check(em, 2.0f, 0) != 0) {
            Eft16_set_ex(em, 3, 0xD);
            Eft16_set_ex(em, 4, 0xE);
            break;
        }
        break;
    case 0x425:
        sound_call_005B5150(em, 4, 0x11, 0x10);
        sound_call_005B5150(em, 0x10, 1, 6);
        sound_call_005B5150(em, 0x1A, 1, 0xA);
        sound_call_005B5150(em, 0x68, 0xE, 0x1B);
        sound_call_005B5150(em, 0x6E, 1, 0xA);
        sound_call_005B5150(em, 0x8C, 1, 0x17);
        break;
    case 0x426:
        sound_call_005B5150(em, 4, 0x11, 0x10);
        sound_call_005B5150(em, 0x10, 1, 0xA);
        sound_call_005B5150(em, 0x1A, 1, 6);
        sound_call_005B5150(em, 0x68, 0xE, 0x1B);
        sound_call_005B5150(em, 0x6E, 1, 6);
        sound_call_005B5150(em, 0x8C, 1, 0x14);
        break;
    case 0x427:
        sound_call_005B5150(em, 4, 0x11, 0x10);
        sound_call_005B5150(em, 0x1C, 0x12, 0);
        sound_call_005B5150(em, 0x22, 0xF, 0);
        sound_call_005B5150(em, 0x44, 3, 0xA);
        sound_call_005B5150(em, 0x50, 4, 0x17);
        if (em_frame_check(em, 28.0f, 0) != 0) {
            Eft20_set(0.4f * em->scale[0], em, 0, 0);
            break;
        }
        break;
    case 0x428:
        sound_call_005B5150(em, 0x14, 0x10, 0x10);
        sound_call_005B5150(em, 4, 0x16, 0x10);
        sound_call_005B5150(em, 0x64, 0x15, 0);
        sound_call_005B5150(em, 0xC2, 0x15, 0);
        sound_call_005B5150(em, 0x32, 0xE, 0x14);
        sound_call_005B5150(em, 0x96, 0xE, 0x14);
        if (em_frame_check(em, 46.0f, 0) != 0) {
            Eft13_set_em(em, 0, 6);
        }
        if (em_frame_check(em, 88.0f, 0) != 0) {
            Eft13_set_em(em, 0, 6);
        }
        if (em_frame_check(em, 190.0f, 0) != 0) {
            Eft13_set_em(em, 0, 6);
            break;
        }
        break;
    case 0x429:
        sound_call_005B5150(em, 6, 8, 0x10);
        sound_call_005B5150(em, 0x42, 8, 0x10);
        break;
    case 0x42A:
        sound_call_005B5150(em, 0xA, 0xE, 6);
        sound_call_005B5150(em, 0x14, 0xA, 0x10);
        sound_call_005B5150(em, 0x30, 3, 6);
        sound_call_005B5150(em, 0x4E, 5, 0x14);
        sound_call_005B5150(em, 0x64, 4, 0x17);
        sound_call_005B5150(em, 0x82, 1, 6);
        break;
    case 0x42B:
        sound_call_005B5150(em, 0x1A, 1, 6);
        sound_call_005B5150(em, 0x1A, 0xE, 6);
        sound_call_005B5150(em, 0x18, 0xE, 0x17);
        sound_call_005B5150(em, 0x54, 0x12, 0);
        sound_call_005B5150(em, 0x60, 0xF, 0);
        sound_call_005B5150(em, 0x7E, 3, 6);
        sound_call_005B5150(em, 0x8C, 4, 0x14);
        break;
    case 0x42C:
        sound_call_005B5150(em, 2, 9, 0x10);
        sound_call_005B5150(em, 0xB4, 9, 0x10);
        v[2] = 20.0f;
        v[0] = 0.0f;
        v[1] = 0.0f;
        em_sleep_eff_set(em, 15, v, 1.2f);
        break;
    case 0x42D:
        sound_call_005B5150(em, 6, 0x10, 0x10);
        break;
    }
}


void em12_effect_move(EMW *em) {
    EM12W *w = (EM12W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_005B4190(em, w);
        break;
    }
}

static void sound_call_005B5150(EMW *em, int frame, int se, int joint) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        if (em->scale[0] == 0.5f) {
            Em_se_req2(em, se + 0x20, 0, pos, 5, 0);
        } else {
            Em_se_req2(em, se, 0, pos, 5, 0);
        }
    }
}

void em12_local_init(EMW *em) {
    eft01_set((PLW *)em, 0);
}

void dummy_em_prog_005B5240(void) {
}
