/* set13c - SLPM_654.95 0x00158DB0-0x00158F18 (see set13.c). set13_disp_pos_calc: a point at distance d from the camera along dir. Helper for
 * set13_m: a test whether the line from the camera to a point is blocked by one of the
 * stage's spheres (stage_sphr_tbl: r,x,y,z quads ending with r = -1). */
#include "set.h"
#include "game.h"
#include "fl.h"

/* Capsule handed to hit_cap_pk: two points and a radius. */
typedef struct SET13_CAP {
    f32 p0[3];          /* 0x00 */
    f32 p1[3];          /* 0x0C */
    f32 r;              /* 0x18 */
} SET13_CAP;

extern FLMAT rview_mat;
extern f32 *stage_sphr_tbl[];

void flvecCopy(f32 *, f32 *);
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


int set13_hit_calc(f32 *pos) {
    f32 c[3];
    u8 res[0x10];
    SET13_CAP cap;
    u8 pk[0x40];
    f32 *p;
    f32 r;

    flvecCopy(cap.p0, rview_mat[3]);
    flvecCopy(cap.p1, pos);
    cap.r = 5.0f;
    hit_cap_pk(&cap, pk);
    p = stage_sphr_tbl[game_w.stage];
    if (p != 0) {
        while ((r = p[0]) != -1.0f) {
            c[0] = p[1];
            p = (f32 *)((u8 *)p + 8);
            c[1] = *p++;
            c[2] = *p;
            p++;
            if (hit_cap_sphr_m(pk, c, res, r)) {
                return 1;
            }
        }
    }
    return 0;
}
