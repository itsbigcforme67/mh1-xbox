/* shell06_r01 - near-match fix: shell06_move_sub (extra case 0 label). Whole file in shell06_nm.c. 0x0062B160-0x0062BD9C: shell06_move_sub. Whole file in shell06_nm.c. */
#include "shell06.h"
#include "game.h"
#include "fl.h"
#include "clay.h"

typedef struct MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} MDLW;


extern s32 shell06_flag_tbl[];
extern s32 shell06_body_tbl[];
extern u8 shell06_tbl[4];
extern u16 sleeve_param_tbl0[3];
extern MDLW *eft_mdlw[5];
extern u32 col_type_tbl[];
extern SH06SPLIT split_param_tbl[];

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

int PachingerCamChk(void);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flvecNormalize(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void AddVector(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void flmatInvert(FLMAT *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
void flmatRotZ33(FLMAT *, f32);
void Material_set_sub(void *, CLAY *);
f32 GetGroundShellHit(VEC3 *);
void shell_rate_add(SHLW *);
void shell_rate_add_g(SHLW *);
void eft14_set(f32 *pos, s16 arg, f32 scale);
void eft12_set_sh(SHLW *, int, int);
void Shell09_set_pl2(PLW *pl, f32 *pos, int arg, u8 stg);
void Eft18_set3(SHLW *sh, s16 arg, int x07);
void Eft18_set(PLW *pl, s16 joint, s16 arg);
void Eft18_set4(PLW *pl, s16 joint, s16 arg, int x07);
void shell06_set_sub(PLW *pl, u8 arg, int joint);
void pl_atck_data_set_shl(SHLW *, void *, int, u8 *);
FLMAT *get_joint_wmat(PLW *, s16);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
void flvecRotX(f32 *, f32);
void flvecRotY(f32 *, f32);
void flvecCopy(void *, void *);
u32 ran_suu(int);










void shell06_move_sub(SHLW *sh) {
    f32 wb[3];
    f32 t1[3];
    f32 d1[3];
    f32 t2[3];
    f32 d[3];
    f32 v[3];
    f32 r[3];
    FLMAT m;
    SH06W *w = SH06_W(sh);
    SH06P *p = &shell06_param_tbl[sh->arg];
    f32 len;
    s32 k;

    if (sh->x06 > 0) {
        switch (p->special) {
        case 1:
            if (sh->char0 >= p->special_arg) {
                sh->mode++;
                goto eft;
            }
            if (sh->x9C == 0) {
                sh->mode++;
                goto eft;
            }
            if (sh->x9C->x04 == 3 || sh->x9C->be_flag == 0) {
                sh->mode++;
                goto eft;
            }
            flmatCopy(&m, get_joint_wmat((PLW *)sh->x9C, sh->x7E));
            flmatGetTrans(t1, &m);
            flvecApplyMat33(d1, sh->rate, &m);
            sh->pos2.x = d1[0] + t1[0];
            sh->pos2.y = d1[1] + t1[1];
            sh->pos2.z = d1[2] + t1[2];
            goto eft;
        case 3:
        case 4:
        case 5:
            if (sh->char0 >= p->special_arg) {
                if (sh->stg == game_w.stage) {
                    eft14_set(&sh->pos2.x, 0, 1.0f);
                }
                switch (p->special) {
                case 3:
                    Shell09_set_pl2(sh->owner, &sh->pos2.x, 8, sh->stg);
                    break;
                case 4:
                    Shell09_set_pl2(sh->owner, &sh->pos2.x, 0xE, sh->stg);
                    break;
                case 5:
                    Shell09_set_pl2(sh->owner, &sh->pos2.x, 0xF, sh->stg);
                    break;
                }
                sh->mode++;
                goto eft;
            }
            if (sh->x9C == 0) {
                sh->mode++;
                goto eft;
            }
            if (sh->x9C->x04 == 3 || sh->x9C->be_flag == 0) {
                sh->mode++;
                goto eft;
            }
            flmatCopy(&m, get_joint_wmat((PLW *)sh->x9C, sh->x7E));
            flmatGetTrans(t1, &m);
            flvecApplyMat33(d1, sh->rate, &m);
            sh->pos2.x = d1[0] + t1[0];
            sh->pos2.y = d1[1] + t1[1];
            sh->pos2.z = d1[2] + t1[2];
            goto eft;
        case 8:
            if (w->x03 != 0 && --w->x03 == 0) {
                pl_atck_data_set_shl(sh, sh->owner, sh->arg + 1, shell06_tbl);
                shell06_change_atck_data(sh);
                sh->x88 = shell06_body_tbl[0];
                sh->x8C = 0;
                sh->rate[0] *= 0.7f;
                sh->rate[1] *= 0.7f;
                sh->rate[2] *= 0.7f;
                shell06_atck_data_calc(sh);
            }
            break;
        case 6:
            switch (w->x02) {
            case 0:
                if (sh->x9C == 0) {
                    sh->mode = 2;
                    sh->xB = 0;
                    goto eft;
                }
                if (sh->xA0 == 0 || sh->x9C->be_flag == 0) {
                    sh->mode = 2;
                    sh->xB = 0;
                    goto eft;
                }
                switch (SH06_JNT(sh)->type) {
                case 0:
                    v[0] = SH06_JNT(sh)->pos[0];
                    v[1] = SH06_JNT(sh)->pos[1];
                    v[2] = SH06_JNT(sh)->pos[2];
                    break;
                case 1:
                    v[0] = 0.5f * (SH06_JNT(sh)->pos[0] + SH06_JNT(sh)->pos2[0]);
                    v[1] = 0.5f * (SH06_JNT(sh)->pos[1] + SH06_JNT(sh)->pos2[1]);
                    v[2] = 0.5f * (SH06_JNT(sh)->pos[2] + SH06_JNT(sh)->pos2[2]);
                    break;
                }
                flmatCopy(&m, get_joint_wmat((PLW *)sh->x9C, SH06_JNT(sh)->joint));
                flvecApplyMat33_2(v, &m);
                flmatGetTrans(t2, &m);
                v[0] += t2[0];
                v[1] += t2[1];
                v[2] += t2[2];
                d[0] = sh->pos2.x - v[0];
                d[1] = sh->pos2.y - v[1];
                d[2] = sh->pos2.z - v[2];
                flvecNormalize(d);
                flvecCopy(r, sh->rate);
                r[0] *= 0.8f;
                r[1] *= 0.8f;
                r[2] *= 0.8f;
                len = -2.0f * flvecInnerProduct(r, d);
                d[0] *= len;
                d[1] *= len;
                d[2] *= len;
                sh->rate[0] += d[0];
                sh->rate[1] += d[1];
                sh->rate[2] += d[2];
                len = flSqrt(sh->rate[0] * sh->rate[0] + sh->rate[2] * sh->rate[2]);
                k = (u8)calc_rand(sh->xB8, sh->x7E, 0x14D, 2) - 0x80;
                len = 65536.0f * flArcTan2(sh->rate[1], len) / 6.2831855f;
                sh->ang[0] = (u16)(s32)(0.5f + len) + k;
                sh->ang[1] = (u16)(s32)(0.5f + 65536.0f * flArcTan2(sh->rate[0], sh->rate[2]) / 6.2831855f) + ((u8)calc_rand(sh->xB8, sh->x7E, 0x845, 5) - 0x80);
                w->x02++;
            case 1:
                if (w->x03 != 0 && --w->x03 == 0) {
                    pl_atck_data_set_shl(sh, sh->owner, sh->arg + 1, shell06_tbl);
                    shell06_change_atck_data(sh);
                    sh->x88 = shell06_body_tbl[0];
                    sh->x8C = 0;
                }
                break;
            }
            break;
        }
    }
    flvecCopy(&sh->pos0, &sh->pos2);
    switch (p->kind) {
    default:
    case 0:
    case 1:
        len = flSqrt(sh->rate[0] * sh->rate[0] + sh->rate[2] * sh->rate[2]);
        wb[1] = p->wobble_y * len * rand_sub(calc_rand(sh->xB8, sh->char0, 0x147F, 2), 14);
        if (sh->char0 < 20 && calc_rand(sh->xB8, sh->char0, 0x3045, 1) < 0x2000) {
            len = w->grow->wobble;
            if (w->ammo & 0x10) {
                len += Silencer_Grow_Up_Tbl.wobble;
            }
            if (w->ammo & 0x20) {
                len += LBarrel_Grow_Up_Tbl.wobble;
            }
            len *= p->wobble_x;
            wb[0] = len * rand_sub2(calc_rand(sh->xB8, sh->char0, 0x3121, 4), 6);
        } else {
            wb[0] = 0.0f;
        }
        wb[2] = 0.0f;
        flvecRotX(wb, DEG2RAD(ANG2DEG(sh->ang[0])));
        flvecRotY(wb, DEG2RAD(ANG2DEG(sh->ang[1])));
        AddVector(sh->rate, sh->rate, wb);
        break;
    case 4:
        if (sh->char0 >= 0x17 || w->state == 0xFF) {
            sh->x61 = 0;
            sh->mode++;
            goto eft;
        }
        if (sh->char0 > 2 && (sh->char0 & 3) == 0) {
            w->x03++;
            if (w->x03 <= p->special_arg) {
                pl_atck_data_set_shl(sh, sh->owner, sh->arg + 1, shell06_tbl);
                shell06_change_atck_data(sh);
                sh->x88 = shell06_body_tbl[w->x03];
                sh->x8C = 0;
            }
        }
        break;
    case 2:
        break;
    }
    if (sh->char0 < p->grav_time) {
        shell_rate_add(sh);
    } else {
        shell_rate_add_g(sh);
    }
    sh->x61 = 0x63;
    if (p->kind != 2 || sh->char0 >= 10) {
        len = GetGroundShellHit(&sh->pos2);
        if (p->kind == 4) {
            len -= 100.0f;
        }
        if (sh->pos2.y <= len) {
            sh->x61 = 0;
            sh->mode++;
            switch (p->special) {
            case 7:
                shell06_set_split(sh, &split_param_tbl[p->special_arg]);
                break;
            case 2:
                Shell09_set_pl2(sh->owner, &sh->pos2.x, 7, sh->stg);
                if (sh->stg == game_w.stage) {
                    eft14_set(&sh->pos2.x, 0, 1.0f);
                }
                break;
            case 10:
                eft12_set_sh(sh, 1, 1);
                break;
            }
        }
    }
    if (shell06_time_ck(sh)) {
        shell06_atck_data_calc(sh);
    }
eft:
    switch (w->x16) {
    case 0:
        if (sh->char0 > sleeve_param_tbl0[p->split] || sh->x06 > 0) {
            w->x16++;
            if (sh->stg == game_w.stage) {
                Eft18_set3(sh, 4, 0);
            }
        }
        break;
    case 1:
        w->x16++;
        break;
    }
    if (p->flash != 0) {
        switch (w->x17) {
        case 0:
            w->x17++;
            shell06_eft_i(sh);
            break;
        case 1:
            if (sh->x06 > 0) {
                w->x17++;
            } else {
                shell06_eft_m(sh);
            }
            break;
        }
    }
}
