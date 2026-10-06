/* set13_nm - NOT BUILT. Near-match C for the set13 functions still in asm:
 * set13_m and set13_trans (logic from m2c drafts + asm, not matched) and
 * set13_disp_pos_calc, which is 13
 * instructions off: the original loads dir[1] and dir[2] before the first
 * store. Was set13c 0x00158DB0-0x00158F18 (see set13.c). Helpers for
 * set13_m: a point at distance d from the camera along dir, and a test
 * whether the line from the camera to a point is blocked by one of the
 * stage's spheres (stage_sphr_tbl: r,x,y,z quads ending with r = -1). */
#include "set.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"

/* Capsule handed to hit_cap_pk: two points and a radius. */
typedef struct SET13_CAP {
    f32 p0[3];          /* 0x00 */
    f32 p1[3];          /* 0x0C */
    f32 r;              /* 0x18 */
} SET13_CAP;

extern FLMAT rview_mat;
extern f32 *stage_sphr_tbl[];

void flvecCopy(f32 *, f32 *);

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

extern SET_MDLW *set_mdlw;
extern f32 *sun_pos_tbl[88];
extern s16 *stg_eft_mdl_no[88];
extern s16 flash_flag;
extern u8 ot4[];

int set13_hit_calc(f32 *pos);
void flvecNormalize(f32 *);
f32 flvecCalcLength(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flSqrt(f32);
f32 flArcSin(f32);
f32 flArcCos(f32);
f32 flAbs(f32);
int hit_point_cyl(f32 *, f32 *, f32, f32, f32);
void flmatInit(FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatRotZ33(FLMAT *, f32);
void SetFilterMode(int);
void hit_cap_pk(SET13_CAP *, void *);
u8 hit_cap_sphr_m(void *k, f32 *c, void *out, f32 r);   /* hit2c.c */

void set13_disp_pos_calc(f32 *out, f32 *dir, f32 d) {
    f32 x = dir[0];
    f32 y = dir[1];
    f32 z = dir[2];
    f32 a = d * x;
    f32 b = d * y;
    f32 c = d * z;

    out[0] = rview_mat[3][0] + a;
    out[1] = rview_mat[3][1] + b;
    out[2] = rview_mat[3][2] + c;
}


/* Is the player inside the box x0 < x < x1, z0 < z < z1? */
#define IN_BOX(pl, x0, x1, z0, z1) \
    ((pl)->pos[0] > (x0) && (pl)->pos[0] < (x1) && (pl)->pos[2] > (z0) && (pl)->pos[2] < (z1))

void set13_m(SETW *sw) {
    f32 dir[3];
    f32 disp[3];
    f32 sun[3];
    PLW *pl;
    int hide;
    s16 lim;
    s16 lim2;
    f32 dx, dz, d2, dist, a, b, c, t, step, lo;
    f32 x0lo, x0hi, z0lo, z0hi, x1lo, x1hi, z1lo;
    f32 thr, rad, amax, amin;
    u16 ang;

    sw->timer++;
    pl = &player_work[game_w.master];
    if (sw->arg != 5) {
        sun[0] = sun_pos_tbl[game_w.stage][0];
        sun[1] = sun_pos_tbl[game_w.stage][1];
        sun[2] = sun_pos_tbl[game_w.stage][2];
    }
    switch (sw->arg) {
    case 0:
        dir[0] = sun[0] - rview_mat[3][0];
        dir[1] = sun[1] - rview_mat[3][1];
        dir[2] = sun[2] - rview_mat[3][2];
        hide = (u8)set13_hit_calc(sun);
        flvecNormalize(dir);
        switch (game_w.stage) {
        case 0x33:
            if (pl->pos[0] < 8750.0f) {
                hide = 1;
            }
            break;
        case 0x34:
            if (IN_BOX(pl, 3000.0f, 15000.0f, 3000.0f, 8000.0f)) {
                hide = 1;
            }
            break;
        case 0x37:
            if (IN_BOX(pl, 0.0f, 6200.0f, 0.0f, 9300.0f)) {
                hide = 1;
            }
            break;
        case 0x39:
            if (pl->pos[2] <= 100.0f || pl->pos[2] >= 10400.0f) {
                hide = 1;
            }
            break;
        }
        if (hide != 0) {
            sw->speed -= 0.07f;
            if (sw->speed < 0.0f) {
                sw->speed = 0.0f;
            }
        } else {
            sw->speed += 0.07f;
            if (sw->speed > 1.0f) {
                sw->speed = 1.0f;
            }
        }
        set13_disp_pos_calc(disp, dir, 100.0f);
        switch (game_w.stage) {
        case 0x12:
        case 0x16:
        case 0x17:
        case 0x2F:
        case 0x30:
        case 0x31:
            if (sw->timer >= 400) {
                sw->timer = 0;
            }
            break;
        }
        break;
    case 1:
        flvecCopy(dir, rview_mat[2]);
        set13_disp_pos_calc(disp, dir, -50.0f);
        if (game_w.stage == 0x22 || game_w.stage == 0x18) {
            amin = 0.1f;
            lim = 500;
            thr = 4000000.0f;
            amax = 0.4f;
            dx = 9600.0f - rview_mat[3][0];
            rad = 2000.0f;
            dz = 10180.0f - rview_mat[3][2];
        } else {
            lim = 0x400;
        }
        if (sw->timer >= lim) {
            sw->timer = 0;
        }
        d2 = dx * dx + dz * dz;
        if (d2 <= thr) {
            sw->mode2 = 0;
        } else {
            dist = flSqrt(d2);
            a = flArcSin(rad / dist);
            c = (-dir[0] * dx + -dir[2] * dz) / (dist * flSqrt(dir[0] * dir[0] + dir[2] * dir[2]));
            if (c <= -1.0f) {
                c = -1.0f;
            } else if (!(c < 1.0f)) {
                c = 1.0f;
            }
            b = flArcCos(c);
            sw->mode2 = 1;
        }
        if (sw->mode2 == 0) {
            sw->speed += 0.02f;
            if (sw->speed > amax) {
                sw->speed = amax;
            }
        } else {
            t = (amax + amin) - amax * (b - a) / 3.1415927f;
            if (t > amax) {
                t = amax;
            }
            if (sw->speed > t) {
                sw->speed -= 0.02f;
                if (sw->speed < t) {
                    sw->speed = t;
                }
            } else {
                sw->speed = t;
            }
        }
        break;
    case 2:
        if (sw->timer >= 400) {
            sw->timer = 0;
        }
        flvecCopy(dir, rview_mat[2]);
        set13_disp_pos_calc(disp, dir, -30.0f);
        break;
    case 3:
        sw->cnt++;
        if (game_w.stage == 0x36) {
            lim = 0x640;
            lim2 = 0x2BC;
        } else {
            lim = 0x320;
            lim2 = 0x12C;
        }
        if (sw->timer >= lim) {
            sw->timer = 0;
        }
        if (sw->cnt >= lim2) {
            sw->cnt = 0;
        }
        flvecCopy(dir, rview_mat[2]);
        set13_disp_pos_calc(disp, dir, -30.0f);
        break;
    case 4:
        disp[1] = 41.0f;
        disp[0] = 13960.0f;
        disp[2] = 8560.0f;
        if (hit_point_cyl(pl->pos, disp, 4000.0f, -1.0f, -1.0f) != 0) {
            sw->speed -= 0.015f;
            if (sw->speed < 0.125f) {
                sw->speed = 0.125f;
            }
        } else {
            dir[0] = sun[0] - rview_mat[3][0];
            dir[1] = sun[1] - rview_mat[3][1];
            dir[2] = sun[2] - rview_mat[3][2];
            dist = flvecCalcLength(dir);
            if (dist < 0.001f) {
                ang = 0;
            } else {
                ang = (u16)(0.5f + 65536.0f * flArcCos(-flvecInnerProduct(dir, rview_mat[2]) / dist) / 6.2831855f);
            }
            ang = flAbs((s16)ang);
            t = 0.75f - 0.5f * (ang / 32768.0f);
            if (sw->speed > t) {
                sw->speed -= 0.015f;
                if (sw->speed < t) {
                    sw->speed = t;
                }
            } else {
                sw->speed += 0.015f;
                if (sw->speed > t) {
                    sw->speed = t;
                }
            }
        }
        flvecCopy(dir, rview_mat[2]);
        set13_disp_pos_calc(disp, dir, -30.0f);
        break;
    case 5:
        if ((s8)--sw->se1 <= 0) {
            sw->mode++;
            return;
        }
        if (sw->se1 < 4) {
            sw->speed = sw->se1 / 4.0f;
        }
        dir[0] = sw->pos[0] - rview_mat[3][0];
        dir[1] = sw->pos[1] - rview_mat[3][1];
        dir[2] = sw->pos[2] - rview_mat[3][2];
        flvecNormalize(dir);
        set13_disp_pos_calc(disp, dir, 200.0f);
        break;
    case 6:
        flvecCopy(dir, rview_mat[2]);
        set13_disp_pos_calc(disp, dir, -50.0f);
        switch (game_w.stage) {
        case 0x23:
            lim = 400;
            break;
        case 0x46:
            lim = 200;
            break;
        case 0x49:
            lim = 200;
            break;
        case 0x4B:
            lim = 200;
            break;
        default:
            lim = 0x400;
            break;
        }
        if (sw->timer >= lim) {
            sw->timer = 0;
        }
        break;
    case 7:
        step = 1.0f;
        sw->cnt++;
        switch (game_w.stage) {
        case 0x2D:
            x1lo = 2800.0f;
            lim = 0x2BC;
            lim2 = 200;
            x1hi = 17800.0f;
            x0lo = 3400.0f;
            x0hi = 17200.0f;
            z0hi = 16600.0f;
            z1lo = x1lo;
            z0lo = x0lo;
            break;
        case 0x38:
            x1lo = 100.0f;
            lim = 0x2BC;
            lim2 = 200;
            x1hi = 17200.0f;
            z1lo = 2200.0f;
            x0hi = 16600.0f;
            x0lo = 11800.0f;
            z0lo = 2800.0f;
            z0hi = x0hi;
            break;
        }
        if (sw->timer >= lim) {
            sw->timer = 0;
        }
        if (sw->cnt >= lim2) {
            sw->cnt = 0;
        }
        switch (sw->se0) {
        case 0:
            flvecCopy(dir, rview_mat[2]);
            step = 0.08f;
            if (game_w.info_stop == 1 || IN_BOX(pl, x1lo, x1hi, z1lo, x1hi)) {
                sw->se0 = 1;
            }
            flvecCopy(dir, rview_mat[2]);
            set13_disp_pos_calc(disp, dir, -30.0f);
            break;
        case 3:
            dir[0] = sun[0] - rview_mat[3][0];
            dir[1] = sun[1] - rview_mat[3][1];
            dir[2] = sun[2] - rview_mat[3][2];
            flvecNormalize(dir);
            step = -0.08f;
            if (game_w.info_stop == 1 || IN_BOX(pl, x0lo, x0hi, z0lo, z0hi)) {
                sw->se0 = 2;
            } else if (pl->pos[0] < x1lo || pl->pos[0] > x1hi || pl->pos[2] < z1lo || pl->pos[2] > x1hi) {
                sw->se0 = 0;
            }
            set13_disp_pos_calc(disp, dir, 100.0f);
            break;
        case 1:
            flvecCopy(dir, rview_mat[2]);
            if (sw->speed == 0.0f) {
                sw->speed = 0.0f;
                sw->se0 = 3;
            }
            step = -0.08f;
            if (game_w.info_stop == 1 || IN_BOX(pl, x0lo, x0hi, z0lo, z0hi)) {
                sw->se0 = 2;
            } else if (pl->pos[0] < x1lo || pl->pos[0] > x1hi || pl->pos[2] < z1lo || pl->pos[2] > x1hi) {
                sw->se0 = 0;
            }
            flvecCopy(dir, rview_mat[2]);
            set13_disp_pos_calc(disp, dir, -30.0f);
            break;
        case 2:
            dir[0] = sun[0] - rview_mat[3][0];
            dir[1] = sun[1] - rview_mat[3][1];
            dir[2] = sun[2] - rview_mat[3][2];
            hide = (u8)set13_hit_calc(sun);
            flvecNormalize(dir);
            if (hide != 0) {
                step = -0.07f;
            } else {
                step = 0.07f;
            }
            if (game_w.info_stop == 1) {
                sw->se0 = 2;
            } else {
                if (game_w.info_stop == 0 && !IN_BOX(pl, x0lo, x0hi, z0lo, z0hi)) {
                    sw->se0 = 3;
                }
                set13_disp_pos_calc(disp, dir, 100.0f);
            }
            break;
        }
        if (step < 1.0f) {
            sw->speed += step;
            if (sw->speed < 0.0f) {
                sw->speed = 0.0f;
            } else if (sw->speed > 1.0f) {
                sw->speed = 1.0f;
            }
        }
        break;
    }
    if (sw->prim != 0) {
        sw->prim->pos[0] = disp[0];
        sw->prim->pos[1] = disp[1];
        sw->prim->pos[2] = disp[2];
        add_prim(ot4, sw->prim, 1, 0);
    }
}

/* Roll angle of the flare: angle between the sun direction and the camera
 * view direction on the XZ plane, signed by their cross product. */
static f32 set13_roll(f32 sx, f32 sz, PRIM *pr) {
    f32 ax = sx - pr->pos[0];
    f32 az = sz - pr->pos[2];
    f32 bx = -rview_mat[2][0];
    f32 bz = -rview_mat[2][2];
    f32 c = (ax * bx + az * bz) / (flSqrt(ax * ax + az * az) * flSqrt(bx * bx + bz * bz));
    f32 a;

    if (!(c < 1.0f)) {
        c = 1.0f;
    }
    a = flArcCos(c);
    if (ax * bz - az * bx < 0.0f) {
        a = -a;
    }
    return a;
}

#define ALPHA_GREY(k) ((k) | ((k) << 16) | 0xFF000000 | ((k) << 8))

void set13_trans(PRIM *pr) {
    FLMAT m;
    FLMAT uv;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    s16 no;
    CLAY *cl;
    f32 sx;
    f32 sz;
    s16 lim;
    s16 lim2;
    u8 k;

    if (mw == 0 || mw->flag == 0) {
        return;
    }
    if (flash_flag == 1 || flash_flag == 2) {
        return;
    }
    no = stg_eft_mdl_no[game_w.stage][1];
    if (no < 0) {
        return;
    }
    cl = &mw->clay[no];
    if (sw->arg != 5) {
        sx = sun_pos_tbl[game_w.stage][0];
        sz = sun_pos_tbl[game_w.stage][2];
    }
    flSetRenderState(0x60, 0);
    flSetRenderState(0x6C, 0);
    flSetRenderState(0x6D, 7);
    flmatCopy(&m, &rview_mat);
    flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
    switch (sw->arg) {
    case 0:
        flmatInit(&m);
        switch (game_w.stage) {
        case 0x12:
        case 0x16:
        case 0x17:
        case 0x2F:
        case 0x30:
        case 0x31:
            flmatMakeTrans(&uv, 0.0025f * sw->timer, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 0x28:
            flmatMakeTrans(&uv, 0.0078125f * (sw->timer & 0x7F), 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        default:
            flmatRotZ33(&m, 0.2f * set13_roll(sx, sz, pr));
            break;
        }
        flmatMul33_2(&m, &rview_mat);
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        k = 255.0f * sw->speed;
        flSetRenderState(0x67, ALPHA_GREY(k));
        break;
    case 1:
        if (game_w.stage == 0x22 || game_w.stage == 0x18) {
            lim = 500;
        } else {
            lim = 0x400;
        }
        flmatMakeTrans(&uv, (1.0f / lim) * (sw->timer % lim), 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        flSetRenderState(0x67, ((u8)(255.0f * sw->speed) << 24) | 0xFFFFFF);
        break;
    case 2:
        flmatMakeTrans(&uv, 0.0025f * sw->timer, 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        flSetRenderState(0x1A, (u32)&m);
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 3:
        if (game_w.stage == 0x36) {
            lim = 0x640;
            lim2 = 0x2BC;
        } else {
            lim = 0x320;
            lim2 = 0x12C;
        }
        flmatMakeTrans(&uv, (1.0f / lim) * sw->timer, 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        flSetRenderState(0x1A, (u32)&m);
        flSetRenderState(0x6D, 7);
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        cl = &mw->clay[no + 1];
        flmatMakeTrans(&uv, (1.0f / lim2) * sw->cnt, 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        break;
    case 4:
        flmatMakeTrans(&uv, 0.0009765625f * (sw->timer & 0x3FF), 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        flSetRenderState(0x67, ((u8)(255.0f * sw->speed) << 24) | 0xFFFFFF);
        break;
    case 5:
        k = 255.0f * sw->speed;
        flSetRenderState(0x67, ALPHA_GREY(k));
        break;
    case 6:
        switch (game_w.stage) {
        case 0x23:
            lim = 400;
            break;
        case 0x46:
            lim = 200;
            break;
        case 0x49:
            lim = 200;
            break;
        case 0x4B:
            lim = 200;
            break;
        default:
            lim = 0x400;
            break;
        }
        flmatMakeTrans(&uv, (1.0f / lim) * (sw->timer % lim), 0.0f, 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        flSetRenderState(0x67, ((u8)(255.0f * sw->speed) << 24) | 0xFFFFFF);
        break;
    case 7:
        switch (sw->se0) {
        case 2:
        case 3:
            flmatInit(&m);
            flmatRotZ33(&m, 0.2f * set13_roll(sx, sz, pr));
            flmatMul33_2(&m, &rview_mat);
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            k = 255.0f * sw->speed;
            flSetRenderState(0x67, ALPHA_GREY(k));
            break;
        case 0:
        case 1:
            flmatMakeTrans(&uv, 0.0014285714f * sw->timer, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            flSetRenderState(0x1A, (u32)&m);
            flSetRenderState(0x67, ((u8)(255.0f * sw->speed) << 24) | 0xFFFFFF);
            flSetRenderState(0x6D, 7);
            if (&mw->clay[no + 1] != 0 && mw->clay[no + 1].handle != -1) {
                clay_attr_set(mw->clay[no + 1].attr);
                flExecuteClay(mw->clay[no + 1].handle, 0);
            }
            cl = &mw->clay[no + 2];
            flmatMakeTrans(&uv, 0.005f * sw->cnt, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        }
        break;
    }
    flSetRenderState(0x1A, (u32)&m);
    if (cl != 0 && cl->handle != -1) {
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
    flSetRenderState(0x6C, 1);
    flSetRenderState(0x6D, 3);
}
