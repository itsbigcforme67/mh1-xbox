/* em10, run 1: em10_search_set .. em10_to_normal (game.bin 0x005ACC60-0x005ADB98). Matching functions of em10_nm.c (that file holds the
 * whole code including the near-matches); see it for the description. */
#include "em.h"
#include "pl.h"
#include "game.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM10W {
    u8 eff;             /* 0x00 em10_effect_move step */
    u8 _pad01[5];
    s16 char0;          /* 0x06 animation seen by ef_move_sub */
    f32 pos[3];         /* 0x08 home position */
    u16 ang;            /* 0x14 home facing */
    u8 _pad16[2];
    u8 x18;             /* 0x18 */
    u8 _pad19;
    u8 x1A;             /* 0x1A cleared each frame by talk_move */
    u8 sel;             /* 0x1B yes/no cursor (0 yes) */
} EM10W;

/* One row of talk_tbl (8 bytes). */
typedef struct EM10_TALK {
    u8 kind;            /* 0x0 what happens when the message is closed */
    u8 yes;             /* 0x1 next message for "yes", 0xFF none */
    u8 no;              /* 0x2 next message for "no", 0xFF none */
    u8 x03;             /* 0x3 */
    s32 msg;            /* 0x4 */
} EM10_TALK;

/* Start point (0x10 bytes). */
typedef struct EM10_POS {
    f32 pos[3];         /* 0x0 */
    s16 ang;            /* 0xC degrees */
    u8 act;             /* 0xE */
    u8 pose;            /* 0xF */
} EM10_POS;

typedef struct EM10_TRADE {
    u16 take;           /* 0x0 0xFFFF ends the list */
    u16 give;           /* 0x2 */
} EM10_TRADE;

typedef struct EM10_SEND {
    u16 rate;           /* 0x0 0xFFFF ends the list */
    u16 item;           /* 0x2 */
} EM10_SEND;

extern GAME_W game_w;
extern EM10_TALK talk_tbl[];
extern EM10_SEND send_tbl[];
extern u16 map2_trade_sp[];
extern EM10_TRADE map2_trade_nm[];
extern u16 map4_trade_sp[];
extern EM10_TRADE map4_trade_nm[];
extern u16 map6_trade_sp[];
extern EM10_TRADE map6_trade_nm[];
extern EM10_POS em10_start_pos05[];
extern EM10_POS em10_start_pos16[];
extern EM10_POS em10_start_pos41[];
extern s8 Snd_em_id_conv_tbl[];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
f32 flvecCalcDistance(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 (*)[4]);
void flmatGetTrans(f32 *, void *);
s16 Pl_item_num_ck(PLW *, u16);
s16 Pl_item_num_ck3(PLW *, u16);
s16 Pl_item_search_space(PLW *);
void Pl_item_stack(PLW *, u16, int);
int NPC_Message();
void set01_set(int, int, s16);
void se_req(int, int, int);
void em_act_set(EMW *, int, int);
void em_char_set(EMW *, int, int, int);
void em_dur_init(EMW *);
s16 act_ck(EMW *, int, int);
int em_frame_check(EMW *, int, f32);
void rate_clear_g(EMW *);
void rate_g_calc(EMW *, int);
void rate_add_g(EMW *);
u16 calc_vec_ang2(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 (*)[4]);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Em_se_req2_com(EMW *, int, int, f32 *, int, int);
int Code_Make(int, int, int, int);
void eft01_set(EMW *, s16);

void em10_main_sub(EMW *em);
void em10_to_normal(EMW *em, int flag, int a, int b);

int em10_search_set(EMW *em) {
    PLW *pl = &player_work[game_w.master];

    if (pl->be_flag == 0 || pl->x01 == 0) {
        return 0;
    }
    if (em->stg != pl->stg) {
        return 0;
    }
    if (Pl_stg_ck(pl) == 0) {
        return 0;
    }
    if (flvecCalcDistance(pl->pos, em->pos) <= 500.0f) {
        return 1;
    }
    return 0;
}

void em10_msg_set(EMW *em, u16 no) {
    ((PLW *)player_work)[game_w.master].x8C6 = 10;
    em->x2D6 = 0;
    em->x2D8 = no;
    em->x2DA = 0;
    ((EM10W *)em->ex)->sel = 1;
}

void em10_msg_set2(EMW *em) {
    s16 r2;
    s16 sum;
    EM10_SEND *t;
    EM10W *w = (EM10W *)em->ex;
    PLW *pl = &player_work[game_w.master];
    s16 r;

    r = (u16)ran_suu(1) % 100;

    if (r < 0x28) {
        r2 = (u16)ran_suu(1) % 101;
        sum = 0;
        t = send_tbl;
        while (t->rate != 0xFFFF) {
            sum += t->rate;
            if (sum >= r2) {
                if (Pl_item_num_ck(pl, t->item) <= 0 && Pl_item_search_space(pl) == 1) {
                    em->x6E0 = t->item;
                    break;
                }
                goto other;
            }
            t++;
        }
        em->x2D8 = 9;
        em_act_set(em, 0, 9);
    } else {
    other:
        if (r < 0x46) {
            em->x2D8 = 0xD;
        } else {
            em->x2D8 = 0xC;
        }
    }
    pl->x8C6 = 10;
    em->x2D6 = 0;
    em->x2DA = 0;
    w->sel = 1;
}

int talk_move(EMW *em) {
    u16 *sp;
    EM10_TALK *t;
    EM10_TRADE *nm;
    u8 res;
    PLW *pl;
    EM10W *w;

    pl = &player_work[game_w.master];
    t = &talk_tbl[em->x2D8];
    pl->x8C6 = 5;
    w = (EM10W *)em->ex;
    if (em->x2DA < 300) {
        em->x2DA++;
    } else {
        em->x2DA = 300;
    }
    switch (em->x2D6) {
    case 0:
        if (NPC_Message(t->msg, em->x2DA, t->x03, w->sel) == 0) {
            if ((pl->sw.trg & 0x20) && w->x1A == 0) {
                switch (t->kind) {
                case 0:
                    pl->x8C6 = 0;
                    break;
                case 3:
                    switch (game_w.x2E) {
                    default:
                    case 2:
                        sp = map2_trade_sp;
                        nm = map2_trade_nm;
                        break;
                    case 4:
                        sp = map4_trade_sp;
                        nm = map4_trade_nm;
                        break;
                    case 6:
                        sp = map6_trade_sp;
                        nm = map6_trade_nm;
                        break;
                    }
                    res = 0;
                    while (*sp != 0xFFFF) {
                        if (Pl_item_num_ck(pl, *sp) > 0) {
                            em->x6E2 = *sp;
                            em->x6E0 = 0x4D;
                            if (Pl_item_num_ck3(pl, em->x6E0) > 0) {
                                res = 1;
                                break;
                            }
                        }
                        sp++;
                    }
                    if (res == 0) {
                        while (nm->take != 0xFFFF) {
                            if (Pl_item_num_ck(pl, nm->take) > 0) {
                                em->x6E2 = nm->take;
                                em->x6E0 = nm->give;
                                if (Pl_item_num_ck3(pl, em->x6E0) > 0) {
                                    res = 2;
                                    break;
                                }
                            }
                            nm++;
                        }
                    }
                    if (res) {
                        if (res == 1) {
                            em10_msg_set(em, 6);
                        } else {
                            em10_msg_set(em, 10);
                        }
                    } else {
                        em10_msg_set2(em);
                    }
                    break;
                case 6:
                    if (pl->work8C7 >= 7) {
                        pl->x8C6 = 0;
                    } else {
                        em10_msg_set2(em);
                    }
                    break;
                case 7:
                    pl->x8C6 = 0;
                    Pl_item_stack(pl, em->x6E0, 1);
                    set01_set(1, 0xD, em->x6E0);
                    break;
                case 4:
                    if (w->sel == 0) {
                        em_act_set(em, 0, 9);
                        se_req(6, 0x21, Snd_em_id_conv_tbl[10]);
                        if (em->x2D8 == 6) {
                            em10_msg_set(em, 7);
                        } else {
                            em10_msg_set(em, 0xB);
                        }
                        Pl_item_stack(pl, em->x6E2, -1);
                        Pl_item_stack(pl, em->x6E0, 1);
                        set01_set(1, 0x10, em->x6E2);
                        set01_set(1, 0xD, em->x6E0);
                    } else {
                        se_req(6, 0x20, Snd_em_id_conv_tbl[10]);
                        em10_msg_set(em, 8);
                    }
                    break;
                case 5:
                    if (w->sel == 0) {
                        se_req(6, 0x21, Snd_em_id_conv_tbl[10]);
                        em10_msg_set2(em);
                    } else {
                        se_req(6, 0x20, Snd_em_id_conv_tbl[10]);
                        pl->x8C6 = 0;
                    }
                    break;
                case 1:
                    if (w->sel == 0) {
                        se_req(6, 0x21, Snd_em_id_conv_tbl[10]);
                        if (t->yes != 0xFF) {
                            em_act_set(em, 0, 9);
                            em10_msg_set(em, t->yes);
                        } else {
                            pl->x8C6 = 0;
                        }
                    } else {
                        se_req(6, 0x20, Snd_em_id_conv_tbl[10]);
                        if (t->no != 0xFF) {
                            em10_msg_set(em, t->no);
                        } else {
                            pl->x8C6 = 0;
                        }
                    }
                    break;
                case 2:
                    if (t->yes != 0xFF) {
                        em10_msg_set(em, t->yes);
                    } else {
                        pl->x8C6 = 0;
                    }
                    break;
                case 8:
                    em10_msg_set(em, (u16)ran_suu(1) % 18 + 0xE);
                    break;
                case 9:
                    em10_msg_set(em, (u16)ran_suu(1) % 22 + 0x20);
                    break;
                }
            } else {
                switch (t->kind) {
                case 1:
                case 4:
                case 5:
                    if ((pl->sw.trg & 0x800) && w->sel != 0) {
                        w->sel = 0;
                        se_req(7, 0x16, 0);
                    }
                    if ((pl->sw.trg & 0x400) && w->sel == 0) {
                        w->sel = 1;
                        se_req(7, 0x16, 0);
                    }
                    break;
                }
            }
        } else if ((pl->sw.trg & 0x60) && em->x2DA > 10) {
            em->x2DA = 300;
        }
        break;
    }
    w->x1A = 0;
    return 0;
}

void em10_init(EMW *em) {
    u16 n;
    EM10W *w = (EM10W *)em->ex;

    em->x40C = 0x63;
    em->x40E = 0x63;
    em->x2D4 = 0;
    em->x6E0 = 0;
    em->x6E2 = 0;
    n = (u16)ran_suu(1) & 3;
    switch (game_w.stage) {
    case 5:
        em->pos[0] = em10_start_pos05[n].pos[0];
        em->pos[1] = em10_start_pos05[n].pos[1];
        em->pos[2] = em10_start_pos05[n].pos[2];
        em->ang[1] = (u16)(s32)(0.5f + 65536.0f * (f32)em10_start_pos41[n].ang / 360.0f);
        em_act_set(em, 0, em10_start_pos41[n].act);
        em->x2D4 = em10_start_pos41[n].pose;
        break;
    case 16:
        em->pos[0] = em10_start_pos16[n].pos[0];
        em->pos[1] = em10_start_pos16[n].pos[1];
        em->pos[2] = em10_start_pos16[n].pos[2];
        em->ang[1] = (u16)(s32)(0.5f + 65536.0f * (f32)em10_start_pos41[n].ang / 360.0f);
        em_act_set(em, 0, em10_start_pos41[n].act);
        em->x2D4 = em10_start_pos41[n].pose;
        break;
    case 41:
        em->pos[0] = em10_start_pos41[n].pos[0];
        em->pos[1] = em10_start_pos41[n].pos[1];
        em->pos[2] = em10_start_pos41[n].pos[2];
        em->ang[1] = (u16)(s32)(0.5f + 65536.0f * (f32)em10_start_pos41[n].ang / 360.0f);
        em_act_set(em, 0, em10_start_pos41[n].act);
        em->x2D4 = em10_start_pos41[n].pose;
        break;
    default:
        em->pos[0] = 3600.0f - 80.0f * em->x13;
        em->pos[1] = 0.0f;
        em->pos[2] = 3200.0f - 80.0f * em->x13;
        em->ang[1] = 0;
        em_act_set(em, 0, 0);
        break;
    }
    switch (em->x2D4) {
    case 0:
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
    case 2:
        em_char_set(em, 1, 0, 0);
        break;
    }
    em->home[0] = em->pos[0];
    em->home[1] = em->pos[1];
    em->home[2] = em->pos[2];
    em->x0E = em->ang[1];
    em->x388 = 0;
    em->work08 = 0;
    if (n == 0xFF) {
        em_act_set(em, 0, 1);
        em_char_set(em, 2, 0, 0);
    }
    em->x302 = 200;
    em->x792 = 200;
    em->scale[0] = 1.0f;
    em->scale[1] = 1.0f;
    em->scale[2] = 1.0f;
    em->x839 = 0;
    em->x88B = 1;
    w->pos[0] = em->pos[0];
    w->pos[1] = em->pos[1];
    w->pos[2] = em->pos[2];
    w->ang = em->ang[1];
    w->x18 = 0;
    w->x1A = 0;
    em_dur_init(em);
    em->x734 = 0;
}

void em10_to_normal(EMW *em, int flag, int a, int b) {
    EM10W *w = (EM10W *)em->ex;

    switch ((u8)flag) {
    case 0:
        w->x1A = 0;
        switch (em->x2D4) {
        case 0:
            em_act_set(em, 0, 0);
            if (em->char0 != 0x3E9) {
                em_char_set(em, 1, (u16)a, (u16)b);
            }
            break;
        case 1:
            em_act_set(em, 0, 0xA);
            if (em->char0 != 0x3F5) {
                em_char_set(em, 0xD, (u16)a, (u16)b);
            }
            break;
        case 2:
            em_act_set(em, 0, 6);
            if (em->char0 != 0x3ED) {
                em_char_set(em, 5, (u16)a, (u16)b);
            }
            break;
        }
        break;
    }
    em->ex[0x90] = 1;
}
