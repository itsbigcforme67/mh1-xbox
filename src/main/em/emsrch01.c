/* emsrch01 - monster durability slot init (SLPM_654.95 0x0010A070-0x0010A0F0): em_dur_init. Whole file in emsrch_nm.c. */
/* emsrch_nm - SLPM_654.95 0x00109E30-0x0010A200 (g_em_search_set.s): monster target search over the four players
   (em_search_set), the per-monster "durability" (hit-point) slots (em_dur_init / em_dur_set) and joint matrix
   accessors (get_joint_pos / wmat / mat, plus the *_em duplicates that tail-call them). Working file. */
#include "types.h"
#include "em.h"
#include "pl.h"
extern PLW player_work[];
extern s16 *em_dur_tbl[];
/* per monster type: 9 hit-point values (s16), then the 0x953 value */
u16 calc_vec_ang(f32, f32, f32, f32);
f32 flAbs(f32);
f32 flSqrt(f32);
void flmatGetTrans(f32 *, u8 *);
/* Sets up the nine durability slots (EMW+0x304, 8 bytes each: on, no, 0, 0, hp s16, 0) from em_dur_tbl[type]. */
void em_dur_init(EMW *em) {
    int i;
    s16 *t;
    u8 *slot;

    i = 0;
    slot = (u8 *)em;
    t = em_dur_tbl[em->kind];
    do {
        if (*t > 0) {
            slot[0x304] = 1;
            slot[0x305] = i;
            slot[0x306] = 0;
            slot[0x307] = 0;
            *(s16 *)(slot + 0x308) = *t;
            slot[0x30A] = 0;
        } else {
            slot[0x304] = 0;
        }
        i++;
        t++;
        slot += 8;
    } while (i < 9);
    em->x953 = *t;
}
