/* em10, run 2: em_act00 .. dummy_em_prog_005AF520 (game.bin 0x005ADC50-0x005AF528). Matching functions of em10_nm.c (that file holds the
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

void em_act03_005AE060(EMW *em); /* near-match, stays asm (see em10_nm.c) */

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
