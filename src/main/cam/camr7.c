/* camr7 - SLPM_654.95 0x00225370-0x00225510 (f_cam_223B50, rail camera):
 * k_HitWallCamera pushes the camera out of walls: for every push01 entry
 * (x, y, z offset and radius; radius -1 ends the table) it tests the wall
 * hit between target+offset and camera+offset and stores the distance to
 * the hit (0 when there is none). Returns 0.0f always. */
#include "types.h"
#include "game.h"

extern f32 push01[][4];
void SetVector(f32 *, f32, f32, f32);
u8 GetWallHitBit2(f32, f32 *, f32 *, f32 *, int);
f32 flvecCalcDistance(f32 *, f32 *);

f32 k_HitWallCamera(f32 *cam, f32 *tar, f32 *dist) {
    f32 a[3];
    f32 b[3];
    f32 c[3];
    f32 (*pp)[4] = push01;
    int hit;

    if (pp[0][3] != -1.0f) {
        do {
            a[0] = tar[0] + (*pp)[0];
            a[1] = tar[1] + (*pp)[1];
            a[2] = tar[2] + (*pp)[2];
            b[0] = cam[0] + (*pp)[0];
            b[1] = cam[1] + (*pp)[1];
            b[2] = cam[2] + (*pp)[2];
            SetVector(c, cam[0], cam[1], cam[2]);
            if (game_w.gate_open != 0) {
                hit = GetWallHitBit2((*pp)[3], a, b, cam, 0xC001);
            } else {
                hit = GetWallHitBit2((*pp)[3], a, b, cam, 0x8001);
            }
            if ((u8)hit != 0) {
                *dist = flvecCalcDistance(c, cam);
            } else {
                *dist = 0.0f;
            }
            pp++;
        } while ((*pp)[3] != -1.0f);
    }
    return 0.0f;
}
