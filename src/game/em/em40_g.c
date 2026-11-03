/* em40_g - G game.bin 0x00647060-0x00648E30: monster kind 40, which no G quest uses.
 * PC only (VERSION_MHG; not in the PS2 build, not byte-checked): written from G's asm for the
 * PC so the unused kind can be put on screen (docs/FINDS.md "G's unused monster kind 40").
 *
 * What it is [checked, see FINDS]: G's name table calls kind 40 "ヒゲルー"; its code is a
 * cut-down copy of em09 (Felyne / Melynx, the item thief) without the item theft, the jump
 * attack, the knock-back physics or the eye blink; its own motion file em40_tbl.bin has 18
 * motions for a 16-bone skeleton (the Felyne's bone count), and G's file table gives it no
 * model of its own (em01_amh, the Rathian, the same placeholder as kinds 0 and 51).
 *
 * Functions (G addresses; names follow em09_nm.c where the code is the same):
 *   em40_init 0x647060, oikake_ck 0x647210, em40_act_set 0x647270, em40_next_act_set 0x647320,
 *   (0x647390 = em09_dir_calc), acts 0x647410-0x647710 (wait / idle motions), move 0x647780,
 *   0x647850-0x647CA0 (chase, turn, walk home), move 0x647DD0, 0x647EB0, attack 0x647EC0/F30,
 *   damage 0x647F70/0x648000/0x648120, die 0x648170/0x648260/0x6482F0, 0x648340/50,
 *   em40_main 0x648390, em40_main_sub 0x6484E0, sound_call 0x6485B0, sound/effect script
 *   0x648610, effect move 0x648DD0, program entries 0x648E20 (eft01_set) / 0x648E30 (nop).
 * Two stack matrices (0x648000 / 0x648170 build a rotation that nothing reads: leftovers of
 * em09's knock-back) are left out. */
#include "version.h"
#include "em.h"
#include "game.h"
#include "pl.h"
#include "plf.h"
#include "fl.h"

#ifdef VERSION_MHG

/* Per-monster work at EMW+0x444 (em09's layout, the fields kind 40 uses). */
typedef struct EM40W {
    u8 eff;             /* 0x00 effect move step */
    u8 _pad01[3];
    s16 x04;            /* 0x04 set to 0x8A by the chase (em09 the same) */
    s16 anim;           /* 0x06 animation the sound script follows */
    u8 _pad08[4];
    u16 x0C;            /* 0x0C counted down by em40_main */
    u16 x0E;            /* 0x0E cleared by the act set (em09: chasing) */
    u8 _pad10[4];
    f32 home[3];        /* 0x14 home position (where it was put) */
    u8 _pad20[8];
    f32 x28[3];         /* 0x28 yobi position (em09) */
    u8 _pad34[0x4B - 0x34];
    u8 x4B;             /* 0x4B 1: a yobi to pull when it dies */
} EM40W;

extern GAME_W game_w;

/* quest_w (G): only the quest number (G 0x32D018 = quest_w + QWO(8), as MH1) */
#ifdef VERSION_MH2
#include "mh2_layouts.h"
typedef QUEST_W QUEST_W40_M;
#define QUEST_W40 QUEST_W40_M
#else
typedef struct QUEST_W40 {
    u8 _pad00[8];
    u16 no;             /* 0x08 */
} QUEST_W40;

#endif
extern QUEST_W40 quest_w;

void em_act_set(EMW *, int, u16);
void target_kind_set(EMW *, f32 *);
void em_char_set(EMW *, int, int, int);
int game_func_005499A0(EMW *em, s16 no);
void em_dur_init(EMW *);
void em_dur_set(EMW *, int);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_cmd_ck(EMW *);
void em_cmd_reset(EMW *);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void pull_em_yobi(f32 *);
int ran_suu(int);
u16 Em_Calc_angY(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
int em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 *);
void pl_flag_set(PLW *, int);
void pl_flag_clr(PLW *, int);
int em_frame_check(EMW *, f32, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
int Code_Make(int, int, int, int);
void Shell11_set(EMW *, int);


void em40_init(EMW *em);

/* 0x647210 (em09 oikake_ck): the chase gives up when the target hunter left the stage */
static void oikake_ck_40(EMW *em) {
    if (em->work08 > 15) {
        s8 n = em->x617;

        if (n == -1) {
            em->work08 = 15;
        } else if (em->stg != ((PLW *)player_work)[n].stg) {
            em->work08 = 15;
        }
    }
}

/* 0x647270: em09_act_set without the Felyne's revival points */
static void em40_act_set(EMW *em, int kind, u16 no) {
    EM40W *w = (EM40W *)em->ex;

    w->x0E = 0;
    switch ((u16)kind) {
    case 0:
        em->x388 = 0;
        break;
    case 1:
        em->x388 = 0;
        target_kind_set(em, em->tgt_pos);
        break;
    case 3:
        em->x388 = 0;
        break;
    case 4:
        em->x388 = 0;
        break;
    }
    em_act_set(em, kind, no);
}

/* 0x647320: back to act 0 step 1 and ask the command program for the next act */
static void em40_next_act_set(EMW *em) {
    em->act_spd = 1.0f;
    if (em->x734 == 3) {
        em->x839 = 1;
        em_act_set(em, 0, 1);
    } else {
        em_act_set(em, 0, 1);
    }
    em->mode_old = em->mode;
    em->x15_old = em->x15;
}

/* 0x647410: wait a random 0-15 ticks (motion 1), then ask for a command */
static void em40_wait(EMW *em) {
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

/* 0x6474A0: motion 1 once */
static void em40_act01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em40_next_act_set(em);
        }
        break;
    }
}

/* 0x647520: idle motions 2 / 3, or 6 for 90 ticks */
static void em40_act02(EMW *em, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        switch (n) {
        default:
            em_char_set(em, 2, 0, 0);
            break;
        case 1:
            em_char_set(em, 3, 0, 0);
            break;
        case 2:
            em_char_set(em, 6, 0, 0);
            em->work08 = 0x5A;
            break;
        }
        break;
    case 1:
        switch (n) {
        default:
            if (em->x194 == 0) {
                em40_next_act_set(em);
            }
            break;
        case 2:
            if (--em->work08 <= 0) {
                em40_next_act_set(em);
            }
            break;
        }
        break;
    }
}

/* 0x647640 / 0x6476A0 / 0x647710: one idle motion (9, 10, 11) to its end */
static void em40_act_once(EMW *em, int mot) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, mot, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em40_next_act_set(em);
        }
        break;
    }
}

/* 0x647780: act 0 */
static void em40_move00(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em40_wait(em); break;
    case 1: em40_act01(em); break;
    case 2: em40_act02(em, 0); break;
    case 3: em40_act02(em, 1); break;
    case 4: em40_act02(em, 2); break;
    case 5: em40_act_once(em, 9); break;
    case 6: em40_act_once(em, 10); break;
    case 7: em40_act_once(em, 11); break;
    }
}

#define TURN40(em, spd) \
    do { \
        (em)->horm_ang = (u16)Em_Calc_angY((em)->pos, (em)->tgt_pos); \
        em09_dir_calc(&(em)->ang[1], &(em)->horm_ang, spd); \
        cpRotMatrix((em)->ang, (f32 *)(em)->mat); \
    } while (0)

/* 0x647850: go to the target (motion 12, 4 or 13) until within 100 or 300 ticks */
static void em40_mov00(EMW *em, int n) {
    EM40W *w = (EM40W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x12C;
        w->x04 = 0x8A;
        switch (n & 0xFF) {
        default:
            em_char_set(em, 0xC, 0, 0);
            break;
        case 1:
            em_char_set(em, 4, 0, 0);
            break;
        case 2:
            em_char_set(em, 0xD, 0, 0);
            break;
        }
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em40_next_act_set(em);
        } else {
            TURN40(em, 0x800);
            oikake_ck_40(em);
        }
        break;
    }
}

/* 0x647990: motion 7 or 8 toward the target, at most 140 ticks or the motion's end */
static void em40_mov03(EMW *em, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x8C;
        if ((n & 0xFF) != 1) {
            em_char_set(em, 7, 0, 0);
        } else {
            em_char_set(em, 8, 0, 0);
        }
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0 || em->x194 == 0) {
            em40_next_act_set(em);
        } else {
            TURN40(em, 0x800);
            oikake_ck_40(em);
        }
        break;
    }
}

/* 0x647AC0: turn on the spot toward the target (motion 12) */
static void em40_mov05(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        em_char_set(em, 0xC, 0, 0);
        pl_flag_set((PLW *)em, 0x20000);
        break;
    case 1:
        if (em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800) < 0x800) {
            pl_flag_clr((PLW *)em, 0x20000);
            em40_next_act_set(em);
        }
        break;
    }
    cpRotMatrix(em->ang, (f32 *)em->mat);
}

/* 0x647B90: head for where the target was when it started (motion 12 or 13) */
static void em40_mov06(EMW *em, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x12C;
        if ((n & 0xFF) != 1) {
            em_char_set(em, 0xC, 0, 0);
        } else {
            em_char_set(em, 0xD, 0, 0);
        }
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em40_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x800);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

/* 0x647CA0: walk back toward the home position for 120-247 ticks (motion 12 or 4) */
static void em40_mov08(EMW *em, int n) {
    EM40W *w = (EM40W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((u16)ran_suu(1) & 0x7F) + 0x78;
        if ((n & 0xFF) != 1) {
            em_char_set(em, 0xC, 0, 0);
        } else {
            em_char_set(em, 4, 0, 0);
        }
        break;
    case 1:
        if (em->work08 > 0) {
            em->work08--;
        }
        if (em->x8C3 == 0 && em->work08 <= 0) {
            em40_next_act_set(em);
        } else {
            em->horm_ang = (u16)Em_Calc_angY(em->pos, w->home);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x100);
            cpRotMatrix(em->ang, (f32 *)em->mat);
        }
        break;
    }
}

/* 0x647DD0: act 1 */
static void em40_move01(EMW *em) {
    switch (em->x15) {
    case 0: em40_mov00(em, 0); break;
    case 1: em40_mov00(em, 1); break;
    case 2: em40_mov00(em, 2); break;
    case 3: em40_mov03(em, 0); break;
    case 4: em40_mov03(em, 1); break;
    case 5: em40_mov05(em); break;
    case 6: em40_mov06(em, 0); break;
    case 7: em40_mov06(em, 1); break;
    case 8: em40_mov08(em, 0); break;
    case 9: em40_mov08(em, 1); break;
    }
}

/* 0x647EC0: the one attack: motion 21; the sound script hits with Shell11 (arg 0x10) at frame 24 */
static void em40_atk00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x15, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em40_next_act_set(em);
        }
        break;
    }
}

/* 0x647F70: flinch (motion 60) */
static void em40_dm00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3C, 0, 0);
        em->ang[1] = em->dm_ang + 0x8000;
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em40_next_act_set(em);
        }
        break;
    }
}

/* 0x648000: knocked down (motions 61, 62, 63; em09 also throws the body back) */
static void em40_dm01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3D, 0, 0);
        em->ang[1] = em->dm_ang + 0x8000;
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x3E, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3F, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em40_next_act_set(em);
        }
        break;
    }
}

/* 0x648170: dies (motion 62), then act 5 step 1 */
static void em40_die00(EMW *em) {
    EM40W *w = (EM40W *)em->ex;

    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x3E, 0, 0);    /* (G turns a stack vector by dm_ang + 0x8000 that nothing reads) */
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            if (w->x4B == 1) {
                pull_em_yobi(w->x28);
            }
            em->x01 = 0;
            em_act_set(em, 5, 1);
        }
        break;
    }
}

/* 0x648260: comes back when the quest revives it */
static void em40_die_rev(EMW *em) {
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
        em40_init(em);
        Quest_enemy_revival_set(em);
        break;
    }
}

/* 0x6484E0 */
static void em40_main_sub(EMW *em) {
    switch (em->mode) {
    case 0: em40_move00(em); break;
    case 1: em40_move01(em); break;
    case 2: em40_wait(em); break;                       /* 0x647EB0 */
    case 3: if (em->x15 == 0) em40_atk00(em); break;    /* 0x647F30 */
    case 4:                                             /* 0x648120 */
        switch (em->x15) {
        case 0: em40_dm00(em); break;
        case 1: em40_dm01(em); break;
        }
        break;
    case 5:                                             /* 0x6482F0 */
        switch (em->x15) {
        case 0: em40_die00(em); break;
        case 1: em40_die_rev(em); break;
        }
        break;
    case 6: break;                                      /* 0x648350 -> 0x648340: nothing */
    case 7: em40_move00(em); break;
    }
}

/* 0x647060: em09_init without the stolen item and the stage-0x37 variant */
void em40_init(EMW *em) {
    EM40W *w = (EM40W *)em->ex;

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
    em_char_set(em, 1, 0, 0);           /* G: game 0x549FE0, G's init-time char set (as the other G inits) */
    em->x388 = 0;
    em_act_set(em, 1, 8);
    em->x792 = em->x302 = game_func_005499A0(em, 0);    /* em_vital_tbl[40][0] = 300 */
    w->x0E = 0;
    w->x0C = 0;
    w->home[0] = em->pos[0];
    w->home[1] = 0.0f;
    w->home[2] = em->pos[2];
    w->x4B = 0;
    em->x56A = 0;
    if (em->type & 0x80) {
        em->scale[2] = 10.0f;
        em->scale[1] = 10.0f;
        em->scale[0] = 10.0f;
    }
    em_dur_init(em);
    em->x734 = 3;
#ifndef __MWERKS__
    {   /* PC: G's em_prog_tbl[40] names 0x648DD0 after MH1's em04_effect_move (tools/mhg_map paired them by
         * shape), so the host would run the Mosswine's script; kind 40 gets its own program table */
        extern void *em40_prog_pc[];
        *(void **)((u8 *)em + 0x3CC) = em40_prog_pc;
    }
#endif
}

/* 0x648390 */
void em40_main(EMW *em) {
    EM40W *w = (EM40W *)em->ex;
    u8 dmg[4];

    if (w->x0C != 0) {
        w->x0C--;
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 1:
    case 2:
        em_act_set(em, 5, 0);
        break;
    case 12:
    case 13:
        em40_act_set(em, 4, 1);
        em_dur_set(em, 0);
        break;
    case 14:
        if (em->x388 == 2) {
            em40_act_set(em, 4, 1);
        } else {
            em40_act_set(em, 4, 0);
        }
        em->x839 = 0;
        break;
    }
    switch (em->x734) {
    case 3:
        if (em->x839 != 0) {
            em_cmd_ck(em);
            em->x839 = 0;
        }
        break;
    }
    em40_main_sub(em);
    if (em->x6FF != 0) {
        em40_main_sub(em);
        em->x6FF = 0;
    }
}

/* 0x6485B0 */
static void sound_call_40(EMW *em, int frame, int se) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_se_req2(em, se, 0, em->pos, 6, 0);
    }
}

/* 0x648610: sounds per animation (the Felyne's numbers, em09 ef_move_sub's first cases) and the attack's hit */
static void em40_ef_move_sub(EMW *em, EM40W *w) {
    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_40(em, 4, Code_Make(0x12, 1, 0x12, 1));
        break;
    case 0x3EA:
        sound_call_40(em, 0x12, 1);
        sound_call_40(em, 0x26, 1);
        sound_call_40(em, 0x72, 1);
        sound_call_40(em, 0x7E, 1);
        sound_call_40(em, 0x98, 1);
        sound_call_40(em, 0xCC, 0xC);
        sound_call_40(em, 8, 0x1E);
        sound_call_40(em, 0x58, 0x1E);
        sound_call_40(em, 0xD0, 0x1E);
        break;
    case 0x3EB:
        sound_call_40(em, 0x26, 0x12);
        sound_call_40(em, 6, 0x1E);
        sound_call_40(em, 0x16, 0x1E);
        sound_call_40(em, 0x9A, 0x1E);
        sound_call_40(em, 0xA4, 0x1E);
        break;
    case 0x3EC:
        sound_call_40(em, 0xE, Code_Make(9, 2, 9, 2));
        sound_call_40(em, 0x4C, Code_Make(0xC, 2, 0xC, 2));
        sound_call_40(em, 0xA0, Code_Make(9, 2, 9, 2));
        sound_call_40(em, 0xC8, Code_Make(0xA, 2, 0xA, 2));
        sound_call_40(em, 8, 0x1E);
        sound_call_40(em, 0x50, 0x1E);
        sound_call_40(em, 0x8C, 0x1E);
        sound_call_40(em, 0xC8, 0x1E);
        break;
    case 0x3ED:
        sound_call_40(em, 4, 0x17);
        sound_call_40(em, 0xE, 3);
        break;
    case 0x3F3:
        sound_call_40(em, 0x50, Code_Make(0x12, 2, 0x12, 2));
        sound_call_40(em, 0xC, 0x1E);
        sound_call_40(em, 0x1E, 0x1E);
        sound_call_40(em, 0x2E, 0x1E);
        sound_call_40(em, 0x42, 0x1E);
        sound_call_40(em, 0x58, 0x1E);
        sound_call_40(em, 0x76, 0x1E);
        sound_call_40(em, 0x8C, 0x1E);
        sound_call_40(em, 0x98, 0x1E);
        sound_call_40(em, 0xA2, 0x1E);
        sound_call_40(em, 0xAE, 0x1E);
        sound_call_40(em, 0xB8, 0x1E);
        sound_call_40(em, 0xC2, 0x1E);
        sound_call_40(em, 0xAE, 0x1E);
        break;
    case 0x3F4:
        sound_call_40(em, 8, Code_Make(0xE, 2, 0xE, 2));
        sound_call_40(em, 0xA, 0x1E);
        sound_call_40(em, 0x16, 0x1E);
        sound_call_40(em, 0x20, 0x1E);
        sound_call_40(em, 0x2E, 0x1E);
        sound_call_40(em, 0x38, 0x1E);
        sound_call_40(em, 0x44, 0x1E);
        sound_call_40(em, 0x4E, 0x1E);
        sound_call_40(em, 0x58, 0x1E);
        sound_call_40(em, 0x64, 0x1E);
        sound_call_40(em, 0x70, 0x1E);
        break;
    case 0x3F5:
        sound_call_40(em, 8, Code_Make(0xD, 2, 0xD, 2));
        sound_call_40(em, 0xC, 0x21);
        sound_call_40(em, 0xC, 0x19);
        sound_call_40(em, 0x2A, 0x17);
        sound_call_40(em, 0x2E, 0x18);
        break;
    case 0x3FD:                         /* motion 21, the attack */
        sound_call_40(em, 2, 0x16);
        sound_call_40(em, 0xE, 0x17);
        sound_call_40(em, 0x18, 3);
        sound_call_40(em, 0x48, 0x17);
        if (em_frame_check(em, 24.0f, 0) != 0) {
            Shell11_set(em, 0x10);
        }
        break;
    case 0x424:
        sound_call_40(em, 4, Code_Make(5, 4, 7, 4));
        sound_call_40(em, 0x32, 0x10);
        sound_call_40(em, 0xC, 0x1E);
        sound_call_40(em, 0x16, 0x1E);
        sound_call_40(em, 0x16, 0x1E);
        sound_call_40(em, 0x20, 0x1E);
        sound_call_40(em, 0x2E, 0x1E);
        sound_call_40(em, 0x46, 0x1E);
        sound_call_40(em, 0xD2, 0x1C);
        break;
    case 0x425:
        sound_call_40(em, 4, Code_Make(5, 4, 7, 4));
        break;
    case 0x426:
        sound_call_40(em, 4, Code_Make(4, 2, 4, 2));
        sound_call_40(em, 4, 0x1C);
        sound_call_40(em, 0x2E, 0x1D);
        sound_call_40(em, 4, Code_Make(6, 3, 6, 3));
        sound_call_40(em, 0x64, 0x1C);
        sound_call_40(em, 0xA, 0x1A);
        sound_call_40(em, 0x34, 0x1A);
        break;
    case 0x427:
        sound_call_40(em, 0x1C, 0x15);
        sound_call_40(em, 0x28, 0x15);
        sound_call_40(em, 0x34, 0x15);
        sound_call_40(em, 0x40, 0x15);
        sound_call_40(em, 0x1C, 0x11);
        sound_call_40(em, 0x66, 4);
        sound_call_40(em, 0x68, 0x16);
        sound_call_40(em, 0x80, 0x17);
        sound_call_40(em, 0x84, 0x18);
        break;
    default:
        break;
    }
}

/* 0x648DD0 (em_prog_tbl[40][3]): the per-tick effect move */
static void em40_effect_move(EMW *em) {
    EM40W *w = (EM40W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        em40_ef_move_sub(em, w);
        break;
    }
}

/* 0x648E20 (em_prog_tbl[40][0], [1]): eft01_set(em, 0); the G pointer table resolves it by this name */
void game_func_00648E20(EMW *em) {
    eft01_set((PLW *)em, 0);
}

/* 0x648E30 (em_prog_tbl[40][2]): nothing */
static void em40_prog_nop(EMW *em) {
    (void)em;
}

#ifndef __MWERKS__
/* PC: kind 40's program table (G 0x6BE560 with its fourth entry pointing here; see em40_init) */
void *em40_prog_pc[4] = { (void *)game_func_00648E20, (void *)game_func_00648E20, (void *)em40_prog_nop,
                          (void *)em40_effect_move };
#endif

#endif /* VERSION_MHG */
