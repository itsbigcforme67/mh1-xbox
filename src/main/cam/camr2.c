/* camr2 - SLPM_654.95 0x00223E90-0x00223F8C (f_cam_223B50, rail camera):
 * the camera slides along a stage "rail" (cubic sections, 0x10 bytes per
 * section length at rail+0x10C) towards the point nearest to the player,
 * at most 0.5 per frame. RAILPOS is CameraWork+0x5AC: distance along the
 * section, the same as a 0..1 fraction, section number. Guesses from code. */
#include "types.h"
#include "cam.h"

int GetNearPoint(f32 *, void *, u8 *, void *);
u8 GetNearSection(u8 *, void *);
void ScaleVector(f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);

int cam_rail_move(void *, u8 *, void *, void *);
int cam_rail_move_0(void *, u8 *, void *, void *);

int CamRailMove(CAMW *cw, void *a1, void *a2, int mode) {
    u8 *rail = (u8 *)cw->area;
    int r;

    if (rail == 0) return -1;
    if (mode == 0) {
        r = cam_rail_move_0(&cw->rail_t, rail + 0x20, a1, a2);
    } else {
        r = cam_rail_move(&cw->rail_t, rail + 0x20, a1, a2);
    }
    return r;
}

void CamRailPoint(f32 *out, f32 *c, f32 t) {
    ScaleVector(out, c, t);
    AddVector(out, out, c + 3);
    ScaleVector(out, out, t);
    AddVector(out, out, c + 6);
    ScaleVector(out, out, t);
    AddVector(out, out, c + 9);
}
