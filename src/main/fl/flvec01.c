/* fl library vector rotations (SLPM_654.95 0x00172FF0-0x001730F0): flvecRotX / flvecRotY rotate a 3 float vector about the X / Y axis by an angle
 * (copy to a temporary, sin/cos from flSinCos, two products each). */
#include "types.h"

typedef struct FV3 { f32 x, y, z; } FV3;

void flSinCos(f32 *, f32 *, f32);

void flvecRotX(f32 *v, f32 a) {
    f32 s;
    f32 c;
    FV3 t;

    t = *(FV3 *)v;
    flSinCos(&s, &c, a);
    v[1] = c * t.y - s * t.z;
    v[2] = s * t.y + c * t.z;
}

void flvecRotY(f32 *v, f32 a) {
    f32 s;
    f32 c;
    FV3 t;

    t = *(FV3 *)v;
    flSinCos(&s, &c, a);
    v[0] = c * t.x + s * t.z;
    v[2] = -s * t.x + c * t.z;
}
