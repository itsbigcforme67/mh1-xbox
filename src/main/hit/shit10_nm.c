/* shit10_nm (not built): sphere-vs-wall-polygon tests of f_sphr. sphr_face_o4
 * (0x00117190) tests a swept sphere (HSWEEP: p1 -> p0, radius r) against one
 * wall polygon: first against the face (closest point of the plane inside
 * the triangle), else against its three edges / corners (nearest distance
 * below r). A hit is appended to hit_decision[] (polygon copy) with the
 * touching point (hit_near_point), the sphere centre (hit_hosei_base),
 * which side it came from (hit_side) and whether it is an edge hit
 * (hit_area_out = 1). Guesses from the code, ordering unverified. */
#include "types.h"
#include "hit3.h"

void AddVector(f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void SetVector(f32 *, f32, f32, f32);
void NvecFloatAdjust(f32 *, f32 *, void *);
f32 flAbs(f32);
f32 flvecCalcDistance(f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void flvecNormalize(f32 *);
int FaceLinePos(f32 *, f32 *, f32 *, f32 *);
int NormalClipFace(f32 *, f32 *, f32 *);

/* sphr_face_o3 (0x00116A00): same test for players (no radius-adjusted height
 * test, nearest points keep their own y). */
int sphr_face_o3(HSWEEP *sw, HPOLY *pl, f32 *pos) {
    f32 e[3];
    f32 q[3];
    f32 d[3];
    f32 best[3];
    f32 c[3];
    f32 tri[4][3];
    f32 t;
    f32 bd;
    f32 *vp;
    int k;
    int s4;
    s8 side;
    f32 *pt;

    if (sw->w22 != 0) {
        t = pos[1];
        if (!(t < pl->v[0][1]) && !(t < pl->v[1][1]) && !(t < pl->v[2][1])) return 0;
    }
    PointToPoint(e, sw->p1, pl->v[0]);
    if (flvecInnerProduct(e, pl->n) <= 0.0f) return 0;
    PointToPoint(e, sw->p0, pl->v[0]);
    t = flvecInnerProduct(e, pl->n);
    side = 0;
    if (t <= 0.0f) side = 1;
    q[0] = sw->p0[0];
    q[1] = sw->p0[1];
    q[2] = sw->p0[2];
    if (flAbs(t) < sw->r) {
        ScaleVector(c, pl->n, t);
        PointToPoint(c, sw->p0, c);
    } else {
        if (FaceLinePos(sw->p0, pl->n, pl->v[0], c) == 0) return 0;
        side = (s8)(side | 2);
    }
    if (NormalClipFace(c, pl->v[0], pl->n) != 0) {
        if (hit_poly_num >= 0x13) return 0;
        hit_decision[hit_poly_num] = *pl;
        NvecFloatAdjust(hit_decision[hit_poly_num].n, hit_decision[hit_poly_num].n, &hit_decision[hit_poly_num]);
        hit_hosei_base[hit_poly_num][0] = q[0];
        hit_hosei_base[hit_poly_num][1] = q[1];
        hit_hosei_base[hit_poly_num][2] = q[2];
        hit_side[hit_poly_num] = side;
        hit_area_out[hit_poly_num] = 0;
        hit_near_point[hit_poly_num][0] = c[0];
        hit_near_point[hit_poly_num][1] = c[1];
        hit_near_point[hit_poly_num][2] = c[2];
        hit_poly_num++;
        return 1;
    }
    bd = 8000.0f;
    hit_area_out[hit_poly_num] = 1;
    for (k = 0; k < 3; k++) {
        tri[k][0] = pl->v[k][0];
        tri[k][1] = pl->v[k][1];
        tri[k][2] = pl->v[k][2];
    }
    tri[3][0] = pl->v[0][0];
    tri[3][1] = pl->v[0][1];
    tri[3][2] = pl->v[0][2];
    SetVector(d, c[0], c[1], c[2]);       /* d = projected centre (spB0) */
    pt = (side & 2) ? d : sw->p0;
    vp = tri[0];
    for (k = 0; k < 3; k++, vp += 3) {
        s4 = 0;
        PointToPoint(e, pt, vp);
        PointToPoint(q, vp + 3, vp);
        flvecNormalize(q);
        if (flvecInnerProduct(e, q) < 0.0f) s4 = 1;
        PointToPoint(e, pt, vp + 3);
        t = flvecInnerProduct(e, q);
        if (!(t <= 0.0f)) s4 |= 2;
        switch (s4) {
        case 0:
            ScaleVector(c, q, t);
            AddVector(c, c, vp + 3);
            break;
        case 1:
            c[0] = vp[0];
            c[1] = vp[1];
            c[2] = vp[2];
            break;
        case 2:
            c[0] = vp[3];
            c[1] = vp[4];
            c[2] = vp[5];
            break;
        case 3:
            return 0;
        }
        t = flvecCalcDistance(pt, c);
        if (t < bd) {
            bd = t;
            best[0] = c[0];
            best[1] = c[1];
            best[2] = c[2];
        }
    }
    if (bd < sw->r) {
        if (hit_poly_num >= 0x13) return 0;
        hit_decision[hit_poly_num] = *pl;
        NvecFloatAdjust(hit_decision[hit_poly_num].n, hit_decision[hit_poly_num].n, &hit_decision[hit_poly_num]);
        hit_hosei_base[hit_poly_num][0] = sw->p0[0];
        hit_hosei_base[hit_poly_num][1] = sw->p0[1];
        hit_hosei_base[hit_poly_num][2] = sw->p0[2];
        hit_side[hit_poly_num] = side;
        hit_near_point[hit_poly_num][0] = best[0];
        hit_near_point[hit_poly_num][1] = best[1];
        hit_near_point[hit_poly_num][2] = best[2];
        hit_kouten[hit_poly_num][0] = d[0];
        hit_kouten[hit_poly_num][1] = d[1];
        hit_kouten[hit_poly_num][2] = d[2];
        hit_poly_num++;
        return 1;
    }
    return 0;
}


/* sphr_face_o4 (0x00117190): monsters. */
int sphr_face_o4(HSWEEP *sw, HPOLY *pl, f32 *pos) {
    f32 e[3];
    f32 q[3];
    f32 d[3];
    f32 best[3];
    f32 c[3];
    f32 tri[4][3];
    f32 t;
    f32 bd;
    f32 *vp;
    int k;
    int s4;
    s8 side;
    f32 *pt;

    if (sw->w22 != 0) {
        t = pos[1] - sw->r;
        if (!(t <= pl->v[0][1]) && !(t <= pl->v[1][1]) && !(t <= pl->v[2][1])) return 0;
    }
    PointToPoint(e, sw->p1, pl->v[0]);
    if (flvecInnerProduct(e, pl->n) <= 0.0f) return 0;
    PointToPoint(e, sw->p0, pl->v[0]);
    t = flvecInnerProduct(e, pl->n);
    side = 0;
    if (t <= 0.0f) side = 1;
    q[0] = sw->p0[0];
    q[1] = sw->p0[1];
    q[2] = sw->p0[2];
    if (flAbs(t) < sw->r) {
        ScaleVector(c, pl->n, t);
        PointToPoint(c, sw->p0, c);
    } else {
        if (FaceLinePos(sw->p0, pl->n, pl->v[0], c) == 0) return 0;
        side = (s8)(side | 2);
    }
    if (NormalClipFace(c, pl->v[0], pl->n) != 0) {
        if (hit_poly_num >= 0x13) return 0;
        hit_decision[hit_poly_num] = *pl;
        NvecFloatAdjust(hit_decision[hit_poly_num].n, hit_decision[hit_poly_num].n, &hit_decision[hit_poly_num]);
        hit_hosei_base[hit_poly_num][0] = q[0];
        hit_hosei_base[hit_poly_num][1] = q[1];
        hit_hosei_base[hit_poly_num][2] = q[2];
        hit_side[hit_poly_num] = side;
        hit_area_out[hit_poly_num] = 0;
        hit_near_point[hit_poly_num][0] = c[0];
        hit_near_point[hit_poly_num][1] = c[1];
        hit_near_point[hit_poly_num][2] = c[2];
        hit_near_point[hit_poly_num][1] = q[1];
        hit_poly_num++;
        return 1;
    }
    bd = 8000.0f;
    hit_area_out[hit_poly_num] = 1;
    for (k = 0; k < 3; k++) {
        tri[k][0] = pl->v[k][0];
        tri[k][1] = pl->v[k][1];
        tri[k][2] = pl->v[k][2];
    }
    tri[3][0] = pl->v[0][0];
    tri[3][1] = pl->v[0][1];
    tri[3][2] = pl->v[0][2];
    SetVector(d, c[0], c[1], c[2]);       /* d = projected centre (spB0) */
    pt = (side & 2) ? d : sw->p0;
    vp = tri[0];
    for (k = 0; k < 3; k++, vp += 3) {
        s4 = 0;
        PointToPoint(e, pt, vp);
        PointToPoint(q, vp + 3, vp);
        flvecNormalize(q);
        if (flvecInnerProduct(e, q) < 0.0f) s4 = 1;
        PointToPoint(e, pt, vp + 3);
        t = flvecInnerProduct(e, q);
        if (!(t <= 0.0f)) s4 |= 2;
        switch (s4) {
        case 0:
            ScaleVector(c, q, t);
            AddVector(c, c, vp + 3);
            break;
        case 1:
            c[0] = vp[0];
            c[1] = vp[1];
            c[2] = vp[2];
            break;
        case 2:
            c[0] = vp[3];
            c[1] = vp[4];
            c[2] = vp[5];
            break;
        case 3:
            return 0;
        }
        t = flvecCalcDistance(pt, c);
        if (t < bd) {
            bd = t;
            best[0] = c[0];
            best[1] = c[1];
            best[2] = c[2];
        }
    }
    if (bd < sw->r) {
        if (hit_poly_num >= 0x13) return 0;
        hit_decision[hit_poly_num] = *pl;
        NvecFloatAdjust(hit_decision[hit_poly_num].n, hit_decision[hit_poly_num].n, &hit_decision[hit_poly_num]);
        hit_hosei_base[hit_poly_num][0] = sw->p0[0];
        hit_hosei_base[hit_poly_num][1] = sw->p0[1];
        hit_hosei_base[hit_poly_num][2] = sw->p0[2];
        hit_side[hit_poly_num] = side;
        hit_near_point[hit_poly_num][0] = best[0];
        hit_near_point[hit_poly_num][1] = best[1];
        hit_near_point[hit_poly_num][2] = best[2];
        hit_kouten[hit_poly_num][0] = d[0];
        hit_kouten[hit_poly_num][1] = d[1];
        hit_kouten[hit_poly_num][2] = d[2];
        hit_near_point[hit_poly_num][1] = hit_hosei_base[hit_poly_num][1];
        hit_kouten[hit_poly_num][1] = hit_hosei_base[hit_poly_num][1];
        hit_poly_num++;
        return 1;
    }
    return 0;
}

/* ---- GetWallHitBit2 (0x00115EB0): sweep a sphere of radius r from b back to a
 * (both positions), test every wall polygon (not masked by `mask`) in the grid
 * cells around, then let PushAdjust3 work out the push. pos is the entity
 * position used for the height test. Returns PushAdjust3's result, -1 if b is
 * outside the wall grid. ---- */
extern f32 lit_638_002E8610[8];
int WallFieldInCheck(f32 *);
s32 *GetWallTblAdrs(f32 *);
int BlockPlaceCgeck(f32 *);
int PushAdjust3(HSWEEP *, f32 *, u8 *);
int sphr_face_o4(HSWEEP *, HPOLY *, f32 *);

int GetWallHitBit2(f32 r, f32 *a, f32 *b, f32 *pos, int mask) {
    HSWEEP sw;
    f32 q[8];
    f32 p[3];
    HPOLY *ring[20];
    u8 tmp[8];
    int n2;
    int nr = 0;
    int nh = 0;
    int found = 0;
    int ix;
    int iz;
    int i;
    int quad = 0;
    int half;
    f32 cs;
    f32 *cell;
    HPOLY *pl;
    s32 *cp;

    q[0] = lit_638_002E8610[0];
    q[1] = lit_638_002E8610[1];
    q[2] = lit_638_002E8610[2];
    q[3] = lit_638_002E8610[3];
    q[4] = lit_638_002E8610[4];
    q[5] = lit_638_002E8610[5];
    q[6] = lit_638_002E8610[6];
    q[7] = lit_638_002E8610[7];
    if (WallFieldInCheck(b) == 0) return -1;
    hit_poly_num = 0;
    sw.p0[0] = b[0];
    sw.p0[1] = b[1];
    sw.p0[2] = b[2];
    sw.p1[0] = a[0];
    sw.p1[1] = a[1];
    sw.p1[2] = a[2];
    sw.r = r;
    sw.w20 = 0;
    sw.w22 = 0;
    sw.len = flvecCalcDistance(sw.p0, sw.p1);
    if ((u32)diorama_w.wcsz >= (u32)diorama_w.wcsx) {
        cs = (u32)diorama_w.wcsx;
    } else {
        cs = (u32)diorama_w.wcsz;
    }
    n2 = (int)((sw.len + 2.0f * r) / cs) + 2;
    if (n2 == 2) quad = BlockPlaceCgeck(b);
    for (ix = 0; ix < n2; ix++) {
        half = n2 / 2;
        for (iz = 0; iz < n2; iz++) {
            p[0] = a[0];
            p[1] = a[1];
            p[2] = a[2];
            if (n2 == 2) {
                p[0] += (u32)diorama_w.wcsx * q[quad * 2];
                p[2] += (u32)diorama_w.wcsz * q[quad * 2 + 1];
            } else {
                p[0] -= half * (f32)(u32)diorama_w.wcsx;
                p[2] -= half * (f32)(u32)diorama_w.wcsz;
            }
            p[0] += (f32)((u32)diorama_w.wcsx * ix);
            p[2] += (f32)((u32)diorama_w.wcsz * iz);
            if (WallFieldInCheck(p) == 1) {
                cp = GetWallTblAdrs(p);
                if (*cp != -1) {
                    do {
                        pl = (HPOLY *)*cp;
                        if (!(pl->h2 & (mask & 0xFFFF))) {
                            if (nr != 0) {
                                found = 0;
                                for (i = 0; i < nr; i++) {
                                    if (pl == ring[i]) found = 1;
                                }
                            }
                            if (nr == 0 || found == 0) {
                                if (nh != 0) {
                                    found = 0;
                                    for (i = 0; i < nh; i++) {
                                        if (pl == hit_wall[i]) found = 1;
                                    }
                                }
                                if (found == 0) {
                                    if (sphr_face_o4(&sw, pl, pos) != 0) {
                                        hit_wall[nh++] = pl;
                                    }
                                    ring[nr++] = pl;
                                    if (nr >= 0x14) nr = 0;
                                }
                            }
                        }
                        cp++;
                    } while (*cp != -1);
                }
            }
        }
    }
    return PushAdjust3(&sw, pos, tmp);
}
