/* shell06 - game.bin 0x0062A6C0-0x0062B154: shell06_set to shell06_init_sub.
 * Bowgun shots. shell06_set fires one shot from the player's muzzle joint
 * (plus a muzzle effect picked by the ammo type); shell06_set_split spawns
 * the extra pellets of a scattered shot. shell06_init_sub aims the shot
 * along the joint's matrix and sets its speed and drop from the gun's
 * growth row and the ammo row (shell06_param_tbl). */
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

void shell06_set(PLW *pl, int unused, int joint) {
    if (pl->id != game_w.master || PachingerCamChk() == 0) {
        switch (pl->ammo_type) {
        default:
            Eft18_set(pl, (s16)joint, 0);
            Eft18_set(pl, (s16)joint, 1);
            Eft18_set(pl, (s16)joint, 2);
            break;
        case 0x11:
        case 0x12:
            Eft18_set4(pl, (s16)joint, 5, 0);
            Eft18_set4(pl, (s16)joint, 6, 0);
            break;
        case 0x17:
        case 0x18:
        case 0x19:
            Eft18_set4(pl, (s16)joint, 5, 3);
            Eft18_set4(pl, (s16)joint, 6, 3);
            break;
        case 0x13:
        case 0x14:
            Eft18_set4(pl, (s16)joint, 5, 1);
            Eft18_set4(pl, (s16)joint, 6, 1);
            break;
        case 0x15:
        case 0x16:
            Eft18_set4(pl, (s16)joint, 5, 2);
            Eft18_set4(pl, (s16)joint, 6, 2);
            break;
        case 0x1A:
            Eft18_set4(pl, (s16)joint, 5, 8);
            Eft18_set4(pl, (s16)joint, 6, 8);
            break;
        case 0x1B:
            Eft18_set4(pl, (s16)joint, 5, 5);
            Eft18_set4(pl, (s16)joint, 6, 5);
            break;
        case 0x1C:
            Eft18_set4(pl, (s16)joint, 5, 6);
            Eft18_set4(pl, (s16)joint, 6, 6);
            break;
        case 0x1D:
            Eft18_set4(pl, (s16)joint, 5, 7);
            Eft18_set4(pl, (s16)joint, 6, 7);
            break;
        case 0x1E:
            Eft18_set4(pl, (s16)joint, 5, 9);
            Eft18_set4(pl, (s16)joint, 6, 9);
            break;
        }
    }
    shell06_set_sub(pl, pl->ammo_type, (s16)joint);
}

void shell06_set_sub(PLW *pl, u8 arg, int joint) {
    SHLW *sh;

    if ((sh = pull_shell_work(1)) != 0) {
        sh->type = 6;
        sh->arg = arg;
        sh->x7E = joint;
        sh->move = shell06_move;
        sh->em_no = pl->id;
        sh->x7A = pl->x10;
        sh->owner = pl;
        sh->xC8 = pl->ang[1];
        sh->xB8 = pl->cnt39A;
        sh->x07 = 0;
        sh->stg = pl->stg;
    }
}


void shell06_set_split(SHLW *sh, SH06SPLIT *sp) {
    SHLW *nw;
    s16 i;
    u8 arg = sp->arg;
    s16 num = sp->num;

    shell06_se_req(sh, 0);
    for (i = 0; i < num; i++) {
        if ((nw = pull_shell_work(1)) != 0) {
            nw->type = 6;
            nw->arg = arg;
            nw->x7E = sh->x7E;
            nw->move = shell06_move;
            nw->em_no = ((PLW *)sh->owner)->id;
            nw->x7A = ((PLW *)sh->owner)->x10;
            nw->owner = sh->owner;
            nw->xC8 = sh->xC8;
            nw->ang[0] = sh->ang[0];
            nw->ang[1] = sh->ang[1];
            nw->pos2.x = sh->pos2.x;
            nw->pos2.y = sh->pos2.y;
            nw->pos2.z = sh->pos2.z;
            nw->rate[0] = sh->rate[0];
            nw->rate[1] = sh->rate[1];
            nw->rate[2] = sh->rate[2];
            nw->xB8 = sh->xB8 * (i * 2 + 1);
            nw->x07 = i;
            nw->stg = sh->stg;
        }
    }
}

void shell06_init_sub(SHLW *sh) {
    f32 v[3];
    FLMAT m;
    SH06W *w = SH06_W(sh);
    SH06P *p = &shell06_param_tbl[sh->arg];
    PLW *pl = sh->owner;
    f32 ax;
    f32 ay;
    f32 spd;
    f32 spread;
    f32 drop;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    sh->char0 = 0;
    shell_flag_set(sh, shell06_flag_tbl[sh->arg]);
    pl_atck_data_set_shl(sh, pl, sh->arg + 1, shell06_tbl);
    if (sh->arg != 15) {
        shell06_get_weaopn_data(w, pl);
        shell06_change_atck_data(sh);
        spd = w->grow->spd;
        spread = w->grow->spread;
        drop = w->grow->drop;
        if (w->ammo & 0x10) {
            spd += Silencer_Grow_Up_Tbl.spd;
            spread += Silencer_Grow_Up_Tbl.spread;
            drop += Silencer_Grow_Up_Tbl.drop;
        }
        if (w->ammo & 0x20) {
            spd += LBarrel_Grow_Up_Tbl.spd;
            spread += LBarrel_Grow_Up_Tbl.spread;
            drop += LBarrel_Grow_Up_Tbl.drop;
        }
        spd *= p->spd;
        spread *= p->spread;
        drop *= p->drop;
    } else {
        spd = p->spd;
        spread = p->spread;
        drop = p->drop;
    }
    w->atk = sh->x62;
    sh->x88 = shell06_body_tbl[sh->body];
    sh->x8C = 0;
    flmatCopy(&m, get_joint_wmat(pl, sh->x7E));
    sh->x05 = 0;
    sh->x06 = 0;
    w->x00 = 0;
    w->state = 0;
    w->x02 = 0;
    w->x03 = 0;
    switch (p->kind) {
    case 4:
    case 0:
    case 1:
    default:
        flmatGetTrans(&sh->pos2.x, &m);
        ax = flArcTan2(m[2][1], flSqrt(m[2][0] * m[2][0] + m[2][2] * m[2][2]));
        ay = 3.1415927f + flArcTan2(m[2][0], m[2][2]);
        sh->ang[0] = (u16)(s32)(0.5f + 65536.0f * ax / 6.2831855f);
        sh->ang[1] = (u16)(s32)(0.5f + 65536.0f * ay / 6.2831855f);
        v[0] = -10.0f;
        v[1] = -10.0f;
        v[2] = 87.0f;
        if (p->x01 == 0x62) {
            w->x00 = 1;
            sh->ang[2] = (u16)ran_suu(1);
            if (p->kind == 4) {
                sh->xB = 0;
            }
        }
        flvecRotX(v, ax);
        flvecRotY(v, ay);
        sh->pos2.x += v[0];
        sh->pos2.y += v[1];
        sh->pos2.z += v[2];
        sh->rate[0] = 0.0f;
        sh->rate[1] = 0.0f;
        sh->rate[2] = spd + spread * rand_sub(sh->xB8, 16);
        flvecRotX(sh->rate, ax);
        flvecRotY(sh->rate, ay);
        sh->rate_g[1] = -(p->grav + drop * rand_sub(sh->xB8 >> 1, 15));
        break;
    case 2:
        sh->rate[0] = spd + spread * rand_sub(sh->xB8 >> 2, 14);
        if ((sh->xB8 >> 3) & 1) {
            sh->rate[0] = -sh->rate[0];
        }
        sh->rate[1] = 2.0f * (spd + spread * rand_sub(sh->xB8 >> 4, 12));
        sh->rate[2] = spd + spread * rand_sub(sh->xB8 >> 5, 11);
        if ((sh->xB8 >> 6) & 1) {
            sh->rate[2] = -sh->rate[2];
        }
        sh->ang[0] = (u16)(s32)(0.5f + 65536.0f * flArcTan2(sh->rate[1], flSqrt(sh->rate[0] * sh->rate[0] + sh->rate[2] * sh->rate[2])) / 6.2831855f);
        sh->ang[1] = (u16)(s32)(0.5f + 65536.0f * flArcTan2(sh->rate[0], sh->rate[2]) / 6.2831855f) + 0x8000;
        sh->rate_g[1] = -(p->grav + drop * rand_sub(sh->xB8 >> 7, 9));
        break;
    }
    flvecCopy(&sh->pos0, &sh->pos2);
    flvecCopy(&w->pos, &sh->pos2);
    shell06_se_req(sh, 1);
    if (p->split == 0xFF) {
        w->x16 = 2;
    } else {
        w->x16 = 0;
    }
    w->x17 = 0;
    if (p->time_no != -1) {
        shell06_atck_data_calc(sh);
    }
}


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

void shell06_hit(SHLW *sh) {
    f32 t[3];
    f32 d[3];
    f32 c[3];
    FLMAT m;
    FLMAT inv;
    SH06W *w = SH06_W(sh);
    SH06P *p = &shell06_param_tbl[sh->arg];

    w->x02 = 0;
    switch (w->x16) {
    case 0:
        w->x16++;
        if (sh->stg == game_w.stage) {
            Eft18_set3(sh, 4, 0);
        }
        break;
    case 1:
        w->x16++;
        break;
    }
    switch (p->special) {
    default:
        sh->mode = 2;
        break;
    case 1:
    case 3:
    case 4:
    case 5:
        sh->char0 = 0;
        sh->x06++;
        sh->xB = 0;
        w->x00++;
        if (sh->x9C == 0) {
            sh->mode = 2;
            return;
        }
        if (sh->xA0 == 0 || sh->x9C->be_flag == 0) {
            sh->mode = 2;
            return;
        }
        sh->x7E = SH06_JNT(sh)->joint;
        flmatCopy(&m, get_joint_wmat((PLW *)sh->x9C, sh->x7E));
        flmatGetTrans(t, &m);
        AddVector(c, &sh->pos2.x, &sh->pos0.x);
        ScaleVector(c, c, 0.5f);
        d[0] = c[0] - t[0];
        d[1] = c[1] - t[1];
        d[2] = c[2] - t[2];
        flmatInvert(&inv, &m);
        flvecApplyMat33(sh->rate, d, &inv);
        sh->mode = 1;
        break;
    case 8:
        if (++sh->x06 < p->special_arg) {
            w->x03 = 2;
        } else {
            sh->xB = 0;
        }
        sh->mode = 1;
        goto move;
    case 7:
        shell06_set_split(sh, &split_param_tbl[p->special_arg]);
        sh->xB = 0;
        sh->mode = 2;
        break;
    case 6:
        if (++sh->x06 < p->special_arg) {
            w->x03 = 4;
        } else {
            sh->xB = 0;
        }
        sh->mode = 1;
        goto move;
    case 9:
        sh->mode = 1;
        goto move;
    case 10:
        eft12_set_sh(sh, 1, 1);
        sh->xB = 0;
        sh->mode = 2;
        break;
    }
    goto draw;
move:
    if (++sh->char0 > 0xFF) {
        sh->x61 = 0;
        sh->mode = 2;
        return;
    }
    shell06_move_sub(sh);
draw:
    if (sh->prim != 0) {
        if (p->x01 == 0x62) {
            flvecCopy(sh->prim->pos, &w->pos);
        } else {
            flvecCopy(sh->prim->pos, &sh->pos2);
        }
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

void shell06_trans_sub(PRIM *pr) {
    FLMAT m;
    FLMAT m0;
    SHLW *sh = pr->owner;
    SH06W *w = SH06_W(sh);
    SH06P *p = &shell06_param_tbl[sh->arg];
    MDLW *md;
    void *mat;
    CLAY *cl;
    f32 ax, ay;

    if (sh->stg != game_w.stage) {
        return;
    }
    switch (w->x16) {
    case 0:
        sleeve_trans(sh);
        return;
    case 1:
        sleeve_trans(sh);
        break;
    }
    if (p->x01 != 0xFF && (md = eft_mdlw[0]) != 0 && md->flag != 0) {
        mat = md->mat;
        if (p->x01 == 0x62) {
            if (sh->char0 >= 15) {
                return;
            }
            flmatMakeTrans(&m0, 0.5f, 0.375f, 0.0f);
            flSetRenderState(0x19, (u32)&m0);
            flmatMakeScale(&m, 2.25f, 2.25f, 2.25f);
            cl = &md->clay[p->x01] + sh->char0;
            flmatRotZ33(&m, DEG2RAD(ANG2DEG(sh->ang[2])));
        } else {
            flmatInit(&m);
            cl = &md->clay[p->x01];
        }
        if (w->x00 == 0) {
            ax = flArcTan2(-sh->rate[1], flSqrt(sh->rate[0] * sh->rate[0] + sh->rate[2] * sh->rate[2]));
            ay = flArcTan2(sh->rate[0], sh->rate[2]);
            sh->ang[0] = (u16)(s32)(0.5f + 65536.0f * ax / 6.2831855f);
            sh->ang[1] = (u16)(s32)(0.5f + 65536.0f * ay / 6.2831855f);
        }
        flmatRotX33(&m, DEG2RAD(ANG2DEG(sh->ang[0])));
        flmatRotY33(&m, DEG2RAD(ANG2DEG(sh->ang[1])));
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flSetRenderState(0x67, col_type_tbl[p->col]);
        flSetRenderState(0x1A, (u32)&m);
        flSetRenderState(0x60, 0x80);
        if (cl != 0 && cl->handle != -1) {
            Material_set_sub(mat, cl);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
