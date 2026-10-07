/* em10, one translation unit: game.bin 0x005ACC60-0x005AF528 (em10_turn_sub stays original bytes, see config/c_rawfuncs.txt). Matching functions of em10_nm.c (that file holds the
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

static void em10_msg_set(EMW *em, u16 no) {
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

/* original bytes: build/raw/em10_turn_sub.inc (config/c_rawfuncs.txt); the near-match C is in em10_nm.c, used by the PC build */
#ifdef __MWERKS__
asm void em10_turn_sub(EMW *em)
{
#include "em10_turn_sub.inc"
}
#endif

void em_act00(EMW *em) {
    EM10W *w = (EM10W *)em->ex;
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = (u16)ran_suu(1) & 0xBB;
        if (em->x2D4 == 2) {
            if (em->char0 != 0x3ED) {
                em_char_set(em, 5, 2, 0);
            }
        } else if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 2, 0);
        }
        w->x1A = 0;
        break;
    case 1:
        if (em10_search_set(em) == 1) {
            if (pl->x8C6 != 0) {
                em_act_set(em, 0, 3);
            }
        } else {
            em->work08--;
            if (em->work08 > 0) {
                return;
            }
            switch (em->x2D4) {
            case 0:
                break;
            case 1:
                if ((u16)ran_suu(1) & 3) {
                    em_act_set(em, 0, 0xA);
                } else {
                    em_act_set(em, 0, 0xB);
                }
                break;
            }
        }
        break;
    }
}

void em_act01(EMW *em) {
    EM10W *w = (EM10W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->ex[0x90] = 0;
        if (em->char0 != 0x3EA) {
            em->pos[0] = w->pos[0];
            em->pos[1] = w->pos[1];
            em->pos[2] = w->pos[2];
            em->ang[1] = w->ang;
            em->x0E = w->ang;
            em_char_set(em, 2, 2, 0);
        }
        w->x1A = 0;
        break;
    case 1:
        if (em10_search_set(em) == 1) {
            em->ex[0x90] = 1;
            em_act_set(em, 0, 2);
        }
        break;
    }
}

void em_act02(EMW *em) {
    f32 d[3];
    f32 v[3];
    EM10W *w = (EM10W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 3, 0, 0);
        em->x388 = 2;
        w->x1A = 0;
        break;
    case 1:
        if (em_frame_check(em, 0, 14.0f) != 0) {
            em->x05++;
            d[0] = 0.0f;
            d[1] = 0.0f;
            d[2] = 4.0f;
            flvecApplyMat33(v, d, (f32 (*)[4])((u8 *)em + 0x60));
            em->rate_x = v[0];
            em->adj_y = 6.0f;
            em->adj_z = v[2];
            rate_clear_g(em);
            rate_g_calc(em, 0xC);
        }
        break;
    case 2:
        rate_add_g(em);
        if (em->adj_y <= 0.0f && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 4, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em_char_set(em, 1, 6, 0);
            em_act_set(em, 0, 0);
        }
        break;
    }
}

static void em_act03_005AE060(EMW *em) {
    EM10W *w = (EM10W *)em->ex;
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 7, 8, 0);
        w->x18 = 1;
        w->x1A = 0;
        em->x0E = calc_vec_ang2(em->pos, pl->pos) - 0x4000;
        se_req(6, 0x1E, Snd_em_id_conv_tbl[10]);
        if (pl->work8C7 >= 7) {
            switch (em->x39A % 3) {
            case 0:
                em10_msg_set(em, 3);
                em_act_set(em, 0, 5);
                break;
            case 1:
                em10_msg_set(em, 4);
                em_act_set(em, 0, 7);
                break;
            case 2:
                em10_msg_set(em, 5);
                em_act_set(em, 0, 8);
                break;
            }
        } else {
            switch (em->x2D4) {
            case 0:
                em10_msg_set(em, 0);
                break;
            case 1:
                em10_msg_set(em, 2);
                break;
            case 2:
                em10_msg_set(em, 1);
                break;
            }
        }
        break;
    case 1:
        w->x1A = 0;
        talk_move(em);
        if (pl->x8C6 == 0) {
            em10_to_normal(em, 0, 4, 0);
        } else if (em->x194 == 0) {
            em_act_set(em, 0, 4);
        }
        break;
    }
}

void em_act04(EMW *em) {
    PLW *pl = &player_work[game_w.master];

    ((EM10W *)em->ex)->x1A = 0;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x06 = 0;
        em->x2D6 = 0;
        em_char_set(em, 8, 4, 0);
        talk_move(em);
    case 1:
        talk_move(em);
        if (pl->x8C6 == 0) {
            em10_to_normal(em, 0, 4, 0);
        }
        break;
    }
}

void em_act05(EMW *em, int type) {
    EM10W *w = (EM10W *)em->ex;
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        switch (type) {
        case 0:
            em_char_set(em, 0xB, 6, 0);
            break;
        case 1:
            em_char_set(em, 9, 4, 0);
            break;
        case 2:
            em_char_set(em, 0xA, 4, 0);
            break;
        }
        w->x1A = 1;
        talk_move(em);
        break;
    case 1:
        w->x1A = 1;
        talk_move(em);
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 2, 0);
        }
        break;
    case 2:
        w->x1A = 0;
        talk_move(em);
        if (pl->x8C6 == 0) {
            em10_to_normal(em, 0, 2, 0);
        }
        break;
    }
}

void em_act06(EMW *em) {
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 5, 4, 0);
        em->work08 = (u16)ran_suu(1) & 0xBF;
        break;
    case 1:
        if (--em->work08 <= 0) {
            if ((u16)ran_suu(1) & 1) {
                em->x05 = 2;
                em->work08 = (u16)ran_suu(1) & 0xBF;
                em_char_set(em, 1, 4, 0);
            } else {
                em->x05 = 3;
                em_char_set(em, 6, 4, 0);
            }
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05 = 0;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = 0;
        }
        break;
    }
    if (em10_search_set(em) == 1 && pl->x8C6 != 0) {
        em_act_set(em, 0, 3);
    }
}

void em_act09(EMW *em) {
    EM10W *w = (EM10W *)em->ex;
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xC, 6, 0);
        w->x1A = 1;
        talk_move(em);
        break;
    case 1:
        w->x1A = 1;
        talk_move(em);
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 2, 0);
        }
        break;
    case 2:
        w->x1A = 0;
        talk_move(em);
        if (pl->x8C6 == 0) {
            em10_to_normal(em, 0, 2, 0);
        }
        break;
    }
}

void em_act10(EMW *em) {
    EM10W *w = (EM10W *)em->ex;
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((u16)ran_suu(1) & 0x7F) + 0x78;
        if (em->char0 != 0x3F5) {
            em_char_set(em, 0xD, 6, 0);
        }
        break;
    case 1:
        if (em->work08 > 0) {
            em->work08--;
        }
        if (em->work08 <= 0) {
            em_act_set(em, 0, 0xB);
        } else {
            em->horm_ang = Em_Calc_angY(em->pos, w->pos);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x80);
            em->x0E = em->ang[1];
            cpRotMatrix(em->ang, em->mat);
        }
        if (em10_search_set(em) == 1 && pl->x8C6 != 0) {
            em_act_set(em, 0, 3);
        }
        break;
    }
}

void em_act11(EMW *em) {
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0;
        em_char_set(em, 0xE, 4, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em10_to_normal(em, 0, 4, 0);
        }
        if (em10_search_set(em) == 1 && pl->x8C6 != 0) {
            em_act_set(em, 0, 3);
        }
        break;
    }
}

void em_move00_005AE9A0(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0:
        em_act00(em);
        break;
    case 1:
        em_act01(em);
        break;
    case 2:
        em_act02(em);
        break;
    case 3:
        em_act03_005AE060(em);
        break;
    case 4:
        em_act04(em);
        break;
    case 5:
        em_act05(em, 0);
        break;
    case 6:
        em_act06(em);
        break;
    case 7:
        em_act05(em, 1);
        break;
    case 8:
        em_act05(em, 2);
        break;
    case 9:
        em_act09(em);
        break;
    case 10:
        em_act10(em);
        break;
    case 11:
        em_act11(em);
        break;
    }
}

void em10_main(EMW *em) {
    em->x40C = 0x63;
    em->x40E = 0x63;
    em10_main_sub(em);
    if (em->x6FF != 0) {
        em10_main_sub(em);
        em->x6FF = 0;
    }
    em10_turn_sub(em);
}

void em10_main_sub(EMW *em) {
    switch (em->mode) {
    case 0:
        em_move00_005AE9A0(em);
        break;
    case 1:
        em_move00_005AE9A0(em);
        break;
    case 2:
        em_move00_005AE9A0(em);
        break;
    case 3:
        em_move00_005AE9A0(em);
        break;
    case 4:
        em_move00_005AE9A0(em);
        break;
    case 5:
        em_move00_005AE9A0(em);
        break;
    case 6:
        em_move00_005AE9A0(em);
        break;
    case 7:
        em_move00_005AE9A0(em);
        break;
    }
}

void sound_call(EMW *em, int frame, int code, int joint) {
    f32 p[3];

    if (em_frame_check(em, 0, frame) != 0) {
        flmatGetTrans(p, em->mdl->bone + joint * 400);
        Em_se_req2(em, code, 0, p, 6, 0);
    }
}

void sound_call_com(EMW *em, int frame, int code, int joint) {
    f32 p[3];

    if (em_frame_check(em, 0, frame) != 0) {
        flmatGetTrans(p, em->mdl->bone + joint * 400);
        Em_se_req2_com(em, code, 0, p, 6, 0);
    }
}

void ef_move_sub(EMW *em, EM10W *w) {
    if (em->char0 != w->char0) {
        w->char0 = em->char0;
    }
    switch (w->char0) {
    case 0x3EA:
        sound_call(em, 0x2A, 0x1A, 0);
        break;
    case 0x3EB:
        sound_call(em, 0x16, 0xA, 0);
        sound_call(em, 0x1A, 0x1B, 0);
        sound_call_com(em, 6, 0x43, 0);
        break;
    case 0x3EC:
        sound_call(em, 4, 4, 0);
        sound_call(em, 4, 0x16, 0);
        sound_call_com(em, 6, 0x3A, 0);
        break;
    case 0x3ED:
        sound_call(em, 0xB0, 7, 0);
        sound_call(em, 0x1C, 0x1A, 0);
        sound_call(em, 0xD4, 0x1A, 0);
        break;
    case 0x3EE:
        sound_call(em, 4, 8, 0);
        sound_call(em, 0xB2, 9, 0);
        sound_call(em, 0x5C, 0x1C, 0);
        sound_call(em, 0xCE, 0x1C, 0);
        break;
    case 0x3EF:
        sound_call(em, 0xA, 6, 0);
        sound_call(em, 0x1A, 0x1A, 0);
        break;
    case 0x3F0:
        sound_call(em, 0x2A, 0xB, 0);
        sound_call(em, 0xEE, 0xD, 0);
        sound_call(em, 0x15E, 7, 0);
        sound_call(em, 0x44, 0x1A, 0);
        sound_call(em, 0xB2, 0x1A, 0);
        sound_call(em, 0x11E, 0x1C, 0);
        sound_call(em, 0x4E, 0x19, 0);
        break;
    case 0x3F1:
        sound_call(em, 0x1E, 5, 0);
        sound_call(em, 0x16, 0x16, 0);
        sound_call(em, 0xA4, 0x17, 0);
        sound_call_com(em, 0x18, 0x3D, 0);
        sound_call(em, 0x1E, 0x1B, 0);
        sound_call(em, 0x7A, 0x1C, 0);
        break;
    case 0x3F2:
        sound_call(em, 0x12, 3, 0);
        sound_call(em, 0x32, 0xA, 0);
        sound_call(em, 0x62, 0xD, 0);
        sound_call(em, 0x18, 0x17, 0);
        sound_call(em, 0x74, 0x17, 0);
        sound_call(em, 0xA4, 0x16, 0);
        sound_call(em, 0x24, 0x1B, 0);
        sound_call(em, 0x7E, 0x1C, 0);
        break;
    case 0x3F3:
        sound_call(em, 0x16, 0, 0);
        sound_call(em, 0x3C, 1, 0);
        sound_call(em, 0x72, 2, 0);
        sound_call(em, 0xB0, 3, 0);
        sound_call(em, 0x14, 0x17, 0);
        sound_call(em, 0x1A, 0x1B, 0);
        sound_call(em, 0x4C, 0x1B, 0);
        sound_call(em, 0xA0, 0x1B, 0);
        sound_call(em, 0x20, 0x16, 0);
        sound_call(em, 0xEA, 0x16, 0);
        sound_call_com(em, 0x3C, 0x3D, 0);
        sound_call_com(em, 0x72, 0x4A, 0);
        sound_call_com(em, 0x88, 0x4A, 0);
        sound_call_com(em, 0xB0, 0x4A, 0);
        break;
    case 0x3F4:
        sound_call(em, 4, 0xC, 0);
        sound_call(em, 0x7E, 4, 0);
        sound_call(em, 0x1E, 0x1C, 0);
        sound_call(em, 0xA0, 0x1B, 0);
        sound_call_com(em, 0x50, 0x4A, 0);
        break;
    case 0x3F5:
        sound_call(em, 0x26, Code_Make(0xE, 4, 0xF, 2), 0);
        sound_call(em, 0x6C, Code_Make(0x11, 2, 0xE, 4), 0);
        sound_call(em, 0xC, 0x14, 0);
        sound_call(em, 0x2C, 0x15, 0);
        sound_call(em, 0x4C, 0x14, 0);
        sound_call(em, 0x6A, 0x15, 0);
        sound_call(em, 0x14, 0x1A, 0);
        sound_call(em, 0x50, 0x1A, 0);
        break;
    case 0x3F6:
        sound_call(em, 0x18, Code_Make(0xE, 4, 0x11, 4), 0);
        sound_call(em, 0xB4, Code_Make(0x10, 3, 0x11, 3), 0);
        sound_call(em, 0x12C, Code_Make(0xE, 3, 0xF, 3), 0);
        sound_call(em, 0x174, Code_Make(0x11, 3, 0x10, 3), 0);
        sound_call(em, 0x1DC, Code_Make(0xE, 3, 0xF, 3), 0);
        sound_call(em, 0x258, Code_Make(0xE, 3, 0x10, 3), 0);
        sound_call(em, 0x22, 0x1C, 0);
        sound_call(em, 0xFE, 0x1A, 0);
        sound_call(em, 0x1BE, 0x1A, 0);
        sound_call_com(em, 0x7E, 0x4C, 0);
        sound_call_com(em, 0xB0, 0x4C, 0);
        sound_call_com(em, 0xDC, 0x4C, 0);
        sound_call(em, 0x1A4, 0x19, 0);
        sound_call(em, 0x1B8, 0x19, 0);
        sound_call(em, 0x80, 0x14, 0);
        sound_call(em, 0x122, 0x15, 0);
        break;
    }
}

/* File statics in the original, named by address because data outside
 * this file (the monster program tables) points at them. */
void em10_effect_move_005AF4C0(EMW *em) {
    EM10W *w = (EM10W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub(em, w);
        break;
    }
}

void em10_local_init_005AF510(EMW *em) {
    eft01_set(em, 0);
}

void dummy_em_prog_005AF520(void) {
}
