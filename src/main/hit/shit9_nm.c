/* shit9_nm (not built): GetFloorSlide (0x0011C4E0) and hosei_sub
 * (0x00118C60) of f_sphr. GetFloorSlide: polygon under the entity (pos at
 * +0xAC) and, when it is steep enough (check_angle >= 0x1500), the slide
 * push vector (check_slide) written to out; optionally applied to the
 * position (flag). Returns 0 sliding, 1 not (stands above), -1 off the
 * ground file. */
#include "types.h"
#include "hit3.h"

#define EF(e, T, o) (*(T *)((u8 *)(e) + (o)))

s32 *GetGroundTblAdrs(f32 *);
int GroundFieldInCheck(f32 *);
int PointHitCheckF3(f32 *, f32 *);
void check_angle(f32 *, s32 *);
int check_slide(HPOLY *, u8 *, s32 *, f32 *, f32);
extern f32 slide_tbl;

int GetFloorSlide(void *ent, f32 *out, int flag) {
    f32 pt[4];
    f32 h[8];
    f32 tri[12];
    f32 hf[8];
    HPOLY *pp[8];
    HPOLY *pf[8];
    HPOLY *best;
    f32 *pos = (f32 *)((u8 *)ent + 0xAC);
    s32 ang = 0;
    s32 *cell;
    HPOLY *pl;
    s8 n = 0;
    s8 m;
    s8 any;
    s8 c;
    s8 i;
    int r;
    f32 *hp = h;
    HPOLY **pq = pp;
    f32 hr;

    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;
    if (GroundFieldInCheck(pos) == 0) return -1;
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
                *hp = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                *pq = pl;
                hp++;
                pq++;
            }
            cell++;
        } while (*cell != -1);
    }
    if (n == 0) return -1;
    m = n;
    if (m != 1) {
        any = 0;
        c = 0;
        i = 0;
        if (m > 0) {
            hp = h;
            pq = pp;
            do {
                if (*hp <= 50.0f + pos[1]) {
                    hf[c] = *hp;
                    any = 1;
                    c++;
                    pf[c - 1] = *pq;
                }
                hp++;
                i++;
                pq++;
            } while (i < m);
        }
        if (any == 1) {
            if (c == 1) {
                best = pf[0];
                hr = hf[0];
            } else {
                i = 0;
                if (c > 0) {
                    for (; i < c; i++) {
                        if (i == 0 || h[0] < hf[i]) {
                            h[0] = hf[i];
                            pp[0] = pf[i];
                        }
                    }
                }
                hr = h[0];
                best = pp[0];
            }
        } else {
            i = 0;
            if (m > 0) {
                for (; i < m; i++) {
                    if (i == 0 || !(hf[0] <= h[i])) {
                        hf[0] = h[i];
                        pf[0] = pp[i];
                    }
                }
            }
            hr = hf[0];
            best = pf[0];
        }
    } else {
        best = pp[0];
        hr = h[0];
    }
    if (!(hr < pos[1])) {
        check_angle(best->n, &ang);
        r = check_slide(best, (u8 *)ent, &ang, out, slide_tbl) & 0xFF;
        if (flag != 0 && r != 0) {
            pos[0] += out[0];
            pos[1] += out[1];
            pos[2] += out[2];
        }
        return 0;
    }
    return 1;
}
