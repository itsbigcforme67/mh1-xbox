/* em09 - game.bin 0x005A81B0-0x005AC9xx. Per-monster AI for monster kind 9
 * (the item thief: it runs up to a player, steals an item from the pouch
 * (item_theft) and gives it back when killed (item_return)). Names of the
 * steps follow the split (em_act00, em_move00...). Field meanings are
 * mostly guesses. */
#include "em.h"
#include "game.h"
#include "pl.h"
#include "plf.h"
#include "fl.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM09W {
    u8 eff;             /* 0x00 em09_effect_move step */
    u8 _pad01[3];
    s16 x04;            /* 0x04 */
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    u8 _pad08[4];
    u16 x0C;            /* 0x0C counted down by em09_main */
    u16 chase;          /* 0x0E non-zero: chasing a target */
    u8 _pad10[4];
    f32 home[3];        /* 0x14 home position */
    s32 shell;          /* 0x20 shell02_set result */
    PLW *pl;            /* 0x24 player the item was stolen from */
    f32 x28[3];         /* 0x28 yobi position (SetVector) */
    f32 x34;            /* 0x34 yobi range */
    u8 _pad38[2];
    u8 x3A;             /* 0x3A */
    u8 _pad3B;
    u8 x3C;             /* 0x3C stage */
    u8 _pad3D[0x44 - 0x3D];
    u16 x44;            /* 0x44 effect timer */
    s16 item;           /* 0x46 stolen item, -1 none */
    s16 item_num;       /* 0x48 */
    u8 x4A;             /* 0x4A eye/face blink state 0..3 (em09_effect_move) */
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
int shell02_set(EMW *, int);
void em_rate_add_g(EMW *);
void flvecRotY(f32 *, f32);
void em09_act_set();
void em_escape_mind_set(EMW *, u8, u8);
void em_cmd_reset(EMW *);
void em_ikari_add(EMW *, s16);
int Em_Yobi_Ck(EMW *, f32 *);
void push_em_yobi(f32 *);
void SetVector(f32 *, f32, f32, f32);
int em_cancel_act_ck(EMW *, u8);
void em_dur_set(EMW *, int);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_cmd_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
int Code_Make(int, int, int, int);
void Eft20_set(f32, EMW *, int, int);
void Eft13_set_em(EMW *, int, int);
void em_rate_clear_g(EMW *);
int rate_add_g2(EMW *);
void Em_Mahi_Start(EMW *);
void Em_Mahi_End(EMW *);
void em_mahi_eff_set(EMW *, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void em09_init(EMW *);
#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))
void pull_em_yobi(f32 *);
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

void em09_act_set(em, kind, no) EMW *em; int kind; u16 no; {
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

    if (em->x388 != 0 || (u32)(em->mode - 4) <= 2U || em->be_flag == 0) {
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
    a = d >= 0x8000 ? (u16)-d : d;
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
                pull_em_yobi(w->x28);
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

#define ATK_FLY_START(em) \
    do { \
        em->x388 = 2; \
        em->rate_x = 0.0f; \
        em->adj_y = 16.0f; \
        em->adj_y = 30.0f; \
        em->adj_z = 5.0f; \
        flvecRotY(&em->rate_x, DEG2RAD(ANG2DEG(em->ang[1]))); \
        em->x3C0[0] = 0.0f; \
        em->x3C0[1] = -1.0f; \
        em->x3C0[1] = -3.0f; \
        em->x3C0[2] = 0.0f; \
    } while (0)

static void em_atk00_005A9F70(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x15, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            ATK_FLY_START(em);
            em_char_set(em, 0x16, 0, 0);
            shell02_set(em, 1);
            em->x19 = 0;
            em->x06 = 0;
        }
        break;
    case 2:
        em_rate_add_g(em);
        if (em->x194 == 0 && em->pos[1] - 50.0f <= em->x5AC) {
            if (em->x19 == 0) {
                em->x05++;
                em_char_set(em, 0x18, 0, 0);
                em->adj_y *= 0.5f;
                em->x3C0[1] = -0.5f;
                em->x07 = 0;
            } else {
                em->x05++;
                em_char_set(em, 0x17, 0, 0);
                em->x07 = 1;
            }
        }
        break;
    case 3:
        if (em->x388 != 0) {
            if (em->pos[1] <= em->x5AC && em->x388 != 0) {
                em->pos[1] = em->x5AC;
                em->x388 = 0;
            } else {
                em_rate_add_g(em);
            }
        }
        if (em->x194 == 0) {
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            if (em->x07 != 0) {
                em->x07 = 0;
                em09_next_act_set(em);
            } else {
                em09_act_set(em, 0, 3, 0);
            }
        }
        break;
    }
}

static void em_atk01_005AA1F0(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xF, 0, 0);
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        if (em->x15 == 3) {
            em->work08 = 0x78;
            w->shell = shell02_set(em, 7);
            em->act_spd = 1.2f;
        } else {
            em->work08 = 0x3C;
            w->shell = shell02_set(em, 2);
            em->act_spd = 1.0f;
        }
        break;
    case 1: {
        PLW *pl;

        if (em->x19 != 0) {
            pl = em->x7A0;
            if (pl != 0 && pl->id < 4 && pl == &player_work[pl->id] && em->x7A4->kind == 2 && pl->be_flag != 0) {
                item_theft_005A8540(em, pl, w);
                em_escape_mind_set(em, 1, 0x90);
                em->x05++;
                em_char_set(em, 0x17, 0, 0);
                break;
            }
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 3, 0, 0);
        } else {
            if (em->x15 == 3) {
                em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
            }
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800);
            cpRotMatrix(em->ang, (f32 *)em->mat);
            oikake_ck(em);
        }
        break;
    }
    case 2:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_atk02_005AA420(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        ATK_FLY_START(em);
        em_char_set(em, 0x16, 0, 0);
        shell02_set(em, 1);
        em->x19 = 0;
        em->x06 = 0;
        break;
    case 1:
        em_rate_add_g(em);
        if (em->x194 == 0 && em->pos[1] - 50.0f <= em->x5AC) {
            if (em->x19 == 0) {
                em->x05++;
                em_char_set(em, 0x18, 0, 0);
                em->adj_y *= 0.5f;
                em->x3C0[1] = -0.5f;
                em->x07 = 0;
            } else {
                em->x05++;
                em_char_set(em, 0x18, 0, 0);
                em->x07 = 1;
            }
        }
        break;
    case 2:
        if (em->x388 != 0) {
            if (em->pos[1] <= em->x5AC && em->x388 != 0) {
                em->pos[1] = em->x5AC;
                em->x388 = 0;
            } else {
                em_rate_add_g(em);
            }
        }
        if (em->x194 == 0) {
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            if (em->x07 != 0) {
                em->x07 = 0;
                em09_next_act_set(em);
            } else {
                em09_act_set(em, 0, 3, 0);
            }
        }
        break;
    }
}

static void em_move03_005AA670(EMW *em) {
    switch (em->x15) {
    case 0: em_atk00_005A9F70(em); break;
    case 1: em_atk01_005AA1F0(em); break;
    case 2: em_atk02_005AA420(em); break;
    case 3: em_atk01_005AA1F0(em); break;
    }
}

static void em_dm00_005AA700(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3C, 0, 0);
        em->ang[1] = em->dm_ang + 0x8000;
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x40, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_dm01_005AA7D0(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3D, 0, 0);
        ang[0] = 0;
        em->ang[1] = ang[1] = em->dm_ang + 0x8000;
        ang[2] = 0;
        cpRotMatrix(ang, (f32 *)mat);
        in[0] = 0.0f;
        in[1] = 10.0f;
        in[2] = -12.0f;
        flvecApplyMat33(out, in, (f32 *)mat);
        em_rate_clear_g(em);
        em->rate_x = out[0];
        em->adj_y = out[1];
        em->adj_z = out[2];
        em->x3C0[1] = -0.4f;
        em->x3C0[2] = 0.15f;
        em->x388 = 2;
        em_cmd_reset(em);
        break;
    case 1:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x3E, 0, 0);
            em->work08 = 0x5A;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x3F, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_dm02_005AA990(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1F, 0, 0);
        em_escape_mind_set(em, 0, 0x90);
        em_cmd_reset(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
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

static void em_dm03_005AAA60(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    if (em->work08 > 0) {
        em->work08--;
    }
    if (em->x05 < 4) {
        em_mahi_eff_set(em, 2);
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3C, 0, 0);
        em->ang[1] = em->dm_ang + 0x8000;
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 120.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0x41, 0, 0);
            w->x04 = 0x3A;
        }
        break;
    case 2:
        if (--w->x04 <= 0) {
            em->x05++;
            em_char_set(em, 0x42, 0, 0);
        }
        break;
    case 3:
        if (em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x40, 0, 0);
            Em_Mahi_End(em);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x8BD = 0;
            em09_next_act_set(em);
        }
        break;
    }
}

static void em_move04_005AAC30(EMW *em) {
    switch (em->x15) {
    case 0: em_dm00_005AA700(em); break;
    case 1: em_dm01_005AA7D0(em); break;
    case 2: em_dm02_005AA990(em); break;
    case 3: em_dm03_005AAA60(em); break;
    }
}

static void em_die00_005AACC0(EMW *em) {
    EM09W *w = (EM09W *)em->ex;
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3D, 0, 0);
        in[0] = 0.0f;
        in[1] = 10.0f;
        ang[0] = 0;
        in[2] = -12.0f;
        em->ang[1] = ang[1] = em->dm_ang + 0x8000;
        ang[2] = 0;
        cpRotMatrix(ang, (f32 *)mat);
        flvecApplyMat33(out, in, (f32 *)mat);
        em_rate_clear_g(em);
        em->rate_x = out[0];
        em->adj_y = out[1];
        em->adj_z = out[2];
        em->x3C0[1] = -0.4f;
        em->x3C0[2] = 0.15f;
        em->x388 = 2;
        item_return(em, w);
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x3E, 0, 0);
            em->work08 = 0x12C;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x3F, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x11, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            if (w->x4B == 1) {
                pull_em_yobi(w->x28);
            }
            em->x01 = 0;
            em_act_set(em, 5, 2);
        }
        break;
    }
}

static void em_die01_005AAF20(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3C, 0, 0);
        em->ang[1] = em->dm_ang + 0x8000;
        em->x388 = 0;
        item_return(em, w);
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x40, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x11, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            if (w->x4B == 1) {
                pull_em_yobi(w->x28);
            }
            em->x01 = 0;
            em_act_set(em, 5, 2);
        }
        break;
    }
}

static void em_die_rev_005AB090(EMW *em) {
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
        em09_init(em);
        Quest_enemy_revival_set(em);
        switch (em->stg) {
        case 6:
            Quest_enemy_escape(em);
            em->x04++;
            em->x01 = 0;
            break;
        default:
            if (em->type & 0x10) {
                em09_act_set(em, 1, 2, 0);
            }
            break;
        }
        break;
    }
}

static void em_move05_005AB180(EMW *em) {
    switch (em->x15) {
    case 0: em_die00_005AACC0(em); break;
    case 1: em_die01_005AAF20(em); break;
    case 2: em_die_rev_005AB090(em); break;
    }
}

void em09_main_sub(EMW *em);

void em09_main(EMW *em) {
    EM09W *w = (EM09W *)em->ex;
    u8 dmg[4];
    u8 r;
    int yobi;

    if (game_w.stage == 0x37 && em->type != 0) {
        em->x40C = 10;
        em->x40E = 10;
    }
    if (w->x0C != 0) {
        w->x0C--;
    }
    if (em->x888 == 1 && em->x8B6 == 0) {
        em_ikari_add(em, em->x8B0 / 1800);
    }
    yobi = (u8)Em_Yobi_Ck(em, w->x28);
    if (w->x4B == 0 && em->x388 == 0 && em->x888 == 0 && yobi != 0 && em_cancel_act_ck(em, 4) == 0) {
        em->x917 |= 4;
        w->x4B = 2;
        em->x88B = 1;
    }
    if (w->x4B == 1) {
        pull_em_yobi(w->x28);
        w->x4B = 2;
    }
    r = Em_Dmg_Sys(em, dmg);
    switch (r) {
    case 0:
    case 3:
    case 4:
    case 9:
    case 11:
        break;
    case 1:
    case 2:
        if (em->x388 == 2) {
            em_act_set(em, 5, 0);
        }
        em_act_set(em, 5, 1);
        break;
    case 5:
        if (act_ck((PLW *)em, 4, 1) == 0) {
            em09_act_set(em, 4, 1, 0);
            em_dur_set(em, 0);
        }
        break;
    case 6:
        if (act_ck((PLW *)em, 4, 1) == 0) {
            em_mahi_dmg_timer_set(em);
            em09_act_set(em, 4, 3, 0);
        }
        break;
    case 7:
        em09_act_set(em, 4, 1, 0);
        em_dur_set(em, 0);
        break;
    case 8:
        if (act_ck((PLW *)em, 4, 1) == 0) {
            em_sleep_dmg_timer_set(em);
            em09_act_set(em, 0, 12, 0);
        }
        break;
    case 10:
        em->x88B = 1;
        Em_Sleep_End(em);
        em09_act_set(em, 4, 1, 0);
        break;
    case 12:
    case 13:
        em09_act_set(em, 4, 1, 0);
        em_dur_set(em, 0);
        break;
    case 14:
        if (em->x388 == 2) {
            em09_act_set(em, 4, 1, 0);
            em_dur_set(em, 0);
        } else {
            em09_act_set(em, 4, 0, 0);
        }
        em->x839 = 0;
        break;
    }
    if ((r & 0xFF) != 0) {
        w->chase = 0;
        em->x88B = 1;
        if (w->x4B == 0) {
            SetVector(w->x28, em->pos[0], em->pos[1], em->pos[2]);
            w->x34 = 3000.0f;
            w->x3C = em->stg;
            w->x3A = 1;
            push_em_yobi(w->x28);
            w->x4B = 1;
        }
    }
    switch (em->x734) {
    case 3:
        if (em->x839 != 0) {
            em_cmd_ck(em);
            em->x839 = 0;
        }
        break;
    }
    em09_main_sub(em);
    if (em->x6FF != 0) {
        em09_main_sub(em);
        em->x6FF = 0;
    }
}

void em09_main_sub(EMW *em) {
    switch (em->mode) {
    case 0: em_move00_005A90A0(em); break;
    case 1: em_move01_005A9D90(em); break;
    case 2: em_move02_005A9F30(em); break;
    case 3: em_move03_005AA670(em); break;
    case 4: em_move04_005AAC30(em); break;
    case 5: em_move05_005AB180(em); break;
    case 6: em_move00_005A90A0(em); break;
    case 7: em_move00_005A90A0(em); break;
    }
}

static void move_default_005AB6E0(EMW *em) {
}

static void sound_call_005AB6F0(EMW *em, int frame, int se) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_se_req2(em, se, 0, em->pos, 6, 0);
    }
}

/* Sound and effect script per animation (sound_call(em, frame, se) plays
 * the sound once the animation reaches the frame). */
void ef_move_sub_005AB750(EMW *em, EM09W *w) {
    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_005AB6F0(em, 4, Code_Make(0x12, 1, 0x12, 1));
        break;
    case 0x3EA:
        sound_call_005AB6F0(em, 0x12, 1);
        sound_call_005AB6F0(em, 0x26, 1);
        sound_call_005AB6F0(em, 0x72, 1);
        sound_call_005AB6F0(em, 0x7E, 1);
        sound_call_005AB6F0(em, 0x98, 1);
        sound_call_005AB6F0(em, 0xCC, 0xC);
        sound_call_005AB6F0(em, 8, 0x1E);
        sound_call_005AB6F0(em, 0x58, 0x1E);
        sound_call_005AB6F0(em, 0xD0, 0x1E);
        break;
    case 0x3EB:
        sound_call_005AB6F0(em, 0x26, 0x12);
        sound_call_005AB6F0(em, 6, 0x1E);
        sound_call_005AB6F0(em, 0x16, 0x1E);
        sound_call_005AB6F0(em, 0x9A, 0x1E);
        sound_call_005AB6F0(em, 0xA4, 0x1E);
        break;
    case 0x3EC:
        sound_call_005AB6F0(em, 0xE, Code_Make(9, 2, 9, 2));
        sound_call_005AB6F0(em, 0x4C, Code_Make(0xC, 2, 0xC, 2));
        sound_call_005AB6F0(em, 0xA0, Code_Make(9, 2, 9, 2));
        sound_call_005AB6F0(em, 0xC8, Code_Make(0xA, 2, 0xA, 2));
        sound_call_005AB6F0(em, 8, 0x1E);
        sound_call_005AB6F0(em, 0x50, 0x1E);
        sound_call_005AB6F0(em, 0x8C, 0x1E);
        sound_call_005AB6F0(em, 0xC8, 0x1E);
        break;
    case 0x3ED:
        sound_call_005AB6F0(em, 4, 0x17);
        sound_call_005AB6F0(em, 0xE, 3);
        break;
    case 0x3F3:
        sound_call_005AB6F0(em, 0x50, Code_Make(0x12, 2, 0x12, 2));
        sound_call_005AB6F0(em, 0xC, 0x1E);
        sound_call_005AB6F0(em, 0x1E, 0x1E);
        sound_call_005AB6F0(em, 0x2E, 0x1E);
        sound_call_005AB6F0(em, 0x42, 0x1E);
        sound_call_005AB6F0(em, 0x58, 0x1E);
        sound_call_005AB6F0(em, 0x76, 0x1E);
        sound_call_005AB6F0(em, 0x8C, 0x1E);
        sound_call_005AB6F0(em, 0x98, 0x1E);
        sound_call_005AB6F0(em, 0xA2, 0x1E);
        sound_call_005AB6F0(em, 0xAE, 0x1E);
        sound_call_005AB6F0(em, 0xB8, 0x1E);
        sound_call_005AB6F0(em, 0xC2, 0x1E);
        sound_call_005AB6F0(em, 0xAE, 0x1E);
        break;
    case 0x3F4:
        sound_call_005AB6F0(em, 8, Code_Make(0xE, 2, 0xE, 2));
        sound_call_005AB6F0(em, 0xA, 0x1E);
        sound_call_005AB6F0(em, 0x16, 0x1E);
        sound_call_005AB6F0(em, 0x20, 0x1E);
        sound_call_005AB6F0(em, 0x2E, 0x1E);
        sound_call_005AB6F0(em, 0x38, 0x1E);
        sound_call_005AB6F0(em, 0x44, 0x1E);
        sound_call_005AB6F0(em, 0x4E, 0x1E);
        sound_call_005AB6F0(em, 0x58, 0x1E);
        sound_call_005AB6F0(em, 0x64, 0x1E);
        sound_call_005AB6F0(em, 0x70, 0x1E);
        break;
    case 0x3F5:
        sound_call_005AB6F0(em, 8, Code_Make(0xD, 2, 0xD, 2));
        sound_call_005AB6F0(em, 0xC, 0x21);
        sound_call_005AB6F0(em, 0xC, 0x19);
        sound_call_005AB6F0(em, 0x2A, 0x17);
        sound_call_005AB6F0(em, 0x2E, 0x18);
        break;
    case 0x3F6:
        sound_call_005AB6F0(em, 8, 0x1E);
        sound_call_005AB6F0(em, 0x18, 0x1E);
        sound_call_005AB6F0(em, 0x28, 0x1E);
        break;
    case 0x3F7:
        sound_call_005AB6F0(em, 8, Code_Make(0, 2, 0, 2));
        sound_call_005AB6F0(em, 0xC, 0x18);
        sound_call_005AB6F0(em, 0x12, 0x22);
        sound_call_005AB6F0(em, 0x22, 0x18);
        sound_call_005AB6F0(em, 0x28, 0x22);
        break;
    case 0x3F8:
        sound_call_005AB6F0(em, 0x2E, Code_Make(0x11, 1, 0x11, 1));
        sound_call_005AB6F0(em, 0xA, 0x17);
        sound_call_005AB6F0(em, 0x18, 0x17);
        sound_call_005AB6F0(em, 0x26, 0x17);
        sound_call_005AB6F0(em, 0x32, 0x17);
        break;
    case 0x3F9:
        sound_call_005AB6F0(em, 0x10, 0x1A);
        sound_call_005AB6F0(em, 0x24, 0x1A);
        sound_call_005AB6F0(em, 0x3A, 0x1A);
        sound_call_005AB6F0(em, 0x4E, 0x1A);
        sound_call_005AB6F0(em, 0x7E, 0x14);
        sound_call_005AB6F0(em, 0x80, 0x19);
        sound_call_005AB6F0(em, 0x78, 6);
        sound_call_005AB6F0(em, 0x9C, 0x1B);
        if (em_frame_check(em, 18.0f, 0) != 0) {
            Eft20_set(0.5f, em, 3, 4);
        }
        if (em_frame_check(em, 58.0f, 0) != 0) {
            Eft20_set(0.5f, em, 3, 4);
        }
        if (em_frame_check(em, 38.0f, 0) != 0) {
            Eft20_set(0.5f, em, 2, 4);
        }
        if (em_frame_check(em, 126.0f, 0) != 0) {
            Eft13_set_em(em, 2, 7);
        }
        if (em_frame_check(em, 158.0f, 0) != 0) {
            Eft13_set_em(em, 2, 3);
        }
        if (em_frame_check(em, 166.0f, 0) != 0) {
            Eft13_set_em(em, 2, 3);
        }
        if (em_frame_check(em, 176.0f, 0) != 0) {
            Eft13_set_em(em, 2, 3);
            break;
        }
        break;
    case 0x3FD:
        sound_call_005AB6F0(em, 2, 0x16);
        sound_call_005AB6F0(em, 0xE, 0x17);
        sound_call_005AB6F0(em, 0x18, 3);
        sound_call_005AB6F0(em, 0x48, 0x17);
        break;
    case 0x3FE:
        sound_call_005AB6F0(em, 2, 0x18);
        sound_call_005AB6F0(em, 6, 0x21);
        sound_call_005AB6F0(em, 0x10, 4);
        break;
    case 0x3FF:
        sound_call_005AB6F0(em, 0x12, 0x18);
        sound_call_005AB6F0(em, 0x22, 0x17);
        sound_call_005AB6F0(em, 0x2A, 0xB);
        sound_call_005AB6F0(em, 0x70, 0xF);
        sound_call_005AB6F0(em, 0xAA, 0xF);
        sound_call_005AB6F0(em, 0xF0, 0xD);
        sound_call_005AB6F0(em, 0x68, 0x16);
        sound_call_005AB6F0(em, 0x82, 0x17);
        sound_call_005AB6F0(em, 0x8A, 0x16);
        sound_call_005AB6F0(em, 0xA2, 0x17);
        sound_call_005AB6F0(em, 0xA8, 0x16);
        sound_call_005AB6F0(em, 0xBE, 0x17);
        sound_call_005AB6F0(em, 0xC8, 0x16);
        sound_call_005AB6F0(em, 0xE0, 0x17);
        sound_call_005AB6F0(em, 0xE6, 0x20);
        sound_call_005AB6F0(em, 0x124, 0x17);
        break;
    case 0x400:
        sound_call_005AB6F0(em, 0x10, 0x1E);
        sound_call_005AB6F0(em, 0x1A, 0x1E);
        sound_call_005AB6F0(em, 0x24, 0x1E);
        sound_call_005AB6F0(em, 0x30, 0x1E);
        sound_call_005AB6F0(em, 0x3A, 0x1E);
        sound_call_005AB6F0(em, 0x46, 0x1E);
        sound_call_005AB6F0(em, 0x52, 0x1E);
        sound_call_005AB6F0(em, 0x92, 0x1E);
        break;
    case 0x407:
        sound_call_005AB6F0(em, 0xE, 0xB);
        sound_call_005AB6F0(em, 0x44, 0x13);
        sound_call_005AB6F0(em, 0x52, 0x13);
        sound_call_005AB6F0(em, 0x5E, 0x13);
        sound_call_005AB6F0(em, 0x44, 0x16);
        sound_call_005AB6F0(em, 0x52, 0x16);
        sound_call_005AB6F0(em, 0x5E, 0x16);
        sound_call_005AB6F0(em, 0x70, 0x1C);
        sound_call_005AB6F0(em, 0x72, 6);
        break;
    case 0x408:
        sound_call_005AB6F0(em, 4, 0x15);
        sound_call_005AB6F0(em, 0xC, 0x18);
        sound_call_005AB6F0(em, 8, Code_Make(5, 4, 7, 4));
        sound_call_005AB6F0(em, 0x1E, 0x15);
        sound_call_005AB6F0(em, 0x36, 0x1A);
        break;
    case 0x409:
        sound_call_005AB6F0(em, 8, 0x15);
        sound_call_005AB6F0(em, 6, 0x19);
        sound_call_005AB6F0(em, 0x26, 0x21);
        sound_call_005AB6F0(em, 0x26, 0x1A);
        break;
    case 0x40A:
        sound_call_005AB6F0(em, 8, 0x17);
        sound_call_005AB6F0(em, 0xC, 0x21);
        sound_call_005AB6F0(em, 0x26, 0x21);
        break;
    case 0x40B:
        sound_call_005AB6F0(em, 0xA, 0x17);
        sound_call_005AB6F0(em, 0xE, 0x21);
        sound_call_005AB6F0(em, 0xE, Code_Make(5, 4, 7, 4));
        sound_call_005AB6F0(em, 0x16, 0x15);
        sound_call_005AB6F0(em, 0x1E, 0x15);
        sound_call_005AB6F0(em, 0x26, 0x15);
        sound_call_005AB6F0(em, 0x2E, 0x15);
        sound_call_005AB6F0(em, 0x34, 0x15);
        sound_call_005AB6F0(em, 0x3C, 0x15);
        sound_call_005AB6F0(em, 0x44, 0x15);
        sound_call_005AB6F0(em, 0x52, 0x17);
        sound_call_005AB6F0(em, 0x6C, 0x1E);
        break;
    case 0x40C:
        sound_call_005AB6F0(em, 4, 0x11);
        sound_call_005AB6F0(em, 0x54, 0x19);
        sound_call_005AB6F0(em, 0x5E, 0x19);
        sound_call_005AB6F0(em, 0x78, 0x18);
        break;
    case 0x40D:
        sound_call_005AB6F0(em, 8, 0x18);
        sound_call_005AB6F0(em, 0xE, 0x15);
        sound_call_005AB6F0(em, 0x16, 0x15);
        sound_call_005AB6F0(em, 0x1E, 0x15);
        sound_call_005AB6F0(em, 0x26, 0x15);
        sound_call_005AB6F0(em, 0x2E, 0x15);
        sound_call_005AB6F0(em, 0x34, 0x15);
        sound_call_005AB6F0(em, 0x44, 0x1F);
        sound_call_005AB6F0(em, 0x54, 0x1F);
        sound_call_005AB6F0(em, 0x72, 0x1E);
        break;
    case 0x40E:
        sound_call_005AB6F0(em, 0x12, Code_Make(8, 4, 0xA, 4));
        sound_call_005AB6F0(em, 0x3C, Code_Make(9, 4, 0xC, 4));
        sound_call_005AB6F0(em, 0x10, 0x15);
        sound_call_005AB6F0(em, 0x40, 0x15);
        sound_call_005AB6F0(em, 0x10, 0x18);
        sound_call_005AB6F0(em, 0x40, 0x18);
        break;
    case 0x424:
        sound_call_005AB6F0(em, 4, Code_Make(5, 4, 7, 4));
        sound_call_005AB6F0(em, 0x32, 0x10);
        sound_call_005AB6F0(em, 0xC, 0x1E);
        sound_call_005AB6F0(em, 0x16, 0x1E);
        sound_call_005AB6F0(em, 0x16, 0x1E);
        sound_call_005AB6F0(em, 0x20, 0x1E);
        sound_call_005AB6F0(em, 0x2E, 0x1E);
        sound_call_005AB6F0(em, 0x46, 0x1E);
        sound_call_005AB6F0(em, 0xD2, 0x1C);
        break;
    case 0x425:
        sound_call_005AB6F0(em, 4, Code_Make(5, 4, 7, 4));
        break;
    case 0x426:
        sound_call_005AB6F0(em, 4, Code_Make(4, 2, 4, 2));
        sound_call_005AB6F0(em, 4, 0x1C);
        sound_call_005AB6F0(em, 0x2E, 0x1D);
        sound_call_005AB6F0(em, 4, Code_Make(6, 3, 6, 3));
        sound_call_005AB6F0(em, 0x64, 0x1C);
        sound_call_005AB6F0(em, 0xA, 0x1A);
        sound_call_005AB6F0(em, 0x34, 0x1A);
        break;
    case 0x427:
        sound_call_005AB6F0(em, 0x1C, 0x15);
        sound_call_005AB6F0(em, 0x28, 0x15);
        sound_call_005AB6F0(em, 0x34, 0x15);
        sound_call_005AB6F0(em, 0x40, 0x15);
        sound_call_005AB6F0(em, 0x1C, 0x11);
        sound_call_005AB6F0(em, 0x66, 4);
        sound_call_005AB6F0(em, 0x68, 0x16);
        sound_call_005AB6F0(em, 0x80, 0x17);
        sound_call_005AB6F0(em, 0x84, 0x18);
        break;
    case 0x428:
        sound_call_005AB6F0(em, 0x14, 0x1E);
        sound_call_005AB6F0(em, 0x22, 0x1E);
        sound_call_005AB6F0(em, 0x2A, 0x1E);
        sound_call_005AB6F0(em, 0x46, 0x1E);
        sound_call_005AB6F0(em, 0x54, 0x1E);
        sound_call_005AB6F0(em, 0x6C, 0x1E);
        sound_call_005AB6F0(em, 0x8C, 0x13);
        break;
    case 0x429:
        sound_call_005AB6F0(em, 0x30, 0x13);
        sound_call_005AB6F0(em, 4, 6);
        break;
    case 0x42A:
        sound_call_005AB6F0(em, 4, 0x13);
        sound_call_005AB6F0(em, 0x40, 0x1A);
        sound_call_005AB6F0(em, 0x5A, 0x1D);
        break;
    case 0x42B:
        sound_call_005AB6F0(em, 4, 0x12);
        sound_call_005AB6F0(em, 0x9C, 0x1D);
        break;
    default:
        move_default_005AB6E0(em);
        break;
    }
}

void em09_effect_move_005AC940(EMW *em) {
    EM09W *w = (EM09W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        w->x44 = (u16)ran_suu(1) & 0xF;
        break;
    case 1:
        if (em->mode == 4 || em->mode == 5) {
            w->x4A = 3;
            w->x44 = 2;
        } else {
            if (em->char0 != 0x42B) {
                w->x44++;
                switch ((w->x44 >> 1) % 20) {
                case 1:
                    w->x4A = 1;
                    break;
                case 2:
                    w->x4A = 2;
                    break;
                default:
                    w->x4A = 0;
                    break;
                }
            } else {
                w->x44 = 0;
                w->x4A = 2;
            }
        }
        ef_move_sub_005AB750(em, w);
        break;
    }
}

typedef struct EM09_MATSEL {
    s32 _00;
    s32 num;            /* 0x04 number of materials */
    s32 idx[33];        /* 0x08 material indices (0x4C bytes each) */
} EM09_MATSEL;

void em09_material_sub(EMW *em, int type, EM09_MATSEL *tbl) {
    u8 *base = *(u8 **)((u8 *)em->mdl + 0x10);
    int i = 0;
    EM09W *w = (EM09W *)em->ex;
    s32 *p = (s32 *)&tbl[type];

    if (0 < p[1]) {
        s32 *num = &p[1];

        do {
            u8 *m = p[2] * 0x4C + base;

            *(f32 *)(m + 0x10) = em->x798;
            switch (type) {
            case 0:
            case 1:
                switch (w->x4A) {
                default:
                    switch (i) {
                    case 2:
                    case 3:
                    case 4:
                        *(s32 *)(m + 0x10) = 0;
                        break;
                    }
                    break;
                case 1:
                    switch (i) {
                    case 4:
                    case 3:
                    case 1:
                        *(s32 *)(m + 0x10) = 0;
                        break;
                    }
                    break;
                case 2:
                    switch (i) {
                    case 1:
                    case 2:
                    case 4:
                        *(s32 *)(m + 0x10) = 0;
                        break;
                    }
                    break;
                case 3:
                    switch (i) {
                    case 1:
                    case 2:
                    case 3:
                        *(s32 *)(m + 0x10) = 0;
                        break;
                    }
                    break;
                }
                break;
            }
            flSetRenderState((u8)(i + 0x3A), (u32)m);
            i++;
            p++;
        } while (i < *num);
    }
}

void em09_local_init_005ACC40(EMW *em) {
    eft01_set((PLW *)em, 0);
}

void dummy_em_prog_005ACC50(void) {
}
