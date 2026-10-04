/* shell05 - game.bin 0x0062A090-0x0062A6B8. Generated from the shell18
 * template, plus spawners set2-set4 and the 0x47 case by hand. */
#include "shell.h"
#include "prim.h"

extern u8 shell05_tbl[8];
extern s32 shell05_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

static void shell05_move(SHLW *sh);
static void shell05_i(SHLW *sh);
static void shell05_m(SHLW *sh);
static void shell05_d(SHLW *sh);
static void shell05_e(SHLW *sh);
static void shell05_trans(SHLW *sh);

void shell05_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 5;
            sh->arg = arg;
            sh->move = shell05_move;
            sh->em_no = em->id;
            sh->x7A = em->x10;
            sh->char0 = em->char0;
            sh->owner = em;
            sh->xC8 = *(s32 *)&em->pos.y;
            VEC3_COPY(sh->pos, em->pos);
            em->x19 = 0;
            sh->x05 = 0;
            sh->xCC = 0;
        }
    }
}

void shell05_set4(EMW *em, int arg, int cc) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 5;
            sh->arg = arg;
            sh->move = shell05_move;
            sh->em_no = em->id;
            sh->x7A = em->x10;
            sh->char0 = em->char0;
            sh->owner = em;
            sh->xC8 = *(s32 *)&em->pos.y;
            VEC3_COPY(sh->pos, em->pos);
            em->x19 = 0;
            sh->x05 = 0;
            sh->xCC = cc;
        }
    }
}

void shell05_set2(EMW *em, f32 *pos, int arg) {
    SHLW *sh = pull_shell_work(0);

    if (sh != 0) {
        sh->type = 5;
        sh->arg = arg;
        sh->move = shell05_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->char0 = em->char0;
        sh->owner = em;
        sh->xC8 = *(s32 *)&em->pos.y;
        sh->pos.x = 0.0f;
        sh->pos.y = 0.0f;
        sh->pos.z = 0.0f;
        sh->pos2.x = pos[0];
        sh->pos2.y = pos[1];
        sh->pos2.z = pos[2];
        em->x19 = 0;
        sh->x05 = 1;
        sh->xCC = 0;
    }
}

/* An object that launches shells on behalf of a monster. */
typedef struct SHL_SRC {
    u8 _pad00[0x24];
    VEC3 pos;           /* 0x24 */
    u8 _pad30[4];
    EMW *em;            /* 0x34 */
} SHL_SRC;

void shell05_set3(SHL_SRC *src, int arg) {
    SHLW *sh = pull_shell_work(0);
    EMW *em;

    if (sh != 0) {
        em = src->em;
        sh->type = 5;
        sh->arg = arg;
        sh->move = shell05_move;
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
        sh->xCC = 0;
    }
}

static void shell05_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell05_i(sh);
        break;
    case 1:
        shell05_m(sh);
        break;
    case 2:
        shell05_d(sh);
        break;
    case 3:
        shell05_d(sh);
        break;
    case 4:
        shell05_d(sh);
        break;
    case 5:
        shell05_d(sh);
        break;
    case 7:
        shell05_d(sh);
        break;
    case 6:
        shell05_e(sh);
        break;
    }
}

static void shell05_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = shell05_trans;
    shell_flag_set(sh, 0x20);
    pl_atck_data_set_shl(sh, em, sh->arg, shell05_tbl);
    sh->x88 = shell05_body_tbl[sh->body];
    sh->x8C = 0;
    sh->stg = em->stg;
    if (sh->arg == 0x47) {
        sh->x61 = (u32)sh->xCC >> 1;
    }
    if (sh->x60 != 0) {
        a = flAbs(em->blend0 % 100);
        sh->x60 += (u8)(a - (s32)flAbs(em->act_tm0) - 1);
        if ((s8)sh->x60 < 0) {
            sh->x60 = 0;
        }
    }
    sh->prim = 0;
}

static void shell05_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    if (em->char0 != sh->char0 || sh->xB == 0) {
        sh->xB = 0;
        sh->mode = 2;
    }
    if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell05_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell05_e(SHLW *sh) {
    push_shell_work(sh);
}

static void shell05_trans(SHLW *sh) {
}
