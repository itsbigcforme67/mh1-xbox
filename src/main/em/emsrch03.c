/* emsrch03 - monster effect prim set-up and area lookup (SLPM_654.95 0x0010B6C0-0x0010B7E8): em_effect_pull, Em_max_parts_get, Em_area_ck. Whole file in emsrch_nm.c. */
/* emsrch_nm - SLPM_654.95 0x00109E30-0x0010A200 (g_em_search_set.s): monster target search over the four players
   (em_search_set), the per-monster "durability" (hit-point) slots (em_dur_init / em_dur_set) and joint matrix
   accessors (get_joint_pos / wmat / mat, plus the *_em duplicates that tail-call them). Working file. */
#include "types.h"
#include "em.h"
#include "pl.h"
#include "game.h"
extern PLW player_work[];
extern s16 *em_dur_tbl[];
/* per monster type: 9 hit-point values (s16), then the 0x953 value */
u16 calc_vec_ang(f32, f32, f32, f32);
f32 flAbs(f32);
f32 flSqrt(f32);
void flmatGetTrans(f32 *, u8 *);
/* ===== g_em_effect_pull.s 0x0010B6C0-0x0010B7E8 ===== */
#include "prim.h"
extern u8 em_parts_num[];
void enemy_trans();
typedef struct EMVT { void *_0; void (*init)(EMW *); } EMVT;
/* per monster function table: init at +4 */
typedef struct EMP {
    u8 on;                  /* 0x000 */
    u8 _pad001[0x3CB];
    EMVT *vt;               /* 0x3CC */
    u8 _pad3D0[0x564 - 0x3D0];
    PRIM *prim;             /* 0x564 */
    s16 prim_no;            /* 0x568 */
} EMP;
void em_effect_pull(void) {
    s16 i;
    EMP *e;

    for (i = 0, e = (EMP *)em_work; i < 20; i++, e = (EMP *)((u8 *)e + 0xA10)) {
        if (e->on != 0) {
            e->vt->init((EMW *)e);
            e->prim_no = get_prim();
            if (e->prim_no != -1) {
                e->prim = get_prim_ptr(e->prim_no);
                e->prim->owner = e;
                e->prim->trans = (void (*)(PRIM *))enemy_trans;
            }
        }
    }
}
u8 Em_max_parts_get(s16 kind) {
    return em_parts_num[kind];
}
s16 Em_area_ck(s16 stg) {
    s16 i;
    u8 *p;

    for (i = 0, p = (u8 *)&game_w; i < 4; i++, p++) {
        if (p[0x28] == stg) {
            return i;
        }
    }
    return -1;
}
