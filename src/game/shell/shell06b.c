/* shell06 - game.bin 0x0062BDA0-0x0062D4C4: shell06_hit to shell06_eft_t.
 * shell06_move_sub (0x62B160) is still assembly; near-match in shell06_nm.c.
 * Bowgun shots: hits, the mode machine, damage fall-off with range, firing
 * sounds, the sleeve (cartridge) model and the muzzle flash. */
#include "shell06.h"
#include "game.h"
#include "clay.h"
#include "fl.h"

typedef struct MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern u8 shell06_tbl[4];
extern SH06SPLIT split_param_tbl[];
extern u32 col_type_tbl[];
extern s16 *shell06_change_time[];
extern s16 shell06_hit_mark1[];
extern s16 shell06_hit_mark2[];
extern s16 shell06_sound_type[];
extern f32 shell06_atk_rate[];
extern f32 scale64_0067D2D0[][3];
extern f32 scale66_1_0067D2F0[][3];
extern f32 scale66_2_0067D310[][3];

FLMAT *get_joint_wmat(PLW *, s16);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatInvert(FLMAT *, FLMAT *);
void flmatRotZ33(FLMAT *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void AddVector(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
void eft12_set_sh(SHLW *, int, int);
void Eft18_set3(SHLW *sh, s16 arg, int x07);
void release_prim(s16);
void flvecCopy(void *, void *);
void flvecNormalize(f32 *);
void flmatRotX33(FLMAT *, f32);
void RotateZ(FLMAT *, f32);
void Material_set_sub(void *, CLAY *);
void SetTrnslMode(int, int);
void se_req2(int, int, int, VEC3 *, int, int);
int Pl_silencer_ck(void *);
void Pl_se_req2(void *, int, int, VEC3 *, int, int);
void Eft18_set2(f32 *, s16, int);

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

void shell06_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell06_i(sh);
        break;
    case 1:
        shell06_m(sh);
        break;
    case 2:
        shell06_d(sh);
        break;
    case 3:
        shell06_m(sh);
        break;
    case 4:
        shell06_d(sh);
        break;
    case 5:
        shell06_d(sh);
        break;
    case 7:
        shell06_d(sh);
        break;
    case 6:
        shell06_e(sh);
        break;
    }
}

void shell06_i(SHLW *sh) {
    shell06_init_sub(sh);
    sh->prim_no = get_prim();
    if (sh->prim_no != -1) {
        sh->prim = get_prim_ptr(sh->prim_no);
        sh->prim->owner = sh;
        sh->prim->trans = shell06_trans;
        flvecCopy(sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    } else {
        push_shell_work(sh);
    }
}

void shell06_m(SHLW *sh) {
    SH06W *w = SH06_W(sh);
    SH06P *p = &shell06_param_tbl[sh->arg];

    if (sh->mode == 3) {
        shell06_hit(sh);
    } else if (++sh->char0 > 0xFF) {
        sh->x61 = 0;
        sh->mode = 2;
    } else {
        shell06_move_sub(sh);
        if (sh->prim != 0) {
            if (p->x01 == 0x62) {
                flvecCopy(sh->prim->pos, &w->pos);
            } else {
                flvecCopy(sh->prim->pos, &sh->pos2);
            }
            add_prim(ot1, sh->prim, 0x20, 0);
        }
    }
}

void shell06_d(SHLW *sh) {
    SH06W *w = SH06_W(sh);

    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
    if (w->x17 > 0) {
        shell06_eft_d(sh);
    }
}

void shell06_e(SHLW *sh) {
    push_shell_work(sh);
}

void shell06_trans(PRIM *pr) {
    SHLW *sh = pr->owner;

    if (sh->stg == game_w.stage) {
        shell06_trans_sub(pr);
    }
}

void shell06_get_weaopn_data(SH06W *w, PLW *pl) {
    w->atk_rate = pl->atk_rate;
    w->ammo = pl->wpn_ammo;
    w->grow = Gun_Grow_Up_DATA[Gun_data[pl->wpn_kind].grow_no] + (w->ammo & 0xF);
}

void shell06_change_atck_data(SHLW *sh) {
    SH06W *w = SH06_W(sh);
    u16 v;

    if (sh->x63 != 8 && sh->x62 != 0) {
        v = sh->x62 * w->atk_rate;
        if (v == 0) {
            v = 1;
        } else if (v > 0xFF) {
            v = 0xFF;
        }
        sh->x62 = v;
    }
}

int shell06_time_ck(SHLW *sh) {
    SH06W *w = SH06_W(sh);
    SH06P *p = &shell06_param_tbl[sh->arg];
    s16 no = p->time_no;
    int ret = 0;
    s16 *tbl;
    f32 r;
    s16 t;

    if (no == -1) {
        return ret;
    }
    tbl = shell06_change_time[no];
    r = w->grow->range;
    if (w->ammo & 0x10) {
        r += Silencer_Grow_Up_Tbl.range;
    }
    if (w->ammo & 0x20) {
        r += LBarrel_Grow_Up_Tbl.range;
    }
    while (w->state != 0xFF) {
        t = r * tbl[w->state];
        if (t >= sh->char0) {
            break;
        }
        ret = 1;
        if (t < 0) {
            w->state = 0xFF;
            sh->xB = 0;
            break;
        }
        w->state++;
    }
    return ret;
}

void shell06_atck_data_calc(SHLW *sh) {
    SH06W *w = SH06_W(sh);
    s16 *mark;
    f32 r;

    if (sh->x62 == 0 || w->state >= 5 || w->state == 0xFF) {
        return;
    }
    switch (sh->x6F) {
    case 9:
    case 11:
    case 13:
    case 15:
        mark = shell06_hit_mark1;
        break;
    case 10:
    case 12:
    case 14:
    case 16:
        mark = shell06_hit_mark2;
        break;
    default:
        return;
    }
    sh->x6F = mark[w->state];
    sh->x6E = shell06_sound_type[w->state];
    if (w->atk != 0.0f) {
        r = w->atk * shell06_atk_rate[w->state];
        if (r >= 255.0f) {
            sh->x62 = 0xFF;
        } else if (r <= 1.0f) {
            sh->x62 = 1;
        } else {
            sh->x62 = r;
        }
    }
}

void sleeve_trans(SHLW *sh) {
    FLMAT m;
    MDLW *md;
    void *mat;
    CLAY *cl;
    s16 i;

    md = eft_mdlw[0];
    if (md != 0 && md->flag != 0) {
        cl = &md->clay[89];
        flSetRenderState(0x60, 0x80);
        mat = md->mat;
        for (i = 0; i < 2; i++) {
            flmatInit(&m);
            flmatRotX33(&m, DEG2RAD(ANG2DEG(sh->ang[0])));
            flmatRotY33(&m, DEG2RAD(ANG2DEG(sh->ang[1])));
            flmatSetTrans(&m, sh->pos2.x, sh->pos2.y, sh->pos2.z);
            if (i != 0) {
                RotateZ(&m, 3.1415927f);
            }
            flSetRenderState(0x1A, (u32)&m);
            if (cl != 0 && cl->handle != -1) {
                Material_set_sub(mat, cl);
                clay_attr_set(cl->attr);
                flExecuteClay(cl->handle, 0);
            }
        }
        clay_attr_reset();
    }
}

u16 calc_rand(u16 a, u16 b, u16 c, u16 d) {
    return (a + b * c) >> d;
}

f32 rand_sub(u16 seed, u16 bits) {
    u16 s = 16 - bits;

    return 1.5625e-5f * (f32)((seed & (u16)(0xFFFF >> s)) << s);
}

f32 rand_sub2(u16 seed, u16 bits) {
    u16 s = 16 - bits;

    return 1.5625e-5f * (f32)(s16)(((seed & (u16)(0xFFFF >> s)) << s) - 0x8000);
}

void shell06_se_req(SHLW *sh, s16 kind) {
    s16 v;

    if (sh->stg != game_w.stage) {
        return;
    }
    switch (kind) {
    case 0:
        switch (sh->arg) {
        case 12:
        case 13:
        case 14:
            se_req2(1, 0x4E, 0, &sh->pos2, 1, 0);
            break;
        }
        break;
    case 1:
        if (sh->arg != 15) {
            v = Pl_silencer_ck(sh->owner) ? 5 : 0;
            switch (Shell_data[sh->arg].se_kind) {
            case 0:
                Pl_se_req2(sh->owner, v + 10, 0, &sh->pos2, 1, 0);
                break;
            case 1:
                Pl_se_req2(sh->owner, v + 11, 0, &sh->pos2, 1, 0);
                break;
            case 2:
                Pl_se_req2(sh->owner, v + 12, 0, &sh->pos2, 1, 0);
                break;
            }
        }
        break;
    }
}

void shell06_eft_i(SHLW *sh) {
    SH06W *w = SH06_W(sh);
    SH06E *e;

    w->x14 = 0;
    e = w->eft;
    e->prim_no = get_prim();
    if (e->prim_no != -1) {
        e->cnt = 0;
        e->x0A = 0;
        e->x06 = 0;
        e->prim = get_prim_ptr(e->prim_no);
        e->prim->owner = sh;
        e->prim->no = 0;
        e->prim->trans = shell06_eft_t;
    } else {
        e->prim = 0;
    }
    shell06_eft_m(sh);
}

void shell06_eft_m(SHLW *sh) {
    f32 v[3];
    SH06W *w = SH06_W(sh);
    SH06E *e = w->eft;

    flvecCopy(v, sh->rate);
    flvecNormalize(v);
    v[0] *= -25.0f;
    v[1] *= -25.0f;
    v[2] *= -25.0f;
    switch (w->state) {
    case 1:
        if (++w->x14 == 1 && sh->stg == game_w.stage) {
            Eft18_set2(&sh->pos2.x, 3, 0);
        }
        break;
    }
    e->cnt++;
    if (e->prim != 0) {
        e->prim->pos[0] = sh->pos2.x + v[0];
        e->prim->pos[1] = sh->pos2.y + v[1];
        e->prim->pos[2] = sh->pos2.z + v[2];
        add_prim(ot0, e->prim, 0x40, 0);
    }
}

void shell06_eft_d(SHLW *sh) {
    SH06E *e = SH06_W(sh)->eft;

    if (e->prim != 0) {
        release_prim(e->prim_no);
    }
}

void shell06_eft_t(PRIM *pr) {
    FLMAT m;
    FLMAT m0;
    SHLW *sh = pr->owner;
    SH06E *e = &SH06_W(sh)->eft[pr->no];
    SH06W *w = SH06_W(sh);
    MDLW *md = eft_mdlw[0];
    void *mat;
    CLAY *cl;
    f32 sx, sy, sz;
    s16 k;
    s16 i;

    if (sh->stg == game_w.stage && md != 0 && md->flag != 0 && w->state < 3) {
        mat = md->mat;
        k = e->cnt & 1;
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                cl = &md->clay[64];
                sx = scale64_0067D2D0[k][0];
                sy = scale64_0067D2D0[k][1];
                sz = scale64_0067D2D0[k][2];
            } else {
                cl = &md->clay[66];
                if (w->state == 1 && w->x14 < 6) {
                    sx = scale66_2_0067D310[w->x14][0];
                    sy = scale66_2_0067D310[w->x14][1];
                    sz = scale66_2_0067D310[w->x14][2];
                } else {
                    sx = scale66_1_0067D2F0[k][0];
                    sy = scale66_1_0067D2F0[k][1];
                    sz = scale66_1_0067D2F0[k][2];
                }
                flmatMakeTrans(&m0, 0.0f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&m0);
            }
            flmatMakeScale(&m, sx, sy, sz);
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            flmatMul33_2(&m, &rview_mat);
            if (cl != 0 && cl->handle != -1) {
                flSetRenderState(0x60, 0);
                flSetRenderState(0x1A, (u32)&m);
                flSetRenderState(0x67, -0x41);
                Material_set_sub(mat, cl);
                clay_attr_set(cl->attr);
                SetTrnslMode(4, 1);
                flExecuteClay(cl->handle, 0);
            }
        }
        clay_attr_reset();
    }
}
