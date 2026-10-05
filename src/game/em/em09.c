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
    u8 _pad00[4];
    s16 x04;            /* 0x04 */
    u8 _pad06[8];
    u16 chase;          /* 0x0E non-zero: chasing a target */
    u8 _pad10[4];
    f32 home[3];        /* 0x14 home position */
    u8 _pad20[4];
    PLW *pl;            /* 0x24 player the item was stolen from */
    u8 x28[0x46 - 0x28]; /* 0x28 yobi work (pull_em_yobi) */
    s16 item;           /* 0x46 stolen item, -1 none */
    s16 item_num;       /* 0x48 */
    u8 _pad4A;
    u8 x4B;             /* 0x4B */
} EM09W;

extern GAME_W game_w;
extern EMW em_work[];
extern f32 em09_rev_set_tbl_st53[][6];
extern s32 em09_rev_set_tbl_st53A[];
extern u8 em09_act_tbl[];

void em_act_set(EMW *, int, u16);
u16 em_act_search(void *);
void target_kind_set(EMW *, f32 *);
void em_char_set(EMW *, int, int, int);
int Quest_clear_ck();
s16 Pl_item_num_ck(PLW *, int);
void em_char_set(EMW *, int, int, int);
void em09_next_act_set(EMW *);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void Eft24_set_em(EMW *, int, int, int, f32, f32);
void shell14_set(EMW *, int);
int em_frame_check(EMW *, f32, int);
void Quest_enemy_escape(EMW *);
void shell02_set(EMW *, int);
void em_rate_add_g(EMW *);
void flvecRotY();
void pull_em_yobi(void *);
void Item_stolen(s32, u16, s16);
u16 Em_Calc_angY(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
f32 CalcDistanceXZ(f32 *, f32 *);
int em09_dir_calc(s32 *, s32 *, int);

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

    if (em->x388 != 0 || (u8)(em->mode - 4) < 3 || em->be_flag == 0) {
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
    int a;
    int d;

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

static void em_act00_005A87D0(EMW *em, int unused) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        em->work08 = (u16)ran_suu(1) & 0xF;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x839 = 1;
        }
        break;
    }
}

static void em_act01_005A8860(EMW *em, int unused) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act02_005A88E0(EMW *em, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        switch (n) {
        case 0: em_char_set(em, 2, 0, 0); break;
        case 1: em_char_set(em, 3, 0, 0); break;
        case 2: em_char_set(em, 4, 0, 0); break;
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act05_005A89B0(EMW *em, int unused) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 5, 0, 0);
        em->work08 = 0x78;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act06_005A8A30(EMW *em, int unused) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1F, 0, 0);
        Eft24_set_em(em, 1, 2, 0x3C, 30.0f, 1.0f);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3F, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act07_005A8B10(EMW *em, int unused) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x25, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act08_005A8B80(EMW *em, int unused) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((u16)ran_suu(1) & 0x3F) + 0x3C;
        em_char_set(em, 0x26, 0, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act09_005A8C20(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x96;
        em_char_set(em, 0x20, 0, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x22, 4, 4);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act10_005A8CE0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x78;
        em_char_set(em, 0x23, 0, 0);
        em->x88B = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act11_005A8D70(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x14;
        em_char_set(em, 0x24, 0, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em->work08 = 0x6E;
            w->chase = 1;
            shell14_set(em, 0);
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act12_005A8E30(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x43, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        v[2] = 10.0f;
        v[0] = 0.0f;
        v[1] = 0.0f;
        em_sleep_eff_set(em, 10, v, 0.6f);
        if (--em->work08 <= 0) {
            em->x05++;
            em_act_set(em, 0, 0xD);
        }
        break;
    }
}

static void em_act13_005A8F00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x88B = 1;
        Em_Sleep_End(em);
        em_char_set(em, 0x3F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_act14_005A8F90(EMW *em) {
    EM09W *w;
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 144.0f, 0) != 0) {
            em->x05++;
        }
        break;
    case 2:
        w = (EM09W *)em->ex;
        em->x40C = 10;
        em->x40E = 10;
        if (em->x194 == 0) {
            Quest_enemy_escape(em);
            em->x04++;
            em->x01 = 0;
            if (w->x4B == 1) {
                pull_em_yobi(&w->x28);
            }
            if (w->item != -1) {
                Item_stolen((s32)w->pl, (u16)w->item, w->item_num);
            }
        }
        break;
    }
}

static void em_move00_005A90A0(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_act00_005A87D0(em, 0); break;
    case 1: em_act01_005A8860(em, 0); break;
    case 2: em_act02_005A88E0(em, 0); break;
    case 3: em_act02_005A88E0(em, 1); break;
    case 4: em_act02_005A88E0(em, 2); break;
    case 5: em_act05_005A89B0(em, 2); break;
    case 6: em_act06_005A8A30(em, 0); break;
    case 7: em_act07_005A8B10(em, 0); break;
    case 8: em_act08_005A8B80(em, 0); break;
    case 9: em_act09_005A8C20(em); break;
    case 10: em_act10_005A8CE0(em); break;
    case 11: em_act11_005A8D70(em); break;
    case 12: em_act12_005A8E30(em); break;
    case 13: em_act13_005A8F00(em); break;
    case 14: em_act14_005A8F90(em); break;
    }
}

#define TURN(em, spd) \
    do { \
        (em)->horm_ang = (u16)Em_Calc_angY((em)->pos, (em)->tgt_pos); \
        em09_dir_calc(&(em)->ang[1], &(em)->horm_ang, spd); \
        cpRotMatrix((em)->ang, (f32 *)(em)->mat); \
    } while (0)

static void em_mov00_005A91E0(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x12C;
        w->x04 = 0x8A;
        em_char_set(em, 12, 0, 0);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            TURN(em, 0x800);
            oikake_ck(em);
        }
        break;
    }
}

static void em_mov01_005A92D0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x21, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 46.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0xE, 0, 0);
            em->work08 = 0x12C;
        }
        break;
    case 2:
        oikake_ck(em);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 200.0f || --em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_mov02_005A9410(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xB, 0, 0);
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

static void em_mov04_005A94F0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x8C;
        em_char_set(em, 0x10, 0, 0);
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        shell02_set(em, 4);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f
            || (em->x19 != 0 && em->x7A4->kind == 2 && em->x7A0->x10 == 0) || --em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            TURN(em, 0x800);
            oikake_ck(em);
        }
        break;
    }
}

static void em_mov05_005A9630(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        em_char_set(em, 0xB, 0, 0);
        pl_flag_set((PLW *)em, 0x20000);
        break;
    case 1:
        if (em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800) < 0x800) {
            pl_flag_clr((PLW *)em, 0x20000);
            em09_next_act_set(em);
        }
        break;
    }
    cpRotMatrix(em->ang, (f32 *)em->mat);
}

static void em_mov06_005A9700(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x12C;
        em_char_set(em, 0xC, 0, 0);
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

static void em_mov07_005A97E0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x12C;
        em_char_set(em, 0xF, 0, 0);
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

static void em_mov08_005A98C0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((u16)ran_suu(1) & 0x7F) + 0x78;
        em_char_set(em, 0xB, 0, 0);
        em->horm_ang = ((u16)ran_suu(1) & 0xF) << 12;
        break;
    case 1:
        if (em->work08 > 0) {
            em->work08--;
        }
        if (em->x8C3 == 0 && em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x100);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

static void em_mov09_005A99B0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x64;
        em_char_set(em, 0x10, 0, 0);
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f
            || (em->x19 != 0 && em->x7A4->kind == 2 && em->x7A0->x10 == 0) || --em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            TURN(em, 0x800);
            oikake_ck(em);
        }
        break;
    }
}

void em_mov10(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x78;
        em_char_set(em, 0xF, 0, 0);
        em->horm_ang = ((u16)ran_suu(1) & 0xF) << 12;
        break;
    case 1:
        if (em->work08 > 0) {
            em->work08--;
        }
        if (em->x8C3 == 0 && em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x100);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

void em_mov11(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x12C;
        em_char_set(em, 0xB, 0, 0);
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

void em_mov12(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((u16)ran_suu(1) & 0x7F) + 0x78;
        em_char_set(em, 0xB, 0, 0);
        break;
    case 1:
        if (em->work08 > 0) {
            em->work08--;
        }
        if (em->x8C3 == 0 && em->work08 <= 0) {
            em09_next_act_set(em);
        } else {
            em->horm_ang = (u16)Em_Calc_angY(em->pos, w->home);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x100);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

void em_mov10(EMW *);
void em_mov11(EMW *);
void em_mov12(EMW *);
static void em_move01_005A9D90(EMW *em) {
    switch (em->x15) {
    case 0: em_mov00_005A91E0(em); break;
    case 1: em_mov01_005A92D0(em); break;
    case 2: em_mov02_005A9410(em); break;
    case 4: em_mov04_005A94F0(em); break;
    case 5: em_mov05_005A9630(em); break;
    case 6: em_mov06_005A9700(em); break;
    case 7: em_mov07_005A97E0(em); break;
    case 8: em_mov08_005A98C0(em); break;
    case 9: em_mov09_005A99B0(em); break;
    case 10: em_mov10(em); break;
    case 11: em_mov11(em); break;
    case 12: em_mov12(em); break;
    }
}

static void em_fly00_005A9E90(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xD, 0, 0);
        em->adj_y = 0.0f;
        em->x3C0[1] = -0.5f;
        break;
    case 1:
        em_rate_add_g(em);
        if (em->pos[1] <= em->x5AC) {
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_move02_005A9F30(EMW *em) {
    switch (em->x15) {
    case 0:
        em_fly00_005A9E90(em);
        break;
    }
}
