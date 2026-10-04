/* shell19 - game.bin, generated from the shell18 template by tools/gen_shell.py. */
#include "shell.h"
#include "prim.h"

extern u8 shell19_tbl[8];
extern s32 shell19_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

static void shell19_move(SHLW *sh);
static void shell19_i(SHLW *sh);
static void shell19_m(SHLW *sh);
static void shell19_d(SHLW *sh);
static void shell19_e(SHLW *sh);

void shell19_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 1;
            sh->arg = arg;
            sh->move = shell19_move;
            sh->em_no = em->id;
            sh->x7A = em->x10;
            sh->char0 = em->char0;
            sh->owner = em;
            sh->xC8 = em->ang[1];
            sh->ang[0] = em->ang[0];
            sh->ang[1] = em->ang[1];
            sh->ang[2] = em->ang[2];
            em->x19 = 0;
        }
    }
}

static void shell19_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell19_i(sh);
        break;
    case 1:
        shell19_m(sh);
        break;
    case 2:
        shell19_d(sh);
        break;
    case 3:
        shell19_m(sh);
        break;
    case 4:
        shell19_d(sh);
        break;
    case 5:
        shell19_d(sh);
        break;
    case 7:
        shell19_d(sh);
        break;
    case 6:
        shell19_e(sh);
        break;
    }
}

static void shell19_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;
    s32 b;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    if (sh->arg == 2) {
        shell_flag_set(sh, 0xA0);
    } else {
        shell_flag_set(sh, 0x20);
    }
    pl_atck_data_set_shl(sh, em, sh->arg, shell19_tbl);
    sh->x88 = shell19_body_tbl[sh->body];
    sh->x8C = 0;
    sh->stg = em->stg;
    if (sh->x60 != 0) {
        if (sh->arg == 3) {
            a = flAbs(em->blend1 % 100);
            b = flAbs(em->act_tm1);
        } else {
            a = flAbs(em->blend0 % 100);
            b = flAbs(em->act_tm0);
        }
        sh->x60 += (u8)(a - b - 1);
        if ((s8)sh->x60 < 0) {
            sh->x60 = 0;
        }
    }
    sh->prim = 0;
}

static void shell19_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    switch (sh->arg) {
    case 0xB:
    case 0x1B:
        if (em->char0 == 0x3F9) {
            sh->x61 = 60;
        }
        break;
    case 0xC:
    case 0xE:
        if (em->char0 == 0x457) {
            sh->x61 = 18;
        }
        break;
    }
    if (sh->arg == 0xB || sh->arg == 0x1B) {
        if (sh->xB == 0 || em->mode == 4 || em->mode == 5) {
            sh->xB = 0;
            sh->mode = 2;
            return;
        }
    } else if (sh->arg == 0xC || sh->arg == 0xE) {
        if (sh->xB == 0 || em->mode == 4 || em->mode == 5) {
            sh->xB = 0;
            sh->mode = 2;
        }
    } else if (em->char0 != sh->char0 || sh->xB == 0) {
        sh->xB = 0;
        sh->mode = 2;
    }
    if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell19_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell19_e(SHLW *sh) {
    push_shell_work(sh);
}
