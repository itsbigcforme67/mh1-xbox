/* Player code (SLPM_654.95 0x00154330-0x0015440C): Get_tuto_id */
#include "pl.h"
#include "game.h"
#include "plf.h"
typedef struct TUTO_REC {
    u16 id;     /* 0x00 */
    u16 _02;
    f32 x;      /* 0x04 */
    f32 y;      /* 0x08 */
    f32 z;      /* 0x0C */
    f32 r;      /* 0x10 */
} TUTO_REC;
f32 flSqrt(f32);
f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];
s16 Pl_item_num_ck(PLW *, int);
void adx_se_set(PLW *, int);
void init_set_work();
void init_eft_work();
void init_shell_work();
void init_item_work();
void clr_set_work();
void clr_eft_work();
void clr_shell_work();
void clr_item_work();
void clr_used_heap(int, int);

u16 Get_tuto_id(TUTO_REC *t) {
    PLW *pl = &player_work[game_w.master];
    f32 dx;
    f32 dz;
    if (t->id != 0xFFFF) {
        do {
            if (!(pl->pos[1] < t->y - 50.0f) && pl->pos[1] < 50.0f + t->y) {
                dx = pl->pos[0] - t->x;
                dz = pl->pos[2] - t->z;
                if (flSqrt(dx * dx + dz * dz) <= t->r) {
                    return t->id;
                }
            }
            t++;
        } while (t->id != 0xFFFF);
    }
    return 0;
}
