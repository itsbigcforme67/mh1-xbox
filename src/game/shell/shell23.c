/* shell23 - game.bin, generated from the shell18 template by tools/gen_shell.py. */
#include "shell.h"
#include "prim.h"

extern u8 shell23_tbl[8];
extern s32 shell23_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

static void shell23_move(SHLW *sh);
static void shell23_i(SHLW *sh);
static void shell23_m(SHLW *sh);
static void shell23_d(SHLW *sh);
static void shell23_e(SHLW *sh);

void shell23_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 1;
            sh->arg = arg;
            sh->move = shell23_move;
            sh->em_no = em->id;
            sh->x7A = em->x10;
            sh->char0 = em->char0;
            sh->owner = em;
            sh->xC8 = *(s32 *)&em->pos.y;
            VEC3_COPY(sh->pos, em->pos);
            em->x19 = 0;
        }
    }
}

static void shell23_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell23_i(sh);
        break;
    case 1:
        shell23_m(sh);
        break;
    case 2:
        shell23_d(sh);
        break;
    case 3:
        shell23_m(sh);
        break;
    case 4:
        shell23_d(sh);
        break;
    case 5:
        shell23_d(sh);
        break;
    case 7:
        shell23_d(sh);
        break;
    case 6:
        shell23_e(sh);
        break;
    }
}

static void shell23_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    shell_flag_set(sh, 0x20);
    pl_atck_data_set_shl(sh, em, sh->arg, shell23_tbl);
    sh->x88 = shell23_body_tbl[sh->body];
    sh->x8C = 0;
    sh->stg = em->stg;
    if (sh->x60 != 0) {
        a = flAbs(em->blend0 % 100);
        sh->x60 += (u8)(a - (s32)flAbs(em->act_tm0) - 1);
        if ((s8)sh->x60 < 0) {
            sh->x60 = 0;
        }
    }
    sh->prim = 0;
}

static void shell23_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    switch (sh->arg) {
    default:
        break;
    case 0xE:
    case 0x13:
    case 0x17:
    case 0x18:
    case 0x1A:
        sh->x61 = 99;
        break;
    }
    if (em->char0 != sh->char0 || sh->xB == 0) {
        sh->xB = 0;
        sh->mode = 2;
    }
    if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell23_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell23_e(SHLW *sh) {
    push_shell_work(sh);
}
