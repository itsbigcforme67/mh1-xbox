/* Player code (SLPM_654.95 0x00151A60-0x00152028): hit data expand, front_land_ck, stage checks */
#include "pl.h"
#include "game.h"
#include "plf.h"
f32 *Stage_data_get(int stg);
void flmatGetTrans(f32 *, u8 *);

int hit_data_expand(PLW *pl, s16 *d, f32 *cap, f32 *sph) {
    f32 v[3];
    f32 t[3];
    u8 *mats;
    if (d[0] == 0x7F) {
        return -1;
    }
    mats = *(u8 **)(pl->mdl50C + 0x24);
    flmatGetTrans(t, mats + d[0] * 0x190);
    switch (d[1]) {
    case 0:
        sph[3] = ((f32 *)d)[3] * pl->scl[0];
        flvecApplyMat33(v, (f32 *)d + 4, (f32 *)(mats + d[0] * 0x190));
        sph[0] = t[0] + v[0];
        sph[1] = t[1] + v[1];
        sph[2] = t[2] + v[2];
        return 0;
    case 1:
        cap[6] = ((f32 *)d)[3] * pl->scl[0];
        flvecApplyMat33(v, (f32 *)d + 4, (f32 *)(mats + d[0] * 0x190));
        cap[0] = t[0] + v[0];
        cap[1] = t[1] + v[1];
        cap[2] = t[2] + v[2];
        flvecApplyMat33(v, (f32 *)d + 7, (f32 *)(mats + d[0] * 0x190));
        cap[3] = t[0] + v[0];
        cap[4] = t[1] + v[1];
        cap[5] = t[2] + v[2];
        return 1;
    default:
        return -1;
    }
}

int hit_data_expand2(PLW *pl, s16 *d, f32 *cap, f32 *sph) {
    f32 *p = (f32 *)pl;
    f32 *q = (f32 *)d;
    switch (d[1]) {
    case 0:
        sph[3] = q[3];
        sph[0] = p[0] + q[4];
        sph[1] = p[1] + q[5];
        sph[2] = p[2] + q[6];
        return 0;
    case 1:
        cap[6] = q[3];
        cap[0] = p[0] + q[4];
        cap[1] = p[1] + q[5];
        cap[2] = p[2] + q[6];
        cap[3] = p[0] + q[7];
        cap[4] = p[1] + q[8];
        cap[5] = p[2] + q[9];
        return 1;
    default:
        return -1;
    }
}

int hit_data_expand3(f32 *p, f32 *a, f32 *b, f32 *cap) {
    cap[6] = b[3];
    cap[0] = p[0];
    cap[1] = p[1];
    cap[2] = p[2];
    cap[3] = a[0];
    cap[4] = a[1];
    cap[5] = a[2];
    return 1;
}

int front_land_ck(f32 dist, f32 h, f32 tol, PLW *pl, int *out) {
    f32 g;
    f32 loc[3];
    f32 w[3];
    s32 a[3];
    f32 m[16];
    f32 ty;
    loc[0] = 0;
    loc[1] = 0;
    loc[2] = dist;
    a[0] = 0;
    a[1] = pl->ang[1];
    a[2] = 0;
    cpRotMatrix(a, m);
    flvecApplyMat33(w, loc, m);
    loc[0] = pl->pos[0] + w[0];
    loc[1] = pl->pos[1] + w[1];
    loc[2] = pl->pos[2] + w[2];
    ty = loc[1] + h;
    if (GetGroundHitAreaUpper(pl, loc, &g) != 1) {
        return 1;
    }
    if (!(g < ty) && g <= ty + tol) {
        *(f32 *)out = g;
        return 1;
    }
    return 0;
}

int front_land_ck2(f32 dist, f32 h, PLW *pl, int mode) {
    f32 g;
    f32 loc[3];
    f32 w[3];
    s32 a[3];
    f32 m[16];
    f32 ty;
    loc[0] = 0;
    loc[1] = 0;
    loc[2] = dist;
    a[0] = 0;
    a[1] = pl->ang[1];
    a[2] = 0;
    cpRotMatrix(a, m);
    flvecApplyMat33(w, loc, m);
    loc[0] = pl->pos[0] + w[0];
    loc[1] = pl->pos[1] + w[1];
    loc[2] = pl->pos[2] + w[2];
    ty = pl->pos[1] + h;
    if (GetGroundHitAreaUpper(pl, loc, &g) != 1) {
        return 0;
    }
    if ((u16)mode == 0) {
        if (g < ty) {
            return 1;
        }
    } else if (!(g <= ty)) {
        return 1;
    }
    return 0;
}

int Pl_stg_ck(PLW *pl) {
    return pl->stg == game_w.stage;
}

int Em_stg_ck(PLW *pl) {
    return pl->stg == game_w.stage;
}
