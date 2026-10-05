/* shit12_nm (not built): line-of-sight queries of f_sphr. GetWallHitLine
 * (0x0011B2A0) walks the grid cells along the segment a -> b, tests the wall
 * polygons of every cell the line passes close to (FaceLinePos +
 * NormalClipFace) and returns 1 with the nearest crossing in out, 0 with b
 * in out when nothing is hit; a point outside the wall grid is returned
 * as the "hit" (1). GetEyeHitLine (0x0011BAD0) does the same against the
 * ground polygons (not water / special kinds) and the wall polygons for an
 * entity. mask selects the wall polygons to skip (h2 & mask). The 20 entry
 * `seen` list keeps already tested polygons; where the original's loop over
 * it was collapsed by the decompiler this is a guess. */
#include "types.h"
#include "hit3.h"

#define EF(e, T, o) (*(T *)((u8 *)(e) + (o)))

int FaceLinePos(f32 *, f32 *, f32 *, f32 *);
int NormalClipFace(f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
f32 flSqrt(f32);
f32 flvecCalcDistance(f32 *, f32 *);
void flvecNormalize(f32 *);
int WallFieldInCheck(f32 *);
int GroundFieldInCheck(f32 *);
s32 *GetWallTblAdrs(f32 *);
s32 *GetGroundTblAdrs(f32 *);
extern HKIND *ground_tbl_add[];

#define SEEN(poly, n, seen, found)           \
    found = 0;                               \
    for (k = 0; k < (n); k++) {              \
        if ((poly) == (seen)[k]) found = 1;  \
    }

int GetWallHitLine(f32 *a, f32 *b, f32 *out, int mask) {
    HPOLY *seen[20];
    f32 e[6];           /* {b, a}: the segment for FaceLinePos */
    f32 dir[3];
    f32 d[3];
    f32 hp[3];
    f32 cen[3];
    f32 q[3];
    f32 best;
    f32 cx, cz;
    f32 margin;
    f32 t;
    int nx, nz, sx, sz;
    int ix, iz, i, j, k;
    int hit = 0;
    int ns = 0;
    int found;
    s32 *cp;
    HPOLY *pl;
    u32 csx = (u32)diorama_w.gcsx;
    u32 csz = (u32)diorama_w.gcsz;

    best = flvecCalcDistance(a, b);
    if (WallFieldInCheck(a) == 0) {
        SetVector(out, a[0], a[1], a[2]);
        return 1;
    }
    if (WallFieldInCheck(b) == 0) {
        SetVector(out, b[0], b[1], b[2]);
        return 1;
    }
    cx = a[0] / (f32)csx;
    cz = a[2] / (f32)csz;
    nx = (int)(b[0] / (f32)csx) - (int)cx;
    nz = (int)(b[2] / (f32)csz) - (int)cz;
    if (nx < 0) {
        sx = -1;
        nx = 1 - nx;
    } else {
        sx = 1;
        nx = nx + 1;
    }
    if (nz < 0) {
        sz = -1;
        nz = 1 - nz;
    } else {
        sz = 1;
        nz = nz + 1;
    }
    SetVector(e + 3, a[0], a[1], a[2]);
    SetVector(e, b[0], b[1], b[2]);
    PointToPoint(d, b, a);
    SetVector(dir, d[0], d[1], d[2]);
    flvecNormalize(dir);
    if (dir[0] == -0.0f) dir[0] = 0.0f;
    if (dir[2] == -0.0f) dir[2] = 0.0f;
    margin = (f32)(int)(csz / 10) + ((f32)(int)(csx / 10) + flSqrt((f32)csx * (f32)csx + (f32)(csz * csz)));
    for (ix = 0, i = 0; ix < nx; ix++, i += sx) {
        for (iz = 0, j = 0; iz < nz; iz++, j += sz) {
            cen[0] = (f32)(csx >> 1) + (f32)csx * (f32)((int)cx + i);
            cen[2] = (f32)(csz >> 1) + (f32)csz * (f32)((int)cz + j);
            cen[1] = 0.0f;
            if (dir[0] == 0.0f && dir[2] == 0.0f) {
                t = (-dir[0] * (a[0] - cen[0])) - (dir[2] * (a[2] - cen[2]));
            } else {
                t = ((-dir[0] * (a[0] - cen[0])) - (dir[2] * (a[2] - cen[2]))) / (dir[0] * dir[0] + dir[2] * dir[2]);
            }
            q[0] = a[0] + dir[0] * t;
            q[2] = a[2] + dir[2] * t;
            q[1] = 0.0f;
            if (flvecCalcDistance(cen, q) <= margin) {
                cp = GetWallTblAdrs(cen);
                if (cp != (s32 *)-1 && *cp != -1) {
                    do {
                        pl = (HPOLY *)*cp;
                        if (!(pl->h2 & (mask & 0xFFFF))) {
                            SEEN(pl, ns, seen, found);
                            if (!found) {
                                if (FaceLinePos(e, pl->n, pl->v[0], hp) != 0) {
                                    if (NormalClipFace(hp, pl->v[0], pl->n) != 0) {
                                        hit = 1;
                                        t = flvecCalcDistance(a, hp);
                                        if (!(best <= t)) {
                                            best = t;
                                            SetVector(out, hp[0], hp[1], hp[2]);
                                        }
                                    } else {
                                        goto next;
                                    }
                                }
                                if (ns < 0x14) seen[ns++] = pl;
                                if (hit != 0) return 1;
                            }
                        }
                    next:
                        cp++;
                    } while (*cp != -1);
                }
            }
        }
    }
    SetVector(out, b[0], b[1], b[2]);
    return 0;
}

int GetEyeHitLine(void *ent, f32 *a, f32 *b, f32 *out, int mask) {
    HPOLY *seen[20];
    f32 e[6];
    f32 dir[3];
    f32 d[3];
    f32 hp[3];
    f32 cen[3];
    f32 q[3];
    f32 best;
    f32 cx, cz;
    f32 margin;
    f32 t;
    int nx, nz, sx, sz;
    int ix, iz, i, j, k;
    int hit = 0;
    int ns = 0;
    int found;
    int attr;
    s32 *cp;
    HPOLY *pl;
    HKIND *hk;
    u32 csx = (u32)diorama_w.gcsx;
    u32 csz = (u32)diorama_w.gcsz;

    best = flvecCalcDistance(a, b);
    if (GroundFieldInCheck(a) == 0) {
        SetVector(out, a[0], a[1], a[2]);
        return 1;
    }
    if (GroundFieldInCheck(b) == 0) {
        SetVector(out, b[0], b[1], b[2]);
        return 1;
    }
    if (WallFieldInCheck(a) == 0) {
        SetVector(out, a[0], a[1], a[2]);
        return 1;
    }
    if (WallFieldInCheck(b) == 0) {
        SetVector(out, b[0], b[1], b[2]);
        return 1;
    }
    cx = a[0] / (f32)csx;
    cz = a[2] / (f32)csz;
    nx = (int)(b[0] / (f32)csx) - (int)cx;
    nz = (int)(b[2] / (f32)csz) - (int)cz;
    if (nx < 0) {
        sx = -1;
        nx = 1 - nx;
    } else {
        sx = 1;
        nx = nx + 1;
    }
    if (nz < 0) {
        sz = -1;
        nz = 1 - nz;
    } else {
        sz = 1;
        nz = nz + 1;
    }
    SetVector(e + 3, a[0], a[1], a[2]);
    SetVector(e, b[0], b[1], b[2]);
    PointToPoint(d, b, a);
    SetVector(dir, d[0], d[1], d[2]);
    flvecNormalize(dir);
    if (dir[0] == -0.0f) dir[0] = 0.0f;
    if (dir[2] == -0.0f) dir[2] = 0.0f;
    margin = (f32)(int)(csz / 10) + ((f32)(int)(csx / 10) + flSqrt((f32)csx * (f32)csx + (f32)(csz * csz)));
    for (ix = 0, i = 0; ix < nx; ix++, i += sx) {
        for (iz = 0, j = 0; iz < nz; iz++, j += sz) {
            cen[0] = (f32)(csx >> 1) + (f32)csx * (f32)((int)cx + i);
            cen[2] = (f32)(csz >> 1) + (f32)csz * (f32)((int)cz + j);
            cen[1] = 0.0f;
            if (dir[0] == 0.0f && dir[2] == 0.0f) {
                t = (-dir[0] * (a[0] - cen[0])) - (dir[2] * (a[2] - cen[2]));
            } else {
                t = ((-dir[0] * (a[0] - cen[0])) - (dir[2] * (a[2] - cen[2]))) / (dir[0] * dir[0] + dir[2] * dir[2]);
            }
            q[0] = a[0] + dir[0] * t;
            q[2] = a[2] + dir[2] * t;
            q[1] = 0.0f;
            if (flvecCalcDistance(cen, q) <= margin) {
                cp = GetGroundTblAdrs(cen);
                if (cp != (s32 *)-1 && *cp != -1) {
                    do {
                        pl = (HPOLY *)*cp;
                        hk = &ground_tbl_add[EF(ent, u8, 0x736)][pl->kind];
                        attr = hk->water;
                        if (EF(ent, u8, 2) == 8 || EF(ent, u8, 2) == 0x22 || EF(ent, u8, 2) == 0xE || EF(ent, u8, 2) == 0x1A) {
                            attr = (attr | hk->x0E) & 0xFF;
                        }
                        if (attr == 0) {
                            SEEN(pl, ns, seen, found);
                            if (!found) {
                                if (FaceLinePos(e, pl->n, pl->v[0], hp) != 0) {
                                    if (NormalClipFace(hp, pl->v[0], pl->n) != 0) {
                                        hit = 1;
                                        t = flvecCalcDistance(a, hp);
                                        if (!(best <= t)) {
                                            best = t;
                                            SetVector(out, hp[0], hp[1], hp[2]);
                                        }
                                    } else {
                                        goto gnext;
                                    }
                                }
                                if (ns < 0x14) seen[ns++] = pl;
                                if (hit != 0) return 1;
                            }
                        }
                    gnext:
                        cp++;
                    } while (*cp != -1);
                }
                cp = GetWallTblAdrs(cen);
                if (cp != (s32 *)-1 && *cp != -1) {
                    do {
                        pl = (HPOLY *)*cp;
                        if (!(pl->h2 & (mask & 0xFFFF))) {
                            SEEN(pl, ns, seen, found);
                            if (!found) {
                                if (FaceLinePos(e, pl->n, pl->v[0], hp) != 0) {
                                    if (NormalClipFace(hp, pl->v[0], pl->n) != 0) {
                                        hit = 1;
                                        t = flvecCalcDistance(a, hp);
                                        if (!(best <= t)) {
                                            best = t;
                                            SetVector(out, hp[0], hp[1], hp[2]);
                                        }
                                    } else {
                                        goto wnext;
                                    }
                                }
                                if (ns < 0x14) seen[ns++] = pl;
                                if (hit != 0) return 1;
                            }
                        }
                    wnext:
                        cp++;
                    } while (*cp != -1);
                }
            }
        }
    }
    SetVector(out, b[0], b[1], b[2]);
    return 0;
}
