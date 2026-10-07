/* fl motion evaluation (SLPM_654.95 0x00174300-0x00174544): flGetMotionMatrix. Evaluates the f-curves of one motion node into 10 floats
 * (scale xyz, rotation xyz, translation xyz; indexes 3..5 are angle values stored in 1/(2^16/2pi) steps, the others in 1/16 steps)
 * and builds the node matrix from them. */
#include "types.h"

typedef struct MOT {
    u16 flags;     /* 0x0: bits 12..15 = format (1 raw, 2 scaled) */
    s16 cnt;       /* 0x2 */
    s32 off;       /* 0x4 */
} MOT;

typedef struct CURVE {
    u8 x0;
    u8 idx;
    u8 x2[6];
} CURVE;

extern s32 base_addr_0038A25C;
f32 flFCVGetValue2(f32, CURVE *, s16 *);
void flmatMakeScale(f32, f32, f32, void *);
void flmatRotXYZ33(f32, f32, f32, void *);

int flGetMotionMatrix(MOT *mot, f32 *init, f32 t, f32 *v, f32 *mat, s16 *hint) {
    int i;
    int j;
    f32 *d;
    CURVE *c;
    u16 idx;

    i = 0;
    d = v;
    do {
        i += 5;
        d[0] = init[0];
        d[1] = init[1];
        d[2] = init[2];
        d[3] = init[3];
        d[4] = init[4];
        init += 5;
        d += 5;
    } while (i < 10);
    if (mot != 0) {
        switch (mot->flags & 0xF000) {
        case 0x1000:
            j = 0;
            c = (CURVE *)(mot->off + base_addr_0038A25C);
            for (; j < mot->cnt; j++) {
                idx = c->idx;
                v[idx] = flFCVGetValue2(t, c, hint + idx);
                c++;
            }
            break;
        case 0x2000:
            j = 0;
            c = (CURVE *)(mot->off + base_addr_0038A25C);
            for (; j < mot->cnt; j++) {
                idx = c->idx;
                v[idx] = flFCVGetValue2(t, c, hint + idx);
                switch (idx) {
                case 3:
                case 4:
                case 5:
                    v[idx] = v[idx] * 0.0003834952f;
                    break;
                default:
                    v[idx] = v[idx] / 16.0f;
                    break;
                }
                c++;
            }
            break;
        }
    }
    flmatMakeScale(v[0], v[1], v[2], mat);
    flmatRotXYZ33(v[3], v[4], v[5], mat);
    mat[12] = v[6];
    mat[13] = v[7];
    mat[14] = v[8];
    return 1;
}
