/* em09 - game.bin 0x005A81B0-0x005AC9xx. Per-monster AI for monster kind 9
 * (the item thief: it runs up to a player, steals an item from the pouch
 * (item_theft) and gives it back when killed (item_return)). Names of the
 * steps follow the split (em_act00, em_move00...). Field meanings are
 * mostly guesses. */
#include "em.h"
#include "game.h"
#include "pl.h"
#include "plf.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM09W {
    u8 _pad00[0xE];
    u16 chase;          /* 0x0E non-zero: chasing a target */
    u8 _pad10[4];
    f32 home[3];        /* 0x14 home position */
    u8 _pad20[4];
    PLW *pl;            /* 0x24 player the item was stolen from */
    u8 _pad28[0x46 - 0x28];
    s16 item;           /* 0x46 stolen item, -1 none */
    s16 item_num;       /* 0x48 */
} EM09W;

extern GAME_W game_w;
extern f32 em09_rev_set_tbl_st53[][6];
extern s32 em09_rev_set_tbl_st53A[];
extern u8 em09_act_tbl[];

void em_act_set(EMW *, int, u16);
u16 em_act_search(void *);
void target_kind_set(EMW *, f32 *);
void em_char_set(EMW *, int, int, int);
int Quest_clear_ck();
s16 Pl_item_num_ck(PLW *, int);

void oikake_ck(EMW *em) {
    if (em->work08 > 15) {
        s8 n = em->x617;

        if (n == -1) {
            em->work08 = 15;
        } else if (em->stg != ((PLW *)player_work)[n].stg) {
            em->work08 = 15;
        }
    }
}

void em09_act_set(EMW *em, int kind, u16 no) {
    EM09W *w = (EM09W *)em->ex;
    f32 *p = (f32 *)kind;

    w->chase = 0;
    switch ((u16)kind) {
    case 0:
        em->x388 = 0;
        break;
    case 1:
        em->x388 = 0;
        if (no != 2) {
            if (no != 6 && no != 5) {
                w->chase = 1;
            }
            target_kind_set(em, em->tgt_pos);
        } else {
            u32 k = em->type & 0xF;

            em->work08 = 600;
            if (em->stg == 0x35) {
                p = em09_rev_set_tbl_st53[(u8)k];
                em->ang[1] = em09_rev_set_tbl_st53A[(u8)k];
            }
            em->pos[0] = p[0];
            em->pos[1] = p[1];
            p += 2;
            em->pos[2] = p[0];
            em->tgt_pos[0] = p[1];
            p += 2;
            em->tgt_pos[1] = p[0];
            em->tgt_pos[2] = p[1];
        }
        break;
    case 3:
        em->x388 = 0;
        break;
    case 4:
        em->x388 = 0;
        if (no == 1) {
        }
        break;
    }
    em_act_set(em, kind, no);
}

void em09_next_act_set(EMW *em) {
    em->act_spd = 1.0f;
    if (em->x734 == 3) {
        em->x839 = 1;
        em_act_set(em, 0, 1);
    } else {
        em_act_set(em, 0, em_act_search(em09_act_tbl));
    }
    em->mode_old = em->mode;
    em->x15_old = em->x15;
}

int em09_status_ck(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    if (em->x388 != 0 || (u32)(em->mode - 4) < 3 || em->be_flag == 0) {
        return -1;
    }
    if (em->x19 != 0 && em->x7A4->kind == 2) {
        return -2;
    }
    if (w->chase == 0) {
        return 0;
    }
    return 1;
}

int em09_dir_calc(s32 *ang, s32 *tgt, int spd) {
    u32 d, a;

    *tgt = (u16)*tgt;
    d = (u16)(*tgt - *ang);
    a = d;
    if (d >= 0x8000) {
        a = (u16)-d;
    }
    if (a < spd) {
        spd = a;
    }
    if (d >= 0x8000) {
        *ang -= spd;
    } else {
        *ang += spd;
    }
    return spd;
}

static int item_theft_005A8540(EMW *em, PLW *pl, EM09W *w) {
    int list[20];
    int n;
    int i;


    if (pl->id != game_w.master) {
        return 0;
    }
    if (Quest_clear_ck(1)) {
        return 0;
    }
    if (Pl_Skill_ck(pl, 0x2B) == 1) {
        return 0;
    }
    if (Pl_item_num_ck(pl, 0x47) > 0) {
        w->item = 0x47;
        w->item_num = -1;
    } else {
        i = 0;
        n = 0;
        for (; i < 20; i++) {
            u16 id = pl->item[i].id;

            if (id != 0) {
                if (Item_data[id][0] == 0 && Item_data[id][2] < 4) {
                    list[n++] = i;
                }
            }
        }
        if (n == 0) {
            return 0;
        }
        w->item = pl->item[list[(u16)ran_suu(1) % n]].id;
        w->item_num = -1;
    }
    w->pl = pl;
    Pl_item_stack(pl, (u16)w->item, w->item_num);
    set01_set(1, 6, w->item);
    return 1;
}

static void item_return(EMW *em, EM09W *w) {
    if (w->item != -1) {
        switch ((u16)Pl_item_stack(w->pl, (u16)w->item, (s16)-w->item_num)) {
        case 0:
        case 1:
        case 2:
            if (Pl_master_ck(w->pl) == 1) {
                set01_set(1, 7, w->item);
            }
            break;
        case 3:
            if (Pl_master_ck(w->pl) == 1) {
                set01_set(1, 3, w->item);
            }
            break;
        default:
            if (Pl_master_ck(w->pl) == 1) {
                set01_set(0, 12, 0);
            }
            break;
        }
    }
}
