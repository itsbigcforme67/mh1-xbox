/* weapon2 - SLPM_654.95 0x00164D60 (f_weapon, part 3): mini sight marker
 * drawn at a point in front of the player (sight_disp2: along the weapon's
 * -Z, sight_disp_ballista: 1000 ahead of the player's yaw). Guess from code. */
#include "types.h"
#include "pl.h"
#include "fl.h"

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void flvecApplyMat33(f32 *, f32 *, f32 *);
void flmatGetTrans(f32 *, f32 *);
void flvecrRotTransPers(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void Put_mini_sight(f32, int, u32);

void sight_disp2(PLW *pl) {
    f32 v[3];
    f32 o[3];
    FLMAT m;
    f32 p[4];
    f32 *vy = &v[1];
    f32 *vz = &v[2];

    v[0] = 0.0f;
    *vy = 0.0f;
    *vz = -1000.0f;
    flvecApplyMat33(o, v, (f32 *)(pl->mdl148 + 0x40));
    flmatGetTrans(v, (f32 *)(pl->mdl148 + 0x40));
    v[0] = v[0] + o[0];
    *vy = *vy + o[1];
    *vz = *vz + o[2];
    flmatInit(&m);
    flSetRenderState(0x1A, (u32)m);
    flvecrRotTransPers(p, v);
    if (!(p[3] <= 0.0f)) {
        Put_mini_sight(p[0], (int)p[1], 0xF0FF0000);
    }
}

void sight_disp_ballista(PLW *pl) {
    f32 v[3];
    FLMAT m;
    f32 p[4];
    f32 *vy = &v[1];
    f32 *vz = &v[2];

    v[0] = 0.0f;
    *vy = 100.0f;
    *vz = 1000.0f;
    flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
    v[0] = v[0] + pl->pos[0];
    *vy = *vy + pl->pos[1];
    *vz = *vz + pl->pos[2];
    flmatInit(&m);
    flSetRenderState(0x1A, (u32)m);
    flvecrRotTransPers(p, v);
    if (!(p[3] <= 0.0f)) {
        Put_mini_sight(p[0], (int)p[1], 0xF0FF0000);
    }
}
