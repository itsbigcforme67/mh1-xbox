/* shell00 - game.bin 0x006362B0-0x00636918. The player's own weapon attack:
 * hit data, element/ailment values from the weapon, hit counting. */
#include "shell.h"
#include "pl.h"

extern u8 shell00_tbl[4];
extern s32 shell00_body_tbl[];

u8 Em_stg_ck(void *);
void pl_atck_data_set_shl(SHLW *, void *, int, u8 *);
f32 flAbs(f32);
void flvecApplyMat33(VEC3 *, VEC3 *, void *);
s16 Get_atk_value(PLW *, int);

static void shell00_move(SHLW *sh);
static void shell00_i(SHLW *sh);
static void shell00_m(SHLW *sh);
static void shell00_d(SHLW *sh);
static void shell00_e(SHLW *sh);

void flvecCopy(void *, void *);

void shell00_set(PLW *pl, int arg) {
    SHLW *sh = pull_shell_work(0);

    if (sh != 0) {
        sh->type = 0;
        sh->arg = arg;
        sh->move = shell00_move;
        sh->em_no = pl->id;
        sh->x7A = ((EMW *)pl)->x10;
        sh->char0 = pl->char0;
        sh->owner = pl;
        sh->xC8 = *(s32 *)&((EMW *)pl)->pos.y;
        sh->xB8 = pl->cnt39A;
        VEC3_COPY(sh->pos, ((EMW *)pl)->pos);
        flvecCopy(&sh->pos2, (u8 *)pl + 0xAC);
        ((EMW *)pl)->x19 = 0;
    }
}

static void shell00_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell00_i(sh);
        break;
    case 1:
        shell00_m(sh);
        break;
    case 2:
        shell00_d(sh);
        break;
    case 3:
        sh->mode = 1;
        sh->x61 += 2;
        shell00_m(sh);
        break;
    case 4:
        shell00_d(sh);
        break;
    case 5:
        shell00_d(sh);
        break;
    case 7:
        shell00_d(sh);
        break;
    case 6:
        shell00_e(sh);
        break;
    }
}

static void shell00_i(SHLW *sh) {
    PLW *pl = &player_work[sh->em_no];
    int atk = 0;
    VEC3 d, v;
    s16 val;
    s32 a;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    pl_atck_data_set_shl(sh, pl, sh->arg, shell00_tbl);
    sh->x88 = shell00_body_tbl[sh->body];
    sh->x8C = 0;
    sh->stg = pl->stg;
    switch (sh->arg) {
    case 0xB:
        d.x = 0.0f;
        d.y = 0.0f;
        d.z = 200.0f;
        flvecApplyMat33(&v, &d, pl->rot);
        sh->pos2.x += v.x;
        sh->pos2.y += v.y;
        sh->pos2.z += v.z;
        sh->x7E = 4;
        shell_flag_set(sh, 0x60);
        atk = 1;
        break;
    case 4:
    case 5:
    case 0x1D:
        shell_flag_set(sh, 0x20);
        break;
    default:
        shell_flag_set(sh, 0x60);
        atk = 1;
        break;
    }
    if (atk != 0) {
        if ((val = Get_atk_value(pl, 0)) > 0) {
            sh->ailment |= 0x10;
            sh->ailment_val = val;
        }
        if ((val = Get_atk_value(pl, 1)) > 0) {
            sh->ailment |= 0x20;
            sh->ailment_val = val;
        }
        if ((val = Get_atk_value(pl, 2)) > 0) {
            sh->ailment |= 0x40;
            sh->ailment_val = val;
        }
        if ((val = Get_atk_value(pl, 3)) > 0) {
            sh->ailment |= 0x80;
            sh->ailment_val = val;
        }
        if (pl->cnt39A % 3 == 0) {
            if ((val = Get_atk_value(pl, 4)) > 0) {
                sh->ailment |= 2;
                sh->ailment_val = val;
            }
            if ((val = Get_atk_value(pl, 5)) > 0) {
                sh->ailment |= 4;
                sh->ailment_val = val;
            }
            if ((val = Get_atk_value(pl, 6)) > 0) {
                sh->ailment |= 1;
                sh->ailment_val = val;
            }
        }
    }
    if (sh->x60 != 0) {
        a = flAbs(pl->blend0 % 100);
        sh->x60 += (u8)(a - (s32)(flAbs(pl->act_tm0) / 2.0f) - 1);
        if ((s8)sh->x60 < 0) {
            sh->x60 = 0;
        }
    }
}

static void cont_add(SHLW *sh) {
    PLW *pl = &player_work[sh->em_no];

    if (pl->x40A != 0 || pl->x610 != 0) {
        sh->x61++;
    }
}

static void shell00_m(SHLW *sh) {
    PLW *pl = &player_work[sh->em_no];

    cont_add(sh);
    switch (sh->arg) {
    case 8:
        if (pl->char0 == sh->char0 || pl->char0 == 0x57F) {
            return;
        }
        goto reset;
    case 0xB:
        if (--sh->x7E > 0) {
            break;
        }
        goto reset;
    case 0x13:
        if (pl->char0 != sh->char0) {
            goto reset;
        }
        sh->x61 = 99;
        break;
    default:
        if (pl->char0 == sh->char0) {
            break;
        }
    reset:
        sh->xB = 0;
        sh->mode = 2;
        break;
    }
}

static void shell00_d(SHLW *sh) {
    sh->mode = 6;
    sh->be_flag = 0;
}

static void shell00_e(SHLW *sh) {
    push_shell_work(sh);
}
