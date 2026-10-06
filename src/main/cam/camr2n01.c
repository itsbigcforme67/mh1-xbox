/* SLPM_654.95 0x00223DE0-0x00223E8C: cam_rail_move_0 .. cam_rail_move_0. See camr2_nm.c. */
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
