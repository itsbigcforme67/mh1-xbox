/* em10, run 3: em_act04 .. dummy_em_prog_005AF520 (game.bin 0x005AE280-0x005AF528). Matching functions of em10_nm.c (that file holds the
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
