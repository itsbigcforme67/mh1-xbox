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
    u8 _pad01[9];
    u8 x0A;             /* 0x0A */
    u8 _pad0B[5];
    f32 tgt[3];         /* 0x10 target position (flvecCopy source in act 2 / act_set) */
    u8 _pad1C[4];
    f32 dist;           /* 0x20 distance to the target */
    u8 _pad24[0x3C - 0x24];
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

void em12_act_set(EMW *em, int kind, u16 no) {
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
                pull_em_yobi((f32 *)((u8 *)w + 0x24));
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

static void em_move00_005B0460(EMW *em) {
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

static void em_move02_005B0580(EMW *em) {
    em_act00_005AFB20(em);
}

static void em_mov00_005B0590(EMW *em) {
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

static void em_mov01_005B06E0(EMW *em) {
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
                } else if ((u16)(em->horm_ang - em->ang[1]) >= 0xD001) {
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

static void em_move01_005B1400(EMW *em) {
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

static void em_move03_005B17E0(EMW *em) {
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

static void em_move04_005B2440(EMW *em) {
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
                pull_em_yobi((f32 *)((u8 *)w + 0x24));
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
                pull_em_yobi((f32 *)((u8 *)w + 0x24));
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

static void em_move05_005B2F10(EMW *em) {
    em->x40C = 10;
    em->x40E = 10;
    switch (em->x15) {
    case 0: em_die00_005B24F0(em, 0); break;
    case 1: em_die00_005B24F0(em, 1); break;
    case 2: em_die01_005B2A60(em); break;
    case 3: em_die_rev_005B2B70(em); break;
    }
}
