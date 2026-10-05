/* em09_init - game.bin 0x005A7F70-0x005A81A4: monster 9 setup and
 * Em09_item_sub (gives back a stolen item through Item_stolen). Guesses. */
#include "em.h"
#include "game.h"

/* quest_w (0x3C7440): only the quest number is used here. */
typedef struct QUEST_W {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest (0: free play?) */
} QUEST_W;

/* Part of the per-monster work at EMW+0x444 set up by the init code. */
typedef struct EM_INITW {
    u8 _pad00[0xC];
    s16 x0C;            /* 0x0C */
    s16 x0E;            /* 0x0E */
    u8 _pad10[4];
    f32 home[3];        /* 0x14 home position (x, 0, z) */
    u8 _pad20[4];
    s32 x24;            /* 0x24 item kind (Em09_item_sub) */
    u8 _pad28[0x46 - 0x28];
    s16 item;           /* 0x46 stolen item, -1 none */
    s16 item_num;       /* 0x48 */
    u8 _pad4A;
    u8 x4B;             /* 0x4B */
    u8 _pad4C[0x52 - 0x4C];
    s16 hp;             /* 0x52 hit points at start (em02) */
} EM_INITW;

extern QUEST_W quest_w;

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);

void Item_stolen(s32, u16, s16);

void em09_init(EMW *em) {
    EM_INITW *w = (EM_INITW *)em->ex;

    if (quest_w.no == 0) {
        if (game_w.stage == 15) {
            em->pos[0] = 9500.0f + 150.0f * (int)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9200.0f + 150.0f * (em->x13 & 7);
        } else {
            em->pos[0] = 2400.0f + 150.0f * em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 3600.0f;
        }
    }
    em->x88B = 1;
    em->x8C3 = 0;
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    if (game_w.stage == 0x37) {
        if (em->type != 0) {
            em->x40C = 10;
            em->x40E = 10;
        }
        em_act_set(em, 0, 1);
    } else {
        em_act_set(em, 1, 8);
    }
    em->x792 = em->x302 = em_hp_vital_set(em, 0x20);
    w->x0E = 0;
    w->x0C = 0;
    w->home[0] = em->pos[0];
    w->home[1] = 0.0f;
    w->home[2] = em->pos[2];
    w->x4B = 0;
    w->item = -1;
    em->x56A = 0;
    if (em->type & 0x80) {
        em->scale[2] = 10.0f;
        em->scale[1] = 10.0f;
        em->scale[0] = 10.0f;
    }
    em_dur_init(em);
    em->x734 = 3;
}

void Em09_item_sub(EMW *em) {
    EM_INITW *w = (EM_INITW *)em->ex;

    if (w->item != -1) {
        Item_stolen(w->x24, w->item, w->item_num);
    }
}
