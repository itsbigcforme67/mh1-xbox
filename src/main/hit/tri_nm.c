/* tri_nm (not built): tri_in_check 1/143 off (operand order of the last add),
 * VectorHitCheck 54/182, old_pos_save 41/52 (register choice, loop guard).
 * tri - SLPM_654.95 0x0010A210-0x0010A800 (f_tri): triangle tests. tri_in_check
 * is true when the three angles under which a triangle's corners are seen
 * from point p add up to nearly 360 degrees (0xF000 of 0x10000), i.e. p lies
 * inside it; VectorHitCheck intersects the segment A -> B with a triangle
 * (2 = A at eye height 30 below is already in its plane, 1 = crosses it,
 * crossing point in out); old_pos_save keeps the joint positions and
 * matrices of the player model for the previous frame. Guesses. */
#include "types.h"
#include "fl.h"

f32 flArcCos(f32);
int flConvertRtoS(f32);
f32 flvecInnerProduct(f32 *, f32 *);
void flvecNormalize(f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
void flmatCopy(void *, void *);
extern f32 sys_old_pos[][3];
extern FLMAT sys_old_mat[];

int tri_in_check(f32 *tri, f32 *p) {
    f32 c[3];
    f32 b[3];
    f32 a[3];
    f32 ab, bc, ca;
    int s;

    c[0] = tri[0] - p[0];
    c[1] = tri[1] - p[1];
    c[2] = tri[2] - p[2];
    b[0] = tri[3] - p[0];
    b[1] = tri[4] - p[1];
    b[2] = tri[5] - p[2];
    a[0] = tri[6] - p[0];
    a[1] = tri[7] - p[1];
    a[2] = tri[8] - p[2];
    flvecNormalize(c);
    flvecNormalize(b);
    flvecNormalize(a);
    ab = flvecInnerProduct(c, b);
    bc = flvecInnerProduct(b, a);
    ca = flvecInnerProduct(a, c);
    if (!(ab <= 1.0f)) ab = 1.0f;
    if (!(bc <= 1.0f)) bc = 1.0f;
    if (!(ca <= 1.0f)) ca = 1.0f;
    if (ab < -1.0f) ab = -1.0f;
    if (bc < -1.0f) bc = -1.0f;
    if (ca < -1.0f) ca = -1.0f;
    ab = flArcCos(ab);
    bc = flArcCos(bc);
    ca = flArcCos(ca);
    s = flConvertRtoS(ab) & 0xFFFF;
    s = (flConvertRtoS(bc) & 0xFFFF) + s;
    if (s + (flConvertRtoS(ca) & 0xFFFF) >= 0xF000) return 1;
    return 0;
}

int VectorHitCheck(f32 *tri, f32 *A, f32 *B, f32 *out) {
    f32 e1[3];
    f32 e0[3];
    f32 n[3];
    f32 u[3];
    f32 w[3];
    f32 dx, dy, dz;
    f32 d, t;
    int r = 0;

    e1[0] = tri[0] - tri[3];
    e1[1] = tri[1] - tri[4];
    e1[2] = tri[2] - tri[5];
    e0[0] = tri[6] - tri[3];
    e0[1] = tri[7] - tri[4];
    e0[2] = tri[8] - tri[5];
    flvecOuterProduct(n, e0, e1);
    flvecNormalize(n);
    dy = B[1] - A[1];
    dz = B[2] - A[2];
    d = (-(n[0] * tri[0]) - (n[1] * tri[1])) - (n[2] * tri[2]);
    dx = B[0] - A[0];
    if ((d + ((n[0] * A[0]) + (n[1] * (A[1] - 30.0f)) + (n[2] * A[2]))) == 0.0f) {
        out[0] = A[0];
        out[1] = A[1] - 30.0f;
        out[2] = A[2];
        if (tri_in_check(tri, out) != 0) return 2;
    }
    t = -(d + ((n[0] * A[0]) + (n[1] * A[1]) + (n[2] * A[2]))) / ((n[0] * dx) + (n[1] * dy) + (n[2] * dz));
    out[0] = A[0] + dx * t;
    out[1] = A[1] + dy * t;
    out[2] = A[2] + dz * t;
    u[0] = A[0] - out[0];
    u[1] = A[1] - out[1];
    u[2] = A[2] - out[2];
    w[0] = B[0] - out[0];
    w[1] = B[1] - out[1];
    w[2] = B[2] - out[2];
    flvecNormalize(u);
    flvecNormalize(w);
    if (flvecInnerProduct(w, u) < 0.0f && tri_in_check(tri, out) != 0) {
        r = 1;
    }
    return r;
}

void old_pos_save(u8 *em) {
    u8 *skin;
    int n;
    s16 i;
    f32 (*pp)[3];
    FLMAT *mp;

    if (em[0] != 0) {
        if (em[1] == 0) {
        } else {
            skin = *(u8 **)(*(u8 **)(em + 0x50C) + 0x24);
            n = *(s16 *)(skin + 0xC2);
            i = 0;
            if (n > 0) {
                pp = sys_old_pos;
                mp = sys_old_mat;
                do {
                    (*pp)[0] = *(f32 *)(skin + 0x30);
                    (*pp)[1] = *(f32 *)(skin + 0x34);
                    (*pp)[2] = *(f32 *)(skin + 0x38);
                    flmatCopy(mp, skin);
                    pp++;
                    mp++;
                    i++;
                    skin += 0x190;
                } while (i < n);
            }
        }
    }
}
