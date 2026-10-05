/* eft22 - game.bin 0x00555A90-0x00556D94; eft22_end_init is still assembly
 * (see eft22_nm.c), the rest is in eft22b.c. The fishing float: thrown from the rod tip (joint
 * 0xE) in the player's facing direction, it falls to the stage's water
 * height (set per stage in eft22_i) and bobs there while eft23 fish come
 * to it. Drawn with a line back to the rod tip (eft22_t, eft22_sao_pos). */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "uki.h"
#include "fl.h"
#include "clay.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

extern EFT_MDLW *eft_mdlw[5];
extern u32 eft22_pl_rgb[];      /* float colour per player */
extern f32 sao_top_ofs[][3];    /* rod tip offset per bend */
extern u8 ot1[];

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

u32 ran_suu(int);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void get_joint_pos(PLW *, int, f32 *);

void flvecNormalize(f32 *);
void release_prim(s16);
void flmatRotX33(FLMAT *, f32);
void flmatRotZ33(FLMAT *, f32);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
void flExecuteClay(s32, int);
void Material_set_sub(void *, CLAY *);
f32 flvecCalcLength(f32 *);
FLMAT *get_joint_wmat(PLW *, int);
int frame_check2(PLW *, int, f32);
void se_req2(int, int, int, f32 *, int, int);
void eft22_sao_pos(f32 *out, PLW *pl);
void eft22_v0_calc(f32 *vel, f32 from, f32 to, f32 grav, f32 time);
void ScaleVector(f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
int pl_flag_ck(PLW *, int);
int frame_check(PLW *, int, f32);
u8 Pl_stg_ck(PLW *);
void Eft20_set2(f32 *, int, int, f32);
void Eft08_set(f32 *, int, int, f32);
void eft22_end_init(EFTW *ew, UKI *w);
void eft22_line_sub(EFTW *ew, UKI *w);
void eft22_rate_add(EFTW *ew, UKI *w);
void eft22_se_req(EFTW *ew, f32 *pos, s16 kind);

void eft22_move(EFTW *ew);
void eft22_i(EFTW *ew);
void eft22_m(EFTW *ew);
void eft22_d(EFTW *ew);
void eft22_e(EFTW *ew);
void eft22_t(PRIM *pr);

void Eft22_set(PLW *pl, int arg) {
    EFTW *ew = pull_eft_work(1);

    if (ew != 0) {
        ew->type = 0x16;
        ew->move = eft22_move;
        ew->work14 = 0;
        ew->owner = (EMW *)pl;
        ew->x1E = pl->id;
        ew->arg = arg;
    }
}

void eft22_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft22_i(ew);
        break;
    case 1:
        eft22_m(ew);
        break;
    case 2:
        eft22_d(ew);
        break;
    case 3:
        eft22_e(ew);
        break;
    }
}

void eft22_i(EFTW *ew) {
    PLW *pl = (PLW *)ew->owner;
    UKI *w = ew->work;

    ew->mode++;
    ew->mode2 = 0;
    ew->stg = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    get_joint_pos(pl, 0xE, ew->pos);
    flvecCopy(w->pos, ew->pos);
    ew->u0A.ang = pl->ang[1];
    w->ang = ew->u0A.joint;
    w->ang2 = w->ang;
    w->timer = 0;
    w->vel[0] = 0.001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
    w->vel[1] = 2.0f + 0.002f * (f32)((u16)ran_suu(1) & 0x3FF);
    w->vel[2] = 30.0f + 0.002f * (f32)((u16)ran_suu(1) & 0x3FF);
    w->grav = -2.0f;
    w->x28 = 0;
    switch (game_w.stage) {
    case 1:
        w->suimen = -94.0f;
        break;
    case 3:
        w->suimen = -131.0f;
        break;
    case 0x15:
        w->suimen = -10.0f;
        break;
    case 0x2A:
        w->suimen = -45.0f;
        break;
    case 0x30:
        w->suimen = -20.0f;
        break;
    case 0x3B:
        w->suimen = -222.0f;
        break;
    case 0x3D:
        w->suimen = -41.0f;
        break;
    case 0x43:
        w->suimen = -20.0f;
        break;
    case 0x4B:
        w->suimen = -20.0f;
        break;
    default:
        w->suimen = 0.0f;
        break;
    }
    w->line_on = 0;
    flvecRotY(w->vel, DEG2RAD(ANG2DEG(ew->u0A.joint)));
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft22_t;
    } else {
        push_eft_work(ew);
    }
}

void eft22_rate_add(EFTW *ew, UKI *w) {
    ew->pos[0] += w->vel[0];
    ew->pos[1] += w->vel[1];
    ew->pos[2] += w->vel[2];
    w->vel[1] += w->grav;
}

void eft22_m(EFTW *ew) {
    f32 v[3];
    f32 d[3];
    UKI *w = ew->work;
    PLW *pl = (PLW *)ew->owner;
    f32 dist;
    f32 sc;
    f32 g;
    s16 lim;

    ew->timer++;
    switch (ew->mode2) {
    case 0:
        if (ew->pos[1] < w->suimen - 5.0f) {
            ew->pos[1] = w->suimen - 5.0f;
            w->pos[0] = ew->pos[0];
            w->pos[1] = w->suimen - 5.0f;
            w->pos[2] = ew->pos[2];
            v[0] = w->pos[0];
            v[1] = 15.0f + w->suimen;
            v[2] = w->pos[2];
            Eft20_set2(v, 0x11, ran_suu(1), 1.0f);
            w->vel[0] = 0.0f;
            w->vel[1] = 0.0f;
            w->vel[2] = 0.0f;
            w->grav = 0.0f;
            ew->mode2++;
            w->timer = 0;
        }
        break;
    case 1:
        v[0] = ew->pos[0];
        v[1] = 15.0f + w->suimen;
        v[2] = ew->pos[2];
        if (ew->pos[1] > w->pos[1]) {
            ew->pos[1] = w->pos[1];
        }
        if (ew->pos[1] < w->pos[1]) {
            w->grav = 1.0f;
        } else {
            w->vel[1] *= 0.2f;
            w->grav = 2.0f;
        }
        if (pl_flag_ck(pl, 0x80000) == 0) {
            ew->mode++;
            return;
        }
        if (pl->char0 == 0x326) {
            ew->mode2++;
        }
        break;
    case 2:
        v[0] = ew->pos[0];
        v[1] = 15.0f + w->suimen;
        v[2] = ew->pos[2];
        if (ew->pos[1] > w->pos[1]) {
            ew->pos[1] = w->pos[1];
        }
        if (w->timer < (s8)pl->x881) {
            w->vel[1] -= 10.0f + (s8)pl->x881;
            Eft20_set2(v, 0x11, ran_suu(1), 1.0f);
            eft22_se_req(ew, ew->pos, 0);
        }
        if (ew->pos[1] < w->pos[1]) {
            w->grav = 1.0f;
        } else {
            w->vel[1] *= 0.2f;
            w->grav = 2.0f;
        }
        w->timer = (s8)pl->x881;
        if (pl_flag_ck(pl, 0x80000) == 0) {
            ew->mode++;
            return;
        }
        switch (pl->char0) {
        case 0x327:
            ew->mode2++;
            w->vel[0] = ew->pos[0] - pl->pos[0];
            w->vel[2] = ew->pos[2] - pl->pos[2];
            flvecNormalize(w->vel);
            switch (pl->fish_time) {
            case 0x62:
            case 0x63:
            case 0x60:
                ScaleVector(w->vel, w->vel, 1.0f);
                w->slow = 0;
                break;
            default:
                ScaleVector(w->vel, w->vel, 3.0f);
                w->slow = 1;
                break;
            }
            flvecRotY(w->vel, DEG2RAD(ANG2DEG(((u16)ran_suu(1) & 0x3F) - 0x20)));
            break;
        case 0x328:
            w->cast = 0;
            eft22_end_init(ew, w);
            break;
        case 0x329:
            if (frame_check(pl, 0, 26.0f)) {
                w->cast = 1;
                eft22_end_init(ew, w);
            }
            break;
        case 0x32A:
            w->cast = 2;
            eft22_end_init(ew, w);
            break;
        }
        break;
    case 3:
        flvecCopy(d, w->vel);
        if (w->slow == 0) {
            ScaleVector(d, d, 0.2f);
            g = 2.0f;
            flvecRotY(d, DEG2RAD(ANG2DEG(((u16)ran_suu(1) & 0x7FFF) - 0x4000)));
            sc = 1.5f;
        } else {
            ScaleVector(d, d, 0.5f);
            g = 2.0f;
            flvecRotY(d, DEG2RAD(ANG2DEG(((u16)ran_suu(1) & 0x3FFF) - 0x2000)));
            sc = 3.0f;
        }
        w->vel[0] += d[0];
        w->vel[1] += d[1];
        w->vel[2] += d[2];
        if (ew->pos[1] > w->pos[1]) {
            ew->pos[1] = w->pos[1];
        }
        if ((ew->timer & 7) == 0) {
            v[0] = ew->pos[0];
            v[1] = 15.0f + w->suimen;
            v[2] = ew->pos[2];
            Eft08_set(v, 3, 0, sc);
            v[1] += 5.0f;
            Eft08_set(v, 0, 0, sc);
            eft22_se_req(ew, ew->pos, 0);
        }
        if (ew->pos[1] < w->pos[1]) {
            w->grav = 1.0f;
        } else {
            w->vel[1] *= 0.2f;
            w->grav = g;
        }
        switch (pl->char0) {
        case 0x327:
            break;
        case 0x328:
            if (frame_check(pl, 0, 16.0f)) {
                w->cast = 0;
                eft22_end_init(ew, w);
                v[0] = ew->pos[0];
                v[1] = 15.0f + w->suimen;
                v[2] = ew->pos[2];
                Eft08_set(v, 4, 0, sc);
                v[1] += 5.0f;
                Eft08_set(v, 1, 0, sc);
                eft22_se_req(ew, ew->pos, 1);
            }
            break;
        default:
            ew->mode++;
            return;
        }
        break;
    case 4:
        if ((w->cast == 0 && pl->char0 == 0x328) || (w->cast == 1 && pl->char0 == 0x329) ||
            (w->cast == 2 && pl->char0 == 0x32A)) {
            switch (w->cast) {
            case 0:
                if (++w->timer >= 35) {
                    get_joint_pos(pl, 0xE, ew->pos);
                    ew->pos[1] -= 10.0f;
                    ew->mode2++;
                    w->timer = 0;
                    w->vel[0] = 0.0f;
                    w->vel[1] = 0.0f;
                    w->vel[2] = 0.0f;
                    w->grav = 0.0f;
                }
                break;
            case 1:
                if (++w->timer >= 27) {
                    get_joint_pos(pl, 0xE, ew->pos);
                    ew->pos[1] -= 10.0f;
                    ew->mode2++;
                    w->timer = 0;
                    w->vel[0] = 0.0f;
                    w->vel[1] = 0.0f;
                    w->vel[2] = 0.0f;
                    w->grav = 0.0f;
                }
                break;
            default:
                if (++w->timer < 40) {
                    d[0] = w->pos[0] - ew->pos[0];
                    d[1] = w->pos[1] - ew->pos[1];
                    d[2] = w->pos[2] - ew->pos[2];
                    w->vel[0] = 0.09f * d[0];
                    w->vel[1] = 0.09f * d[1];
                    w->vel[2] = 0.09f * d[2];
                } else {
                    ew->mode++;
                    return;
                }
                break;
            }
        } else {
            ew->mode++;
            return;
        }
        break;
    case 5:
        if ((w->cast == 0 && pl->char0 == 0x328) || (w->cast == 1 && pl->char0 == 0x329)) {
            if (w->cast == 0) {
                lim = 50;
            } else {
                lim = 20;
            }
            if (++w->timer < lim) {
                get_joint_pos(pl, 0xE, ew->pos);
                ew->pos[1] -= 10.0f;
            } else {
                ew->mode++;
                return;
            }
        } else {
            ew->mode++;
            return;
        }
        break;
    }
    eft22_line_sub(ew, w);
    eft22_rate_add(ew, w);
    if (ew->mode2 == 3) {
        dist = flvecCalcDistance(ew->pos, w->pos);
        if (dist > 100.0f) {
            PointToPoint(d, ew->pos, w->pos);
            ScaleVector(d, d, 100.0f / dist);
            AddVector(ew->pos, w->pos, d);
        }
    }
    ew->prim->pos[0] = ew->pos[0];
    ew->prim->pos[1] = ew->pos[1];
    ew->prim->pos[2] = ew->pos[2];
    if (Pl_stg_ck(pl) == 0) {
        ew->mode++;
        return;
    }
    add_prim(ot1, ew->prim, 0x20, 0);
}

void eft22_d(EFTW *ew) {
    ew->mode++;
    release_prim(ew->prim_no);
}

void eft22_e(EFTW *ew) {
    push_eft_work(ew);
}

void eft22_t(PRIM *pr) {
    EFTW *ew = pr->owner;
    EFT_MDLW *mw = eft_mdlw[0];
    UKI *w = ew->work;
    void *mats = mw->mat;
    CLAY *cl;
    f32 sp[3];
    f32 d[3];
    FLMAT m;
    FLMAT m2;
    f32 rx, rz;

    switch (ew->arg) {
    case 1:
        flmatMakeScale(&m, 0.3f, 0.3f, 0.3f);
        cl = &mw->clay[24];
        break;
    default:
        flmatInit(&m);
        cl = &mw->clay[113];
        break;
    }
    flmatRotY33(&m, DEG2RAD(ANG2DEG(ew->u0A.joint)));
    flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
    flSetRenderState(0x67, eft22_pl_rgb[ew->x1E]);
    flSetRenderState(0x1A, (u32)&m);
    Material_set_sub(mats, cl);
    clay_attr_set(cl->attr);
    flExecuteClay(cl->handle, 0);
    if (w->line_on != 0) {
        cl = &mw->clay[127];
        eft22_sao_pos(sp, (PLW *)ew->owner);
        PointToPoint(d, pr->pos, sp);
        rx = flArcTan2(-d[2], flSqrt(d[0] * d[0] + d[1] * d[1]));
        rz = flArcTan2(d[0], -d[1]);
        flmatMakeScale(&m, 1.0f, flvecCalcLength(d), 1.0f);
        flmatRotX33(&m, rx);
        flmatRotZ33(&m, rz);
        flmatSetTrans(&m, sp[0], sp[1], sp[2]);
        flSetRenderState(0x67, -1);
        flSetRenderState(0x1A, (u32)&m);
        flmatMakeTrans(&m2, 0.0f, 0.03125f * w->line_cnt, 0.0f);
        flSetRenderState(0x19, (u32)&m2);
        Material_set_sub(mats, cl);
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
}

f32 Eft22_suimen_ck(EFTW *ew) {
    return ((UKI *)ew->work)->suimen;
}

void eft22_v0_calc(f32 *vel, f32 from, f32 to, f32 grav, f32 time) {
    vel[1] = (to - from) / time - 0.5f * grav * time;
}
