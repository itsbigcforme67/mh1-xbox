/* emsrch_nm - SLPM_654.95 0x00109E30-0x0010A200 (g_em_search_set.s): monster target search over the four players
   (em_search_set), the per-monster "durability" (hit-point) slots (em_dur_init / em_dur_set) and joint matrix
   accessors (get_joint_pos / wmat / mat, plus the *_em duplicates that tail-call them). Working file. */
#include "types.h"
#include "em.h"
#include "pl.h"
#include "game.h"

extern PLW player_work[];
extern s16 *em_dur_tbl[];       /* per monster type: 9 hit-point values (s16), then the 0x953 value */

u16 calc_vec_ang(f32, f32, f32, f32);
f32 flAbs(f32);
f32 flSqrt(f32);
void flmatGetTrans(f32 *, u8 *);

/* Picks the nearest player on the monster's stage within dist_xz (0 = any) horizontally, dist_y (0 = any)
   vertically and inside a cone of +-ang around the monster's facing; stores it in x3B0 / x617 / x3AC (distance)
   and horm_ang (angle to it); returns the player number or -1. */
s8 em_search_set(EMW *em, f32 dist_xz, f32 dist_y, u16 ang) {
    int i;
    int best;
    PLW *pl;
    f32 best_d;
    f32 px, py, pz;
    f32 dy, dx, dz, dxz, d;

    i = 0;
    best = -1;
    pl = player_work;
    em->x3B0 = 0;
    best_d = 0.0f;
    em->x617 = -1;
    em->x3AC = 0;
    px = em->pos[0];
    py = em->pos[1];
    pz = em->pos[2];
    do {
        if (em->stg == pl->stg) {
            dy = pl->pos[1] - py;
            if (dist_y == 0.0f || flAbs(dy) <= dist_y) {
                dx = pl->pos[0] - px;
                dz = pl->pos[2] - pz;
                dxz = flSqrt(dx * dx + dz * dz);
                if (dist_xz == 0.0f || dxz <= dist_xz) {
                    d = flSqrt(dxz * dxz + dy * dy);
                    if (ang * 2 >= (((((calc_vec_ang(px, pz, pl->pos[0], pl->pos[2]) & 0xFFFF) - 0x4000) & 0xFFFF) + ang) - em->ang[1] & 0xFFFF)) {
                        if (0.0f == best_d || d <= best_d) {
                            best_d = d;
                            best = i;
                        }
                    }
                }
            }
        }
        i++;
        pl++;
    } while (i < 4);
    if (best != -1) {
        em->x3B0 = &player_work[best];
        em->x617 = best;
        *(f32 *)&em->x3AC = best_d;
        em->horm_ang = ((calc_vec_ang(px, pz, em->x3B0->pos[0], em->x3B0->pos[2]) & 0xFFFF) - 0x4000) & 0xFFFF;
    }
    return best;
}

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

void em_dur_set(EMW *em, u32 n) {
    u8 *slot;

    if (n < 9) {
        slot = (u8 *)(n * 8) + (int)em;
        if (slot[0x304] == 0) {
            return;
        }
        *(s16 *)(slot + 0x308) = em_dur_tbl[em->kind][n];
    }
}

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

/* ===== g_em_effect_pull.s 0x0010B6C0-0x0010B7E8 ===== */
#include "prim.h"

extern u8 em_parts_num[];

void enemy_trans();

typedef struct EMVT { void *_0; void (*init)(EMW *); } EMVT;   /* per monster function table: init at +4 */

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
