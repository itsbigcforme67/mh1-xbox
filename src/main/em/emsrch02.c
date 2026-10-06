/* emsrch02 - monster joint matrix accessors (SLPM_654.95 0x0010A150-0x0010A208): get_joint_pos/wmat/mat and their *_em tail-call twins. Whole file in emsrch_nm.c. */
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
void get_joint_pos(EMW *em, s16 joint, f32 *out) {
    flmatGetTrans(out, em->mdl->bone + joint * 0x190);
}
void get_joint_pos_em(EMW *em, s16 joint, f32 *out) {
    get_joint_pos(em, joint, out);
}
u8 *get_joint_wmat(EMW *em, s16 joint) {
    return em->mdl->bone + joint * 0x190;
}
u8 *get_joint_wmat_em(EMW *em, s16 joint) {
    return get_joint_wmat(em, joint);
}
u8 *get_joint_mat(EMW *em, s16 joint) {
    return em->mdl->bone + joint * 0x190 + 0x40;
}
u8 *get_joint_mat_em(EMW *em, s16 joint) {
    return get_joint_mat(em, joint);
}
