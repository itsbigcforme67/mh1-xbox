/* shit13_nm (not built): hosei_sub (0x00118C60) of f_sphr: turns one wall
 * contact recorded by sphr_face_o3/o4 (hit_decision[idx] = the polygon,
 * hit_near_point = touching point, hit_hosei_base = sphere centre,
 * hit_side, hit_area_out = 1 for edge contacts) into the vector that pushes
 * the sphere out of the wall (out). Contacts on the polygon face push along
 * the polygon normal by the radius; edge contacts push away from the edge
 * point. When w20 is set the push is kept horizontal. Returns 0 when there
 * is nothing to do. *flagp is set for contacts seen from the back (side 1).
 * Guesses from the code. */
#include "types.h"
#include "hit3.h"

void AddVector(f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void SetVector(f32 *, f32, f32, f32);
f32 flvecCalcLength(f32 *);
void flvecNormalize(f32 *);

int hosei_sub(HSWEEP *sw, f32 *out, s8 idx, u8 *flagp) {
    f32 a[3];
    f32 b[3];
    f32 c[3];
    f32 e[3];
    f32 len;
    f32 *np;
    int h = 0;
    int i = idx;

    if (hit_area_out[i] != 1) {
        SetVector(e, hit_decision[i].n[0], hit_decision[i].n[1], hit_decision[i].n[2]);
        if (sw->w20 == 1) {
            if (e[0] == 0.0f && e[1] != 0.0f && e[2] == 0.0f) {
            } else {
                e[1] = 0.0f;
                h = 1;
                flvecNormalize(e);
            }
        }
        ScaleVector(b, e, sw->r);
        AddVector(a, hit_near_point[i], b);
        if (h != 0) a[1] = hit_hosei_base[i][1];
        PointToPoint(b, a, hit_hosei_base[i]);
        len = flvecCalcLength(b);
        flvecNormalize(b);
        ScaleVector(a, b, len);
    } else {
        switch (hit_side[i]) {
        case 0:
            np = hit_near_point[i];
            PointToPoint(a, hit_hosei_base[i], np);
            break;
        default:
        case 1:
            np = hit_near_point[i];
            PointToPoint(a, hit_kouten[i], np);
            break;
        }
        if (sw->w20 == 1 && (a[0] != 0.0f || a[1] == 0.0f || a[2] != 0.0f)) {
            a[1] = 0.0f;
            h = 1;
        }
        flvecNormalize(a);
        ScaleVector(b, a, sw->r);
        AddVector(a, np, b);
        SetVector(c, hit_hosei_base[i][0], hit_hosei_base[i][1], hit_hosei_base[i][2]);
        if (h == 1) c[1] = a[1];
        PointToPoint(b, a, c);
        len = flvecCalcLength(b);
        if (len == 0.0f) return 0;
        flvecNormalize(b);
        ScaleVector(a, b, len);
    }
    if (hit_side[i] == 1) *flagp = 1;
    SetVector(out, a[0], a[1], a[2]);
    return 1;
}
