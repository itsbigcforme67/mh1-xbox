/* shell09 - game.bin 0x006331C0-0x00633B48. Blasts/shockwaves; some types
 * also spawn a second shell09 for the wind-noise effect ("kaze oto"). */
#include "shell.h"
#include "game.h"
#include "pl.h"
#include "prim.h"

extern u8 shell09_tbl[4];
extern s32 shell09_body_tbl[];
extern s16 atck_data_no[20];

void flvecCopy(void *, void *);
void atck_data_set_shl(SHLW *, int, u8 *);
void pl_atck_data_set_shl(SHLW *, void *, int, u8 *);
void se_req2(int, int, int, VEC3 *, int, int);
u16 ran_suu(int);
void release_prim(s16);

static void shell09_move(SHLW *sh);
static void shell09_kaze_oto_set(SHLW *sh);
static void shell09_se_req(SHLW *sh);
static void shell09_i(SHLW *sh);
static void shell09_m(SHLW *sh);
static void shell09_d(SHLW *sh);
static void shell09_e(SHLW *sh);

static void shell09_set_com(SHLW *sh) {
    sh->type = 9;
    sh->move = shell09_move;
    switch (sh->arg) {
    case 0:
    case 2:
    case 7:
    case 8:
    case 0xE:
    case 0xF:
    case 0x10:
        shell09_kaze_oto_set(sh);
        break;
    }
}

void Shell09_set(VEC3 *pos, int arg, int stg) {
    SHLW *sh;

    if ((sh = pull_shell_work(0)) != 0) {
        sh->arg = arg;
        flvecCopy(&sh->pos2, pos);
        sh->em_no = 0;
        sh->x7A = 0;
        sh->stg = stg;
        shell09_set_com(sh);
    }
}

void Shell09_set_pl(PLW *pl, VEC3 *pos, int arg) {
    SHLW *sh;

    if ((sh = pull_shell_work(0)) != 0) {
        sh->arg = arg;
        flvecCopy(&sh->pos2, pos);
        if (arg != 2) {
            sh->em_no = pl->id;
            sh->x7A = ((EMW *)pl)->x10;
            sh->owner = pl;
            sh->xC8 = ((EMW *)pl)->ang[1];
            sh->stg = pl->stg;
        } else {
            sh->em_no = 0;
            sh->x7A = 0;
            sh->stg = game_w.stage;
        }
        shell09_set_com(sh);
    }
}

void Shell09_set_em(EMW *em, VEC3 *pos, int arg) {
    SHLW *sh;

    if ((sh = pull_shell_work(0)) != 0) {
        sh->arg = arg;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->owner = em;
        sh->xC8 = em->ang[1];
        flvecCopy(&sh->pos2, pos);
        sh->stg = em->stg;
        shell09_set_com(sh);
    }
}

void Shell09_set_pl2(PLW *pl, VEC3 *pos, int arg, int stg) {
    SHLW *sh;

    if ((sh = pull_shell_work(0)) != 0) {
        sh->arg = arg;
        flvecCopy(&sh->pos2, pos);
        sh->em_no = pl->id;
        sh->x7A = ((EMW *)pl)->x10;
        sh->owner = pl;
        sh->xC8 = ((EMW *)pl)->ang[1];
        sh->stg = stg;
        shell09_set_com(sh);
    }
}

static void shell09_kaze_oto_set(SHLW *sh) {
    SHLW *nw;
    int arg;

    switch (sh->arg) {
    case 0:
    case 7:
    case 8:
    case 0xE:
    case 0xF:
    case 0x10:
        arg = 9;
        break;
    case 1:
        arg = 3;
        break;
    case 2:
        arg = 10;
        break;
    case 4:
        arg = 5;
        break;
    default:
        return;
    }
    if ((nw = pull_shell_work(0)) != 0) {
        nw->type = 9;
        nw->arg = arg;
        nw->move = shell09_move;
        nw->em_no = sh->em_no;
        nw->x7A = sh->x7A;
        nw->owner = sh->owner;
        nw->xC8 = sh->xC8;
        flvecCopy(&nw->pos2, &sh->pos2);
        nw->stg = sh->stg;
    }
}

static void shell09_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell09_i(sh);
        break;
    case 1:
        shell09_m(sh);
        break;
    case 2:
        shell09_d(sh);
        break;
    case 3:
        shell09_d(sh);
        break;
    case 4:
        shell09_d(sh);
        break;
    case 5:
        shell09_d(sh);
        break;
    case 7:
        shell09_d(sh);
        break;
    case 6:
        shell09_e(sh);
        break;
    }
}

static void shell09_se_req(SHLW *sh) {
    if (sh->stg == game_w.stage) {
        switch (sh->arg) {
        case 0:
        case 0x10:
            se_req2(1, 0x4D, 0, &sh->pos2, 1, 0);
            break;
        case 2:
            se_req2(1, 0x4F, 0, &sh->pos2, 1, 0);
            break;
        case 7:
            if (ran_suu(1) & 1) {
                se_req2(1, 0x4E, 0, &sh->pos2, 1, 0);
            }
            break;
        case 8:
        case 0xE:
        case 0xF:
            se_req2(1, 0x4E, 0, &sh->pos2, 1, 0);
            break;
        }
    }
}

static void shell09_i(SHLW *sh) {
    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    shell09_se_req(sh);
    switch (sh->arg) {
    case 0:
    case 2:
    case 0x10:
        shell_flag_set(sh, 4);
        atck_data_set_shl(sh, atck_data_no[sh->arg], shell09_tbl);
        break;
    case 6:
        shell_flag_set(sh, 0);
        atck_data_set_shl(sh, atck_data_no[sh->arg], shell09_tbl);
        break;
    case 1:
    case 4:
        shell_flag_set(sh, 0x200);
        pl_atck_data_set_shl(sh, sh->owner, atck_data_no[sh->arg], shell09_tbl);
        break;
    case 7:
    case 8:
    case 0xE:
    case 0xF:
        shell_flag_set(sh, 4);
        pl_atck_data_set_shl(sh, sh->owner, atck_data_no[sh->arg], shell09_tbl);
        break;
    case 9:
    case 0xA:
        shell_flag_set(sh, 0x400);
        atck_data_set_shl(sh, atck_data_no[sh->arg], shell09_tbl);
        break;
    case 0xD:
        shell_flag_set(sh, 0x400);
        pl_atck_data_set_shl(sh, sh->owner, atck_data_no[sh->arg], shell09_tbl);
        break;
    default:
        shell_flag_set(sh, 0);
        pl_atck_data_set_shl(sh, sh->owner, atck_data_no[sh->arg], shell09_tbl);
        break;
    }
    sh->x88 = shell09_body_tbl[sh->arg];
    sh->x8C = 0;
    sh->char0 = 0;
    sh->prim = 0;
}

static void shell09_m(SHLW *sh) {
    if ((sh->arg == 1 || sh->arg == 4) && sh->x61 <= 0) {
        shell09_kaze_oto_set(sh);
        sh->x61 = 0;
        sh->mode++;
        return;
    }
    if (++sh->char0 > 40) {
        sh->x61 = 0;
        sh->mode++;
    }
    if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell09_d(SHLW *sh) {
    if ((sh->arg == 1 || sh->arg == 4) && sh->mode == 3) {
        shell09_kaze_oto_set(sh);
    }
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell09_e(SHLW *sh) {
    push_shell_work(sh);
}
