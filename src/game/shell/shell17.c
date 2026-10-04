/* shell17 - game.bin 0x00636920-0x00636E28. Generated from the shell18
 * template, then edited by hand for the flash ("senko") effect. */
#include "shell.h"
#include "prim.h"

extern u8 shell17_tbl[8];
extern s32 shell17_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

/* Flash effect handed to the renderer with push_senko/pull_senko. */
typedef struct SENKO {
    VEC3 pos;           /* 0x00 */
    f32 size;           /* 0x0C */
    u8 x10;             /* 0x10 */
    u8 _pad11[4];
    u8 x15;             /* 0x15 */
} SENKO;

void push_senko(SENKO *);
void pull_senko(SENKO *);

static void shell17_move(SHLW *sh);
static void shell17_i(SHLW *sh);
static void shell17_m(SHLW *sh);
static void shell17_d(SHLW *sh);
static void shell17_e(SHLW *sh);

void shell17_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 17;
            sh->arg = arg;
            sh->move = shell17_move;
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

void shell17_set3(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(1);
        if (sh != 0) {
            sh->type = 17;
            sh->arg = arg;
            sh->move = shell17_move;
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

static void shell17_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell17_i(sh);
        break;
    case 1:
        shell17_m(sh);
        break;
    case 2:
        shell17_d(sh);
        break;
    case 3:
        shell17_m(sh);
        break;
    case 4:
        shell17_d(sh);
        break;
    case 5:
        shell17_d(sh);
        break;
    case 7:
        shell17_d(sh);
        break;
    case 6:
        shell17_e(sh);
        break;
    }
}

static void shell17_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;

    sh->mode++;
    sh->be_flag = 1;
    sh->x14 = 0;
    shell_flag_set(sh, 0x20);
    pl_atck_data_set_shl(sh, em, sh->arg, shell17_tbl);
    sh->x88 = shell17_body_tbl[sh->body];
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
    if (sh->arg == 22) {
        SENKO *sk = sh->senko;

        flvecCopy(sk, &sh->pos2);
        sk->size = 2000.0f;
        sk->x10 = 0;
        sk->x15 = 5;
        push_senko(sk);
    }
}

static void shell17_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    switch (sh->arg) {
    default:
        break;
    case 0x13:
    case 0x17:
    case 0x1C:
        sh->x61 = 99;
        break;
    }
    if (sh->arg == 0x16) {
        sh->senko->x15 = 5;
    }
    if (em->char0 != sh->char0 || sh->xB == 0) {
        sh->xB = 0;
        sh->mode = 2;
        if (sh->arg == 0x16) {
            pull_senko(sh->senko);
        }
    }
    if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell17_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell17_e(SHLW *sh) {
    push_shell_work(sh);
}
