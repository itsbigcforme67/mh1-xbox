/* shell04 - game.bin, generated from the shell18 template by tools/gen_shell.py. */
#include "shell.h"
#include "prim.h"

extern u8 shell04_tbl[8];
extern s32 shell04_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

static void shell04_move(SHLW *sh);
static void shell04_i(SHLW *sh);
static void shell04_m(SHLW *sh);
static void shell04_d(SHLW *sh);
static void shell04_e(SHLW *sh);

void shell04_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 4;
            sh->arg = arg;
            sh->move = shell04_move;
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

/* An object that launches shells on behalf of a monster. */
typedef struct SHL_SRC {
    u8 _pad00[0x24];
    VEC3 pos;           /* 0x24 */
    u8 _pad30[4];
    EMW *em;            /* 0x34 */
} SHL_SRC;

void shell04_set2(SHL_SRC *src, int arg) {
    SHLW *sh = pull_shell_work(0);
    EMW *em;

    if (sh != 0) {
        em = src->em;
        sh->type = 4;
        sh->arg = arg;
        sh->move = shell04_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->char0 = em->char0;
        sh->owner = em;
        sh->xC8 = em->ang[1];
        sh->ang[0] = em->ang[0];
        sh->ang[1] = em->ang[1];
        sh->ang[2] = em->ang[2];
        sh->pos2.x = src->pos.x;
        sh->pos2.y = src->pos.y;
        sh->pos2.z = src->pos.z;
        em->x19 = 0;
        sh->x05 = 1;
    }
}

static void shell04_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell04_i(sh);
        break;
    case 1:
        shell04_m(sh);
        break;
    case 2:
        shell04_d(sh);
        break;
    case 3:
        shell04_m(sh);
        break;
    case 4:
        shell04_d(sh);
        break;
    case 5:
        shell04_d(sh);
        break;
    case 7:
        shell04_d(sh);
        break;
    case 6:
        shell04_e(sh);
        break;
    }
}

static void shell04_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;
    s32 b;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    shell_flag_set(sh, 0x20);
    pl_atck_data_set_shl(sh, em, sh->arg, shell04_tbl);
    sh->x88 = shell04_body_tbl[sh->body];
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

static void shell04_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    switch (sh->arg) {
    default:
        break;
    case 0x2:
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

static void shell04_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell04_e(SHLW *sh) {
    push_shell_work(sh);
}
