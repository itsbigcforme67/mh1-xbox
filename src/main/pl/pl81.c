/* Player code (SLPM_654.95 0x0014F850-0x0014F864): pl_voice_req */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void flmatGetTrans(f32 *, u8 *);
void RotMatVec(f32 *, f32 *, int);
f32 flSqrt(f32);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

void pl_voice_req(PLW *pl, int se) {
    Pl_se_req2(pl, se, 0, pl->pos, 1, 0);
}
