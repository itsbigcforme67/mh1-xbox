/* shell03 - game.bin 0x00628FB0-0x00629B64. Items thrown by a player
 * (item_chr_tbl picks the model): they fly from the player's hand under
 * gravity and burst on landing (arg 1 and 9 burst on a timer instead).
 * Arg 4 is a placed bomb: it sits at the player's feet, blinks faster and
 * faster, and explodes after 75 frames. */
#include "shell.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

/* One entry of a model set's material table (0x4C bytes). */
typedef struct MATERIAL {
    u8 _pad00[4];
    f32 col[3];         /* 0x04 */
    u8 _pad10[0x4C - 0x10];
} MATERIAL;

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    MATERIAL *mat;      /* 0x10 */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern u8 shell03_tbl[4];
extern s32 shell03_body_tbl[2];
extern s8 shell03_atk_sel_tbl[11];
extern f32 rate_tbl1[3][2];
extern f32 rate_tbl2[3][2];
extern s16 item_chr_tbl_0067ADC0[11];

u8 Pl_stg_ck(PLW *);
void pl_atck_data_set_shl(SHLW *, void *, int, u8 *);
void flvecCopy(void *, void *);
void flvecApplyMat33(f32 *, f32 *, void *);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
int get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim(s16);
void release_prim2(s16);
void shell_rate_add_g(SHLW *);
f32 GetGroundShellHit(VEC3 *);
void Eft14_set3(VEC3 *, int, PLW *, f32);
void Eft15_set2(VEC3 *, int, int, f32);
void Eft16_set_ex3(f32 *, int, int, int, f32);
void Eft12_set3(VEC3 *, int, int, int);
void eft12_set_sh(SHLW *, int, int);
void eft14_set(VEC3 *, int, f32);
void Eft02_set2(int, int, int, VEC3 *);
void Shell09_set(VEC3 *, int, int);
void Shell09_set_pl(void *, VEC3 *, int);
void se_req2(int, int, int, f32 *, int, int);

static void shell03_move(SHLW *sh);
static void shell03_i(SHLW *sh);
static void shell03_i00(SHLW *sh);
static void shell03_i01(SHLW *sh);
static void shell03_m(SHLW *sh);
static void shell03_m00(SHLW *sh);
static void shell03_m01(SHLW *sh);
static void shell03_d(SHLW *sh);
static void shell03_e(SHLW *sh);
static void shell03_trans(PRIM *pr);

void shell03_set(PLW *pl, int arg, int x07) {
    SHLW *sh;

    if (arg == 4 || Pl_stg_ck(pl) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 3;
            sh->arg = arg;
            sh->x07 = x07;
            sh->move = shell03_move;
            sh->em_no = pl->id;
            sh->x7A = pl->x10;
            sh->char0 = 0;
            sh->owner = pl;
            sh->xC8 = pl->ang[1];
            sh->ang[0] = pl->ang[0];
            sh->ang[1] = pl->ang[1];
            sh->ang[2] = pl->ang[2];
            sh->stg = pl->stg;
            if (arg == 4) {
                sh->pos2.x = pl->pos[0];
                sh->pos2.y = 21.0f + pl->pos[1];
                sh->pos2.z = pl->pos[2];
                sh->x09 = 1;
            } else {
                sh->pos2.x = pl->hand->pos[0];
                sh->pos2.y = pl->hand->pos[1];
                sh->pos2.z = pl->hand->pos[2];
                sh->x09 = 0;
            }
        }
    }
}

static void shell03_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell03_i(sh);
        break;
    case 1:
        shell03_m(sh);
        break;
    case 2:
        shell03_d(sh);
        break;
    case 3:
        shell03_d(sh);
        break;
    case 4:
        shell03_d(sh);
        break;
    case 5:
        shell03_d(sh);
        break;
    case 7:
        shell03_d(sh);
        break;
    case 6:
        shell03_e(sh);
        break;
    }
}

static void shell03_i(SHLW *sh) {
    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    switch (sh->arg) {
    case 4:
        shell03_i01(sh);
        sh->prim_no = get_prim2();
        if (sh->prim_no != -1) {
            sh->prim = get_prim_ptr2(sh->prim_no);
            sh->prim->owner = sh;
            sh->prim->trans = shell03_trans;
            shell03_m(sh);
        } else {
            push_shell_work(sh);
        }
        break;
    default:
        shell03_i00(sh);
        sh->prim_no = get_prim();
        if (sh->prim_no != -1) {
            sh->prim = get_prim_ptr(sh->prim_no);
            sh->prim->owner = sh;
            sh->prim->trans = shell03_trans;
            shell03_m(sh);
        } else {
            push_shell_work(sh);
        }
        break;
    }
}

static void shell03_i00(SHLW *sh) {
    PLW *pl = sh->owner;
    f32 (*tbl)[2];
    f32 v[3];
    f32 out[3];

    shell_flag_set(sh, 0);
    pl_atck_data_set_shl(sh, pl, shell03_atk_sel_tbl[sh->arg], shell03_tbl);
    sh->x88 = shell03_body_tbl[0];
    sh->x8C = 0;
    sh->stg = pl->stg;
    if (sh->arg != 6 && sh->arg != 8 && sh->arg != 10) {
        sh->xB = 0;
    }
    if (pl->char0 == 0x1A1) {
        tbl = rate_tbl1;
    } else {
        tbl = rate_tbl2;
    }
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = tbl[sh->x07][0];
    flvecApplyMat33(out, v, pl->rot);
    sh->rate[0] = 2.0f * out[0];
    sh->rate[2] = 2.0f * out[2];
    sh->rate_g[0] = 0.0f;
    sh->rate_g[2] = 0.0f;
    sh->rate[1] = 2.0f * tbl[sh->x07][1];
    sh->rate_g[1] = -0.8f;
}

static void shell03_i01(SHLW *sh) {
    PLW *pl = sh->owner;
    f32 v[3];

    shell_flag_set(sh, 0);
    pl_atck_data_set_shl(sh, pl, shell03_atk_sel_tbl[sh->arg], shell03_tbl);
    sh->x88 = 0;
    sh->x8C = shell03_body_tbl[0];
    sh->xB = 0;
    sh->x06 = 15;
    sh->x07 = 0;
    sh->x7E = 75;
    flvecCopy(v, &sh->pos2);
    if (sh->stg == game_w.stage) {
        Eft16_set_ex3(v, 0xD, 0x4B, 0xA, 1.0f);
    }
}

static void shell03_m(SHLW *sh) {
    switch (sh->arg) {
    case 4:
        shell03_m01(sh);
        break;
    default:
        shell03_m00(sh);
        break;
    }
    if (sh->prim != 0) {
        flvecCopy(sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell03_m00(SHLW *sh) {
    PLW *pl = sh->owner;
    f32 h;

    shell_rate_add_g(sh);
    if (sh->arg == 1) {
        sh->x61 = 0;
    } else {
        sh->x61 = 99;
    }
    h = GetGroundShellHit(&sh->pos2);
    sh->char0++;
    if (sh->arg == 1) {
        if (sh->char0 >= 13) {
            if (Pl_stg_ck(pl) != 0) {
                Eft14_set3(&sh->pos2, 4, pl, 1.0f);
                se_req2(1, 0x47, 0, &sh->pos2.x, 4, 0);
            }
            sh->mode++;
            return;
        }
    } else if (sh->arg == 9) {
        if (sh->char0 >= 16) {
            Shell09_set_pl(sh->owner, &sh->pos2, 13);
            if (sh->stg == game_w.stage) {
                Eft15_set2(&sh->pos2, 4, 0, 0.5f);
            }
            sh->mode++;
            return;
        }
    }
    if (sh->pos2.y <= h) {
        sh->pos2.y = h;
        switch (sh->arg) {
        case 0:
            eft12_set_sh(sh, 1, 0);
            break;
        case 5:
            eft12_set_sh(sh, 1, 1);
            break;
        case 7:
            if (sh->stg == game_w.stage) {
                Eft12_set3(&sh->pos2, 0, 5, 2);
            }
            break;
        }
        sh->mode++;
    }
}

static void shell03_m01(SHLW *sh) {
    if (++sh->char0 >= 75) {
        if (sh->stg == game_w.stage) {
            eft14_set(&sh->pos2, 0, 1.0f);
        }
        Shell09_set(&sh->pos2, 0, sh->stg);
        sh->be_flag ^= 1;
        sh->mode++;
        return;
    }
    if (sh->char0 >= 55) {
        sh->be_flag ^= 1;
        return;
    }
    if (sh->x07 == 0) {
        if (0.8f * sh->x7E > (f32)(55 - sh->char0)) {
            sh->be_flag ^= 1;
            if (55 - sh->char0 > 2) {
                sh->x07 = 2;
            }
        }
    } else if (--sh->x07 == 0) {
        sh->be_flag ^= 1;
        sh->x7E = 55 - sh->char0;
    }
    if (sh->stg == game_w.stage) {
        if (sh->char0 == 1) {
            se_req2(1, 0x24, 0, &sh->pos2.x, 1, 0);
        }
        if (--sh->x06 <= 0) {
            sh->x06 = 15;
            se_req2(1, 0x25, 0, &sh->pos2.x, 1, 0);
        }
        if (sh->char0 & 1) {
            Eft02_set2(0xC000, 0, 2, &sh->pos2);
        }
    }
}

static void shell03_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        if (sh->arg == 4) {
            release_prim2(sh->prim_no);
        } else {
            release_prim(sh->prim_no);
        }
    }
}

static void shell03_e(SHLW *sh) {
    push_shell_work(sh);
}

static void shell03_trans(PRIM *pr) {
    FLMAT mat;
    SHLW *sh = pr->owner;
    EFT_MDLW *mw = eft_mdlw[0];
    MATERIAL *mt;
    MATERIAL *m;
    CLAY *cl;
    s32 i;

    if (sh->stg == game_w.stage && mw != 0 && mw->flag != 0) {
        mt = mw->mat;
        flmatMakeTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        if (sh->arg != 4) {
            flmatRotXYZ33(&mat, DEG2RAD(ANG2DEG((s32)(u16)(sh->char0 << 11))), 0.0f, 0.0f);
        }
        cl = &mw->clay[item_chr_tbl_0067ADC0[sh->arg]];
        flSetRenderState(0x1A, (u32)&mat);
        if (cl != 0 && cl->handle != -1) {
            for (i = 0; i < cl->mat_num; i++) {
                m = &mt[cl->mat_no[i]];
                m->col[0] = 1.0f;
                m->col[1] = 1.0f;
                m->col[2] = 1.0f;
                flSetRenderState((u8)(i + 0x3A), (u32)m);
            }
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
