/* shit3 - SLPM_654.95 0x00119210-0x0011B1E0 (f_sphr part 5): ground height
 * queries against the loaded ground HITS file. Each takes a position {x, y,
 * z}, finds the polygons of the cell under (x, z), keeps those whose
 * triangle contains the point (PointHitCheckF3) and computes the plane
 * height there. GetGroundHit picks among up to 5 heights the highest one
 * not more than 50 above y (else the lowest), GetGroundShellHit the same
 * for shells (100, skips water/lava kinds), GetWaterHit / GetTenjoHit /
 * GetYouganHit report water, ceiling and lava polygons. Guesses. */
#include "types.h"
#include "hit3.h"
#include "game.h"

s32 *GetGroundTblAdrs(f32 *);
int GroundFieldInCheck(f32 *);
int PointHitCheckF3(f32 *, f32 *);
extern HKIND *ground_tbl_add[];

f32 GetGroundHit(f32 *pos) {
    f32 pt[4];
    f32 h[8];
    f32 tri[12];
    f32 hf[8];
    s32 *cell;
    HPOLY *pl;
    s8 n = 0;
    s8 m;
    s8 any;
    s8 c;
    s8 i;
    f32 *hp = h;
    f32 *fp;
    f32 r;

    h[0] = pos[1];
    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1 && n < 5) {
                    n++;
                    *hp = -(pl->d + pl->n[0] * pos[0] + pl->n[2] * pos[2]) / pl->n[1];
                    hp++;
                }
                cell++;
            } while (*cell != -1);
        }
    }
    m = n;
    if (m != 1) {
        any = 0;
        c = 0;
        i = 0;
        if (m > 0) {
            hp = h;
            fp = hf;
            do {
                if (*hp <= 50.0f + pos[1]) {
                    any = 1;
                    *fp = *hp;
                    fp++;
                    c++;
                }
                i++;
                hp++;
            } while (i < m);
        }
        if (any == 1) {
            if (c == 1) {
                r = hf[0];
            } else {
                i = 0;
                if (c > 0) {
                    fp = hf;
                    do {
                        if (i == 0) {
                            h[0] = *fp;
                        } else {
                            if (h[0] < *fp) h[0] = *fp;
                        }
                        i++;
                        fp++;
                    } while (i < c);
                }
                r = h[0];
            }
        } else {
            i = 0;
            if (m > 0) {
                hp = h;
                do {
                    if (i == 0) {
                        hf[0] = *hp;
                    } else {
                        if (!(hf[0] <= *hp)) hf[0] = *hp;
                    }
                    i++;
                    hp++;
                } while (i < m);
            }
            r = hf[0];
        }
    } else {
        r = h[0];
    }
    if (r == -0.0f) r = 0.0f;
    return r;
}

f32 GetGroundShellHit(f32 *pos) {
    f32 pt[4];
    f32 h[8];
    f32 tri[12];
    f32 hf[8];
    s32 *cell;
    HPOLY *pl;
    s8 n = 0;
    s8 m;
    s8 any;
    s8 c;
    s8 i;
    f32 *hp = h;
    f32 *fp;
    f32 r;

    h[0] = pos[1];
    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1 && n < 5) {
                    f32 t = pl->n[0] * pos[0] + pl->n[2] * pos[2];
                    HKIND *k = &ground_tbl_add[game_w.stage][pl->kind];
                    if (k->water == 0 && k->x0D == 0) {
                        *hp = -(pl->d + t) / pl->n[1];
                        n++;
                        hp++;
                    }
                }
                cell++;
            } while (*cell != -1);
        }
    }
    m = n;
    if (m != 1) {
        any = 0;
        c = 0;
        i = 0;
        if (m > 0) {
            hp = h;
            fp = hf;
            do {
                if (*hp <= 100.0f + pos[1]) {
                    any = 1;
                    *fp = *hp;
                    fp++;
                    c++;
                }
                i++;
                hp++;
            } while (i < m);
        }
        if (any == 1) {
            if (c == 1) {
                r = hf[0];
            } else {
                i = 0;
                if (c > 0) {
                    fp = hf;
                    do {
                        if (i == 0) {
                            h[0] = *fp;
                        } else {
                            if (h[0] < *fp) h[0] = *fp;
                        }
                        i++;
                        fp++;
                    } while (i < c);
                }
                r = h[0];
            }
        } else {
            i = 0;
            if (m > 0) {
                hp = h;
                do {
                    if (i == 0) {
                        hf[0] = *hp;
                    } else {
                        if (!(hf[0] <= *hp)) hf[0] = *hp;
                    }
                    i++;
                    hp++;
                } while (i < m);
            }
            r = hf[0];
        }
    } else {
        r = h[0];
    }
    if (r == -0.0f) r = 0.0f;
    return r;
}

int GetWaterHit(f32 *pos, f32 *out) {
    f32 pt[4];
    f32 tri[12];
    s32 *cell;
    HPOLY *pl;
    int r = 0;

    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1) {
                    f32 t = pl->n[0] * pos[0] + pl->n[2] * pos[2];
                    if (ground_tbl_add[game_w.stage][pl->kind].water != 0) {
                        *out = -(pl->d + t) / pl->n[1];
                        r = 1;
                    }
                }
                cell++;
            } while (*cell != -1);
        }
    }
    if (*out == -0.0f) *out = 0.0f;
    return r;
}

int GetTenjoHit(f32 *pos, f32 *out, HPOLY *attr) {
    f32 pt[4];
    f32 tri[12];
    s32 *cell;
    HPOLY *pl;
    int r = 0;

    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (pl->n[1] < 0.0f && PointHitCheckF3(tri, pt) == 1) {
                    f32 t;
                    r = 1;
                    t = pl->n[0] * pos[0] + pl->n[2] * pos[2];
                    *out = -(pl->d + t) / pl->n[1];
                    attr->kind = pl->kind;
                    attr->b1 = pl->b1;
                    attr->h2 = pl->h2;
                }
                cell++;
            } while (*cell != -1);
        }
    }
    if (*out == -0.0f) *out = 0.0f;
    return r;
}

int GetYouganHit(f32 *pos) {
    f32 pt[4];
    f32 tri[12];
    s32 *cell;
    HPOLY *pl;
    int r = 0;

    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1 && ground_tbl_add[game_w.stage][pl->kind].lava != 0) {
                    r = 1;
                }
                cell++;
            } while (*cell != -1);
        }
    }
    return r;
}

/* 0x0011B1E0: ground cell list for a position. */
s32 *GetGroundTblAdrs(f32 *p) {
    f32 csz = (u32)diorama_w.gcsz;
    f32 csx = (u32)diorama_w.gcsx;

    return (s32 *)diorama_w.gtbl[(int)(p[2] / csz) + diorama_w.gnz * (int)(p[0] / csx)];
}
