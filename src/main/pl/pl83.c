/* Player code (SLPM_654.95 0x00150D30-0x00150EB4): body_hit_sub_pl (player vs player push) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void pl_body_make(PLW *pl, f32 *out, f32 radius);
void hit_cap_pk(void *, void *);
int hit_cap_cap3_m(void *, void *, f32 *);

void body_hit_sub_pl(PLW *pl, PLW *o) {
    f32 v[3];
    u8 pk1[0x40];
    u8 pk2[0x40];
    f32 b1[8];
    f32 b2[8];
    pl_body_make(pl, b1, 40.0f);
    hit_cap_pk(b1, pk1);
    pl_body_make(o, b2, 40.0f);
    hit_cap_pk(b2, pk2);
    if (hit_cap_cap3_m(pk2, pk1, v) != 0) {
        pl->pos[0] += 0.5f * v[0];
        pl->pos[2] += 0.5f * v[2];
        o->pos[0] += -0.5f * v[0];
        o->pos[2] += -0.5f * v[2];
        if (pl->st == 2 && o->st == 2) {
            pl->pos[1] += 0.5f * v[1];
            o->pos[1] += -0.5f * v[1];
        } else if (pl->st == 2) {
            pl->pos[1] += v[1];
        } else if (o->st == 2) {
            o->pos[1] -= v[1];
        }
        pl->work7EC = 1;
        o->work7EC = 1;
    }
}
