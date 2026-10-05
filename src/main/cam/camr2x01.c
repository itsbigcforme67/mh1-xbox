/* SLPM_654.95 0x00223B50-0x00223C90: cam_rail_move_sub .. cam_rail_move_sub. See camr2_nm.c. */
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
        } while (rp->sec > target);
        t = rp->t;
        if (t - goal <= rem) {
            rp->t = goal;
        } else {
            rp->t = t - rem;
        }
    }
}
