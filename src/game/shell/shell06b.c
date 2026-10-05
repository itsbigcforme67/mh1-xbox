/* shell06 - game.bin 0x0062C4D0-0x0062D4C4: shell06_move to shell06_eft_t.
 * Bowgun shots: the mode machine, damage fall-off with range, firing
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
extern s16 *shell06_change_time[];
extern s16 shell06_hit_mark1[];
extern s16 shell06_hit_mark2[];
extern s16 shell06_sound_type[];
extern f32 shell06_atk_rate[];
extern f32 scale64_0067D2D0[][3];
extern f32 scale66_1_0067D2F0[][3];
extern f32 scale66_2_0067D310[][3];

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
