/* shell01 - game.bin 0x006285E0-0x00628B38. Generated from the shell18
 * template, plus the set2/set3 spawners by hand. */
#include "shell.h"
#include "prim.h"

extern u8 shell01_tbl[8];
extern s32 shell01_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

static void shell01_move(SHLW *sh);
static void shell01_i(SHLW *sh);
static void shell01_m(SHLW *sh);
static void shell01_d(SHLW *sh);
static void shell01_e(SHLW *sh);

void shell01_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 1;
            sh->arg = arg;
            sh->move = shell01_move;
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

/* An object that launches shells on behalf of a monster. */
typedef struct SHL01_SRC {
    u8 _pad00[0x24];
    VEC3 pos;           /* 0x24 */
    u8 _pad30[4];
    EMW *em;            /* 0x34 */
} SHL01_SRC;

void shell01_set2(SHL01_SRC *src, int arg) {
    SHLW *sh = pull_shell_work(0);
    EMW *em;

    if (sh != 0) {
        em = src->em;
        sh->type = 1;
        sh->arg = arg;
        sh->move = shell01_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->char0 = em->char0;
        sh->owner = em;
        sh->xC8 = *(s32 *)&em->pos.y;
        VEC3_COPY(sh->pos, em->pos);
        sh->pos2.x = src->pos.x;
        sh->pos2.y = src->pos.y;
        sh->pos2.z = src->pos.z;
        em->x19 = 0;
        sh->x05 = 1;
    }
}

void shell01_set3(EMW *em, f32 *pos, int arg) {
    SHLW *sh = pull_shell_work(0);

    if (sh != 0) {
        sh->type = 1;
        sh->arg = arg;
        sh->move = shell01_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->char0 = em->char0;
        sh->owner = em;
        sh->xC8 = *(s32 *)&em->pos.y;
        VEC3_COPY(sh->pos, em->pos);
        sh->pos2.x = pos[0];
        sh->pos2.y = pos[1];
        sh->pos2.z = pos[2];
        em->x19 = 0;
        sh->x05 = 1;
    }
}

static void shell01_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell01_i(sh);
        break;
    case 1:
        shell01_m(sh);
        break;
    case 2:
        shell01_d(sh);
        break;
    case 3:
        shell01_m(sh);
        break;
    case 4:
        shell01_d(sh);
        break;
    case 5:
        shell01_d(sh);
        break;
    case 7:
        shell01_d(sh);
        break;
    case 6:
        shell01_e(sh);
        break;
    }
}

static void shell01_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;

    sh->mode++;
    sh->be_flag = 1;
    sh->x14 = 0;
    shell_flag_set(sh, 0x20);
    pl_atck_data_set_shl(sh, em, sh->arg, shell01_tbl);
    sh->x88 = shell01_body_tbl[sh->body];
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

static void shell01_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    switch (sh->arg) {
    default:
        break;
    case 0x18:
    case 0x22:
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

static void shell01_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell01_e(SHLW *sh) {
    push_shell_work(sh);
}
