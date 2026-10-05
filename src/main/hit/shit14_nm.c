/* shit14_nm (not built): PushAdjust3 (0x00117CA0, 1006 instructions) of
 * f_sphr: from the wall contacts that sphr_face_o3/o4 collected
 * (hit_poly_num entries) work out how far to push the position pos out of
 * the walls and move it. One contact: hosei_sub's push, limited to 1.8 *
 * the sweep length. Several: contacts on a polygon face (hit_area_out 0,
 * list A) and edge contacts (list B) are sorted; edge contacts that are
 * covered by a face contact (same normal: 1, behind the face: 2, same
 * touching point: 3) are dropped, the remaining pushes are summed per axis
 * with add_vec_sub2 (max positive / max negative), merged and applied. Every
 * used contact is also written to hited_wall_no[]. Guesses from the code;
 * the decompiler lost some index details, marked below. */
#include "types.h"
#include "hit3.h"

void PointToPoint(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void SetVector(f32 *, f32, f32, f32);
void add_vec_sub2(f32 *, f32 *, f32);
f32 flvecCalcLength(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void flvecNormalize(f32 *);
int hosei_sub(HSWEEP *, f32 *, s8, u8 *);
extern s8 hited_wall_no[];

/* Edge-sharing test: do polygons pa and pb share two vertices? If so
 * *odd = the vertex of pb that is not shared; returns 1, else 0. */
static int share_edge(HPOLY *pa, HPOLY *pb, f32 *odd) {
    u8 idx[4];
    int n = 0;
    int i, j, k;
    int mask;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (pa->v[i][0] == pb->v[j][0] && pa->v[i][1] == pb->v[j][1] && pa->v[i][2] == pb->v[j][2]) {
                idx[n++] = j;
                break;
            }
        }
    }
    if (n < 2) return 0;
    mask = (1 << idx[0]) | (1 << idx[1]);
    for (k = 0; k < 3; k++) {
        if (!(mask & (1 << k))) break;
    }
    if (k == 3) return 1;           /* all shared: nothing to measure */
    SetVector(odd, pb->v[k][0], pb->v[k][1], pb->v[k][2]);
    return 2;
}

int PushAdjust3(HSWEEP *sw, f32 *pos, u8 *flagp) {
    s8 A[0x20];         /* face contacts (hit_area_out == 0) */
    s8 B[0x20];         /* edge contacts */
    s8 cov[0x20];       /* edge contacts that are covered */
    s8 why[0x20];       /* why: 1 same normal, 2 behind a face, 3 same point */
    f32 v[3];
    f32 e[3];
    f32 c[3];
    f32 d[3];
    f32 box[12];
    f32 L;
    int nA = 0;
    int nB = 0;
    int nd = 0;
    int i, j, k;
    int ia, ib;
    int r;
    HPOLY *pa;
    HPOLY *pb;
    f32 t;

    switch (hit_poly_num) {
    case 0:
        break;
    case 1:
        if (hosei_sub(sw, v, 0, flagp) != 0) {
            SetVector(e, v[0], v[1], v[2]);
            L = flvecCalcLength(e);
            if (sw->len != 0.0f) {
                t = 1.8f * sw->len;
                if (!(L <= t)) L = t;
            }
            flvecNormalize(e);
            ScaleVector(v, e, L);
            pos[0] += v[0];
            pos[1] += v[1];
            pos[2] += v[2];
            hited_wall_no[hited_poly_num] = 0;
            hited_poly_num++;
        }
        break;
    default:
        for (i = 0; i < hit_poly_num; i++) {
            if (hit_area_out[i] == 0) {
                A[nA++] = i;
            } else {
                B[nB++] = i;
            }
        }
        if (nA != 0 && nB != 0) {
            for (ia = 0; ia < nA; ia++) {
                for (ib = 0; ib < nB; ib++) {
                    /* (the original scans cov[] for B[ib] here and ignores the result) */
                    pa = &hit_decision[B[ib]];
                    pb = &hit_decision[A[ia]];
                    if (pb->n[0] == pa->n[0] && pb->n[1] == pa->n[1] && pb->n[2] == pa->n[2]) {
                        why[nd] = 1;
                        cov[nd] = B[ib];
                        nd++;
                    } else if (pb->n[1] != 1.0f && pb->n[1] != -1.0f && pa->n[1] != 1.0f && pa->n[1] != -1.0f) {
                        r = share_edge(pb, pa, c);
                        if (r == 2) {
                            PointToPoint(d, c, hit_decision[A[ia]].v[0]);
                            if (!(flvecInnerProduct(d, hit_decision[A[ia]].n) <= 0.0f)) {
                                why[nd] = 2;
                                cov[nd] = B[ib];
                                nd++;
                            }
                        }
                    }
                }
            }
        }
        if (nB >= 2) {
            for (i = 0; i < nB - 1; i++) {
                for (k = 0, r = 0; k < nd; k++) {
                    if (B[i] == cov[k]) r = 1;
                }
                if (r != 1) {
                    for (j = 1; j < nB; j++) {
                        for (k = 0, r = 0; k < nd; k++) {
                            if (B[j] == cov[k]) r = 1;
                        }
                        if (r != 1 && B[i] != B[j]) {
                            if (hit_near_point[B[i]][0] == hit_near_point[B[j]][0] &&
                                hit_near_point[B[i]][1] == hit_near_point[B[j]][1] &&
                                hit_near_point[B[i]][2] == hit_near_point[B[j]][2]) {
                                why[nd] = 3;
                                cov[nd] = B[j];
                                nd++;
                            }
                        }
                    }
                }
            }
        }
        if (nd > 0 && nd < nB) {
            int n0 = nd;
            for (k = 0; k < n0; k++) {
                if (why[k] == 1) {
                    for (j = 0; j < nB; j++) {
                        if (B[j] != cov[k]) {
                            pa = &hit_decision[cov[k]];
                            pb = &hit_decision[B[j]];
                            /* the original pairs covered entry k with A[k] here (index kept as written) */
                            if (pa->n[1] != 1.0f && pa->n[1] != -1.0f && pb->n[1] != 1.0f && pb->n[1] != -1.0f) {
                                r = share_edge(pa, pb, c);
                                if (r == 2) {
                                    PointToPoint(d, c, hit_decision[A[k]].v[0]);
                                    if (!(flvecInnerProduct(d, hit_decision[A[k]].n) <= 0.0f)) {
                                        why[nd] = 2;
                                        cov[nd] = B[j];
                                        nd++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        for (i = 0; i < 12; i++) box[i] = 0.0f;
        for (i = 0; i < nA; i++) {
            if (hosei_sub(sw, v, A[i], flagp) != 0) {
                add_vec_sub2(v, box, sw->r);
                hited_wall_no[hited_poly_num] = A[i];
                hited_poly_num++;
            }
        }
        for (i = 0; i < nB; i++) {
            r = 0;
            for (k = 0; k < nd; k++) {
                if (cov[k] == B[i]) r = 1;
            }
            if (r == 0 && hosei_sub(sw, v, B[i], flagp) != 0) {
                add_vec_sub2(v, box, sw->r);
                hited_wall_no[hited_poly_num] = B[i];
                hited_poly_num++;
            }
        }
        for (k = 0; k < 3; k++) {
            if (box[k] != 0.0f && box[3 + k] != 0.0f) {
                v[k] = box[9 + k] + (box[6 + k] + ((box[k] + box[3 + k]) / 2.0f));
            } else if (box[3 + k] == 0.0f) {
                v[k] = box[k] + box[6 + k];
            } else {
                v[k] = box[3 + k] + box[9 + k];
            }
        }
        SetVector(e, v[0], v[1], v[2]);
        L = flvecCalcLength(e);
        if (sw->len != 0.0f) {
            t = 1.8f * sw->len;
            if (!(L <= t)) L = t;
        }
        flvecNormalize(e);
        ScaleVector(v, e, L);
        pos[0] += v[0];
        pos[1] += v[1];
        pos[2] += v[2];
        break;
    }
    return hit_poly_num;
}
