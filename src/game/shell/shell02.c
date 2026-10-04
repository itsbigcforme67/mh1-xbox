/* shell02 - game.bin 0x00628B40-0x00628FA8. Generated from the shell18
 * template, then edited by hand. */
#include "shell.h"
#include "prim.h"

extern u8 shell02_tbl[8];
extern s32 shell02_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

static void shell02_move(SHLW *sh);
static void shell02_i(SHLW *sh);
static void shell02_m(SHLW *sh);
static void shell02_d(SHLW *sh);
static void shell02_e(SHLW *sh);
static void shell02_trans(SHLW *sh);

SHLW *shell02_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) == 0) {
        return 0;
    }
    sh = pull_shell_work(0);
    if (sh != 0) {
        sh->type = 2;
        sh->arg = arg;
        sh->move = shell02_move;
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
    return sh;
}

static void shell02_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell02_i(sh);
        break;
    case 1:
        shell02_m(sh);
        break;
    case 2:
        shell02_d(sh);
        break;
    case 3:
        shell02_m(sh);
        break;
    case 4:
        shell02_d(sh);
        break;
    case 5:
        shell02_d(sh);
        break;
    case 7:
        shell02_d(sh);
        break;
    case 6:
        shell02_e(sh);
        break;
    }
}

static void shell02_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = shell02_trans;
    shell_flag_set(sh, 0x20);
    sh->xB4 = 0;
    sh->stg = em->stg;
    pl_atck_data_set_shl(sh, em, sh->arg + 1, shell02_tbl);
    sh->x88 = shell02_body_tbl[sh->body];
    sh->x8C = shell02_body_tbl[sh->body];
    if (sh->x60 != 0) {
        a = flAbs(em->blend0 % 100);
        sh->x60 += (u8)(a - (s32)flAbs(em->act_tm0) - 1);
        if ((s8)sh->x60 < 0) {
            sh->x60 = 0;
        }
    }
    sh->prim = 0;
}

static void shell02_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    switch (sh->arg) {
    default:
        break;
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xE:
    case 0xF:
    case 0x10:
    case 0x11:
        sh->x61 = 99;
        break;
    }
    if (em->char0 != sh->char0 || sh->xB == 0 || em->x04 >= 2) {
        sh->xB = 0;
        sh->mode = 2;
    } else if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell02_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell02_e(SHLW *sh) {
    push_shell_work(sh);
}

static void shell02_trans(SHLW *sh) {
}
