/* em18 - game.bin 0x005E6E00-0x005E7918. Monster kind 18: a small
 * creature that idles, turns in place, follows a player (while that player
 * is in mode 4), spins and fades out. Its action program comes from the
 * shared em code (em_cmd_ck / em_act_search over em18_act_tbl); animations
 * 0x3E9-0x3EB. ef_move_sub plays the walk sounds and dust (Eft20). Names of
 * the action steps follow the split (em_act00, em_mov00...). */
#include "em.h"
#include "pl.h"
#include "game.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM18W {
    u8 eff;             /* 0x00 em18_effect_move step */
    u8 _pad01[5];
    s16 char0;          /* 0x06 animation seen by ef_move_sub */
    u8 _pad08[4];
    u16 x0C;            /* 0x0C counts down each frame */
} EM18W;

extern void *em18_act_tbl;
extern GAME_W game_w;

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, int);
u16 em_act_search(void *, int);
void em_cmd_ck(EMW *);
void cpRotMatrix(s32 *, f32 (*)[4]);
int em_frame_check(EMW *, int, f32);
int em_frame_check2(EMW *, int, f32);
int Code_Make(int, int, int, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Eft13_set_em(EMW *, int, int);
void Eft20_set(EMW *, int, int, f32);

void em18_main_sub(EMW *em);

static void em_act00(EMW *em) {
}

static void em_act01(EMW *em) {
    u16 no;

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3E9) {
            em->work08 = 200;
        }
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        if (--em->work08 < 0) {
            if (em->x734 == 3) {
                em->x839 = 1;
                return;
            }
            no = em_act_search(&em18_act_tbl, 1);
            if (no > 1) {
                em_act_set(em, 0, no);
            }
        }
        break;
    }
}

static void em_act02(EMW *em) {
    s32 t;

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3EB) {
            em_char_set(em, 3, 0, 0);
        }
        break;
    case 1:
        t = em->x194;
        if (t == 0) {
            if (em->x734 == 3) {
                if (t == 0) {
                    em->x839 = 1;
                    return;
                }
            } else {
                em_act_set(em, 1, 0);
            }
        }
        break;
    }
}

static void em_move00(EMW *em) {
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
    }
}

static void em_mov00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3EA) {
            em->work08 = 600;
            em_char_set(em, 2, 0, 0);
        }
        break;
    case 1:
        em->ang[1] += 0x70;
        em->ang[1] = (u16)em->ang[1];
        cpRotMatrix(em->ang, em->mat);
        if (--em->work08 <= 0) {
            if (em->x734 == 3) {
                em->x839 = 1;
                return;
            }
            em_act_set(em, 1, 0);
        }
        break;
    }
}

static void em_mov01(EMW *em) {
    if (*(u8 *)&((PLW *)player_work)[em->x616].flag14 != 4) {
        em->x04++;
        em->x01 = 0;
        return;
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3EA) {
            em->work08 = 0x5A;
            em_char_set(em, 2, 0, 0);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            if (em->x734 == 3) {
                em->x839 = 1;
                return;
            }
            em_act_set(em, 0, 2);
        }
        break;
    }
}

static void em_mov02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x35;
        if (em->char0 != 0x3EA) {
            em_char_set(em, 2, 0, 0);
        }
        break;
    case 1:
        switch (game_w.stage) {
        case 0xA:
        case 0x14:
            em->ang[1] += 0x2A0;
            break;
        default:
            em->ang[1] += 0x2B0;
            break;
        }
        em->ang[1] = (u16)em->ang[1];
        cpRotMatrix(em->ang, em->mat);
        if (--em->work08 <= 0) {
            if (em->x734 == 3) {
                em->x839 = 1;
                return;
            }
            em_act_set(em, 1, 1);
        }
        break;
    }
}

static void em_mov03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 100;
        if (em->char0 != 0x3EA) {
            em_char_set(em, 2, 0, 0);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
            em->work08 = 0;
        }
        if (em->work08 < 5) {
            em->x798 = (f32)em->work08 / 5.0f;
        }
        break;
    }
}

static void em_move01(EMW *em) {
    switch (em->x15) {
    case 0:
        em_mov00(em);
        break;
    case 1:
        em_mov01(em);
        break;
    case 2:
        em_mov02(em);
        break;
    case 3:
        em_mov03(em);
        break;
    }
}

static void em_move02(EMW *em) {
}

static void em_atk00(EMW *em) {
}

static void em_move03(EMW *em) {
    switch (em->x15) {
    case 0:
        em_atk00(em);
        break;
    }
}

static void em_dm00(EMW *em) {
}

static void em_move04(EMW *em) {
    switch (em->x15) {
    case 0:
        em_dm00(em);
        break;
    }
}

void em18_main(EMW *em) {
    EM18W *w = (EM18W *)em->ex;

    if (w->x0C != 0) {
        w->x0C--;
    }
    em->x9E1 = 5;
    switch (em->x734) {
    case 3:
        if (em->x839 != 0) {
            em_cmd_ck(em);
            em->x839 = 0;
        }
        break;
    }
    em18_main_sub(em);
    if (em->x6FF != 0) {
        em18_main_sub(em);
        em->x6FF = 0;
    }
}

void em18_main_sub(EMW *em) {
    switch (em->mode) {
    case 0:
        em_move00(em);
        break;
    case 1:
        em_move01(em);
        break;
    case 2:
        em_move02(em);
        break;
    case 3:
        em_move03(em);
        break;
    case 4:
        em_move04(em);
        break;
    case 5:
        em_move04(em);
        break;
    case 6:
        em_move04(em);
        break;
    case 7:
        em_move04(em);
        break;
    }
}

static void sound_call(EMW *em, int frame, int code, int x) {
    if (em_frame_check(em, 0, frame) != 0) {
        Em_se_req2(em, code, 0, em->pos, 6, 0);
    }
}

static void ef_move_sub(EMW *em, EM18W *w) {
    if (em->char0 != w->char0) {
        w->char0 = em->char0;
    }
    switch (w->char0) {
    case 0x3E9:
        break;
    case 0x3EA:
        sound_call(em, 2, 0, 0);
        sound_call(em, 0x26, 0, 0);
        sound_call(em, 0x14, Code_Make(2, 4, 4, 4), 0);
        sound_call(em, 0x3A, Code_Make(3, 4, 5, 4), 0);
        if (em_frame_check2(em, 0, 6.0f) != 0 && *(u16 *)&game_w.x1E % 12 == 1) {
            Eft20_set(em, 2, 5, 0.2f);
        }
        if (em_frame_check2(em, 0, 4.0f) != 0 && *(u16 *)&game_w.x1E % 12 == 0) {
            Eft20_set(em, 3, 5, 0.2f);
        }
        break;
    case 0x3EB:
        sound_call(em, 2, 1, 0);
        if (em_frame_check(em, 0, 16.0f) != 0) {
            Eft13_set_em(em, 0x10, 7);
        }
        if (em_frame_check(em, 0, 46.0f) != 0) {
            Eft20_set(em, 0x1C, 0, 1.0f);
            Eft20_set(em, 0x1C, 1, 1.0f);
        }
        break;
    }
}

void em18_effect_move(EMW *em) {
    EM18W *w = (EM18W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub(em, w);
        break;
    }
}

/* A file static in the original; named by its address because data outside
 * this file (the monster program tables) points at it. */
void dummy_em_prog_005E7910(void) {
}
