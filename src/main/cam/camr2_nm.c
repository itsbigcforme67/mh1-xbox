/* camr2_nm (not built): the whole file; cam_rail_move_sub is 2 instructions off
 * (slt at vs slt v1 in the backward loop), cam_rail_move 83 off (the original
 * keeps the section byte in a stack slot), cam_rail_move_0 1 off (addu operand
 * order). CamRailMove and CamRailPoint match and are built from camr2.c.
 * camr2 - SLPM_654.95 0x00223B50-0x00223F8C (f_cam_223B50, rail camera):
 * the camera slides along a stage "rail" (cubic sections, 0x10 bytes per
 * section length at rail+0x10C) towards the point nearest to the player,
 * at most 0.5 per frame. RAILPOS is CameraWork+0x5AC: distance along the
 * section, the same as a 0..1 fraction, section number. Guesses from code. */
#include "types.h"
#include "cam.h"

typedef struct RAILPOS {
    f32 t;              /* 0x00 distance along the section */
    f32 u;              /* 0x04 t / section length */
    u8 sec;             /* 0x08 section */
} RAILPOS;

int GetNearPoint(f32 *, void *, u8 *, void *);
u8 GetNearSection(u8 *, void *);
void ScaleVector(f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);

#define SEC_LEN(rail, n) (*(f32 *)((n) * 0x10 + (rail) + 0x10C))
#define SEC_LEN2(rail, n) (*(f32 *)((rail) + (n) * 0x10 + 0x10C))

void cam_rail_move_sub(f32 goal, RAILPOS *rp, u8 *rail, int target) {
    f32 rem = 0.5f;
    f32 len;
    f32 t;
    u8 s = rp->sec;

    if (s < target) {
        do {
            t = rp->t;
            len = SEC_LEN(rail, s) - t;
            if (len <= rem) {
                rem -= len;
                rp->sec++;
                rp->t = 0.0f;
            } else {
                rp->t = t + rem;
                return;
            }
        } while ((s = rp->sec) < target);
        t = rp->t;
        if (goal - t <= rem) {
            rp->t = goal;
        } else {
            rp->t = t + rem;
        }
    } else {
        do {
            len = rp->t;
            if (len <= rem) {
                rem -= len;
                rp->sec--;
                rp->t = SEC_LEN(rail, target);
            } else {
                rp->t = len - rem;
                return;
            }
        } while (target < rp->sec);
        t = rp->t;
        if (t - goal <= rem) {
            rp->t = goal;
        } else {
            rp->t = t - rem;
        }
    }
}

int cam_rail_move(RAILPOS *rp, u8 *rail, void *a2, void *a3) {
    u8 sec;
    f32 goal;

    sec = GetNearSection(rail, a3);
    if (GetNearPoint(&goal, a2, rail, a3) != 0) {
        if (rp->sec == sec) {
            if (rp->t < goal) {
                if (goal - rp->t <= 0.5f) {
                    rp->t = goal;
                } else {
                    rp->t = rp->t + 0.5f;
                }
            } else if (!(rp->t <= goal)) {
                if (rp->t - goal <= 0.5f) {
                    rp->t = goal;
                } else {
                    rp->t = rp->t - 0.5f;
                }
            }
        } else {
            cam_rail_move_sub(goal, rp, rail, sec);
        }
        rp->u = rp->t / SEC_LEN(rail, rp->sec);
        return 0;
    }
    return -1;
}

int cam_rail_move_0(RAILPOS *rp, u8 *rail, void *a2, void *a3) {
    rp->sec = GetNearSection(rail, a3);
    if (GetNearPoint(&rp->t, a2, rail, a3) != 0) {
        rp->u = rp->t / *(f32 *)((u8 *)(rp->sec * 0x10) + (int)rail + 0x10C);
        return 0;
    }
    rp->u = 0.0f;
    rp->t = 0.0f;
    return -1;
}

int CamRailMove(CAMW *cw, void *a1, void *a2, int mode) {
    u8 *rail = (u8 *)cw->area;
    int r;

    if (rail == 0) return -1;
    if (mode == 0) {
        r = cam_rail_move_0((RAILPOS *)&cw->rail_t, rail + 0x20, a1, a2);
    } else {
        r = cam_rail_move((RAILPOS *)&cw->rail_t, rail + 0x20, a1, a2);
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
