/*
 * rt_em.c - the monster side of the port runtime: what the monster loop
 * (enemy_mv, src/main/em/f_em_nm.c) and game.bin's monster code call that is
 * not decompiled yet. Weak stand-ins are replaced by the real C when it is
 * linked in (per-monster AI files, the command interpreter em_cmd).
 */
#include "rt.h"
#include "types.h"
#include "em.h"
#include "game.h"
#include "quest.h"
#include "fl.h"
#include "clay.h"
#include "eft.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WEAK __attribute__((weak))
static void once(const char *n) { rt_log_standin(n); if (getenv("RT_TRACE")) fprintf(stderr, "rt_em: %s not ported (stand-in)\n", n); }
#define WSTUB(name) WEAK void name(EMW *em) { static int o; (void)em; if (!o++) once(#name); }

#define PU8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define PS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PU16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define PS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PF(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define PP(p, o) (*(void **)((u8 *)(p) + (o)))

extern s16 *em_dur_tbl[];
int ran_suu(int);
int act_ck(void *, int, int);
void rt_motion_attach(void *w);
void *Stage_data_get(int);

/* ------------------------------------------------ monster AI not linked yet
 * Each emNN file (game.bin, agent B and others) defines these; until it is
 * built for the PC the monster stands still (its init/main do nothing). */
#define EM_SET(nn) \
    WSTUB(em##nn##_init) WSTUB(em##nn##_main) \
    WEAK void em##nn##_act_set(EMW *em, int a, int b, int c) { (void)em; (void)a; (void)b; (void)c; } \
    WSTUB(em##nn##_local_area_move_init)
EM_SET(01) EM_SET(02) EM_SET(03) EM_SET(04) EM_SET(07) EM_SET(08) EM_SET(09) EM_SET(10)
EM_SET(12) EM_SET(14) EM_SET(15) EM_SET(16) EM_SET(17) EM_SET(18) EM_SET(19) EM_SET(20)
EM_SET(21) EM_SET(27) EM_SET(29) EM_SET(33)

/* ------------------------------------------------ main f_em / f_pl helpers */
/* em_dur_init (main 0x10A070): the breakable parts' durability from
 * em_dur_tbl[kind] (9 s16 values, then the part count byte at +0x953):
 * per part 8 bytes at +0x304 {on, index, 0, 0, s16 durability, 0}. */
void em_dur_init(EMW *em)
{
    s16 *t = em_dur_tbl[em->kind];
    u8 *e = (u8 *)em;
    int i;
    for (i = 0; i < 9; i++, t++, e += 8) {
        if (*t > 0) {
            e[0x304] = 1;
            e[0x305] = (u8)i;
            e[0x306] = 0;
            e[0x307] = 0;
            *(s16 *)(e + 0x308) = *t;
            e[0x30A] = 0;
        } else {
            e[0x304] = 0;
        }
    }
    PU8(em, 0x953) = (u8)*t;
}



/* hit_line2_pk (0x28CC40): packs a line p0..p1: +0 p0, +0xC p1, +0x18
 * p1 - p0, +0x24 centre, +0x30 half length. */
f32 flvecCalcDistance(f32 *, f32 *);
void flvecCopy(f32 *, f32 *);
void hit_line2_pk(f32 *p0, f32 *p1, f32 *pk)
{
    flvecCopy(pk, p0);
    flvecCopy(pk + 3, p1);
    pk[6] = p1[0] - p0[0];
    pk[7] = p1[1] - p0[1];
    pk[8] = p1[2] - p0[2];
    pk[9] = 0.5f * (p0[0] + p1[0]);
    pk[10] = 0.5f * (p0[1] + p1[1]);
    pk[11] = 0.5f * (p0[2] + p1[2]);
    pk[12] = flvecCalcDistance(pk, pk + 9);
}

/* act_set (0x14EB00): start action (mode a, sub-mode b) of a player or
 * monster. Monsters (+0x10 != 0) skip the player-only parts (old action,
 * random +0x39A, timers, network). */
void action_timer_calc(void *, int);
int Pl_master_ck(void *);
void pl_to_normal(void *, int, int, int);
WEAK void act_set(void *w, int a, int b)
{
    u8 *p = w;
    if (p[0x10] == 0) {
        p[0x16] = p[0x14];
        p[0x17] = p[0x15];
    }
    p[0x14] = (u8)a;
    p[0x15] = (u8)b;
    p[5] = p[6] = p[7] = 0;
    if (p[0x10] == 0)
        PS16(p, 0x39A) = (s16)ran_suu(1);
    if (p[0x10] != 0 || !((s16)act_ck(p, 1, 8) || (s16)act_ck(p, 1, 2) || (s16)act_ck(p, 1, 10))) {
        int i;
        for (i = 0; i < 4; i++) {
            p[0x720 + i] = 0;
            PS16(p, 0x724 + 2 * i) = 0;
            PS16(p, 0x72C + 2 * i) = 0;
        }
    }
    if (p[0x10] == 0 && p[0x14] == 2) {
        p[0x615] = 0;
        action_timer_calc(p, 0);
    }
    PS32(p, 0x390) &= 0x100100;
    PS32(p, 0x394) = 0;
    if (p[0x10] == 0 && p[0x1E] == 0 && Pl_master_ck(p) != 1 && (s16)act_ck(p, 0, 0))
        pl_to_normal(p, 0, 4, 0);
}

/* GetEmMaterialData (0x16AB30): only kinds 9 and 23 take the ground's
 * diffuse colour (GetPlayerDiffuseData); the host lights monsters itself. */
void GetEmMaterialData(EMW *em) { (void)em; }

/* ------------------------------------------------ per-material state (enemy_trans)
 * enemy_trans (main 0x168B10) draws clay i of a monster only while EMW+0x4E6+i
 * is set (em_init sets all; em29 keeps one per type); kind 3 draws only clay
 * EMW+0x11. Before each clay it calls a per-kind material function on the
 * clay's materials (CLAY+8 = the AMO part's 0x50000 list, index m below):
 * em09_material_sub (game 0x5ACA60) for kinds 9/18/23, em20_material_sub
 * (game 0x5FCBB0, compiled game C) for 20, em_material_sub (main 0x10CEA0,
 * asm only, ported here) for the rest. They write the material's alpha
 * (flMATERIAL +0x10, the diffuse colour's alpha) = EMW+0x798 (1.0; the AI
 * counts it down to fade a carved corpse out), then 0 for the materials not
 * shown; a few also set the diffuse colour (+0x04..0x0C) or the texture
 * (+0x44). Field meanings from the game C where known: hagi[k].cnt (EMW
 * 0x30A + 8k) = times part k was broken; EMW+0x948 bit 0 = the tail is cut
 * (em_tail_off_sub and the cut-tail damage actions set it): the cut-surface
 * caps of the body and of the cut tail show then; the rest is named by offset. */
#define HAGI(k) PU8(em, 0x30A + 8 * (k))
#define TAIL_CUT (PU8(em, 0x948) & 1)
#define OFF(mm) (o[mm].alpha = 0.0f)

int em_frame_check2(void *w, int n, f32 f);
void em20_material_sub(EMW *em, int type, u8 *tbl);

/* em09_material_sub (game 0x5ACA60, asm only): clays 0 and 1 show one of
 * the eye/mouth states m 1-4 picked by EX+0x4A */
static void em09_mat(u8 *em, int clay, int n, rt_em_mat *o)
{
    int m, s = PU8(em, 0x444 + 0x4A);
    if (clay != 0 && clay != 1)
        return;
    for (m = 0; m < n; m++) {
        int a = s == 1 ? (m == 4 || m == 3 || m == 1) : s == 2 ? (m == 4 || m == 2 || m == 1)
              : s == 3 ? (m == 3 || m == 2 || m == 1) : (m == 4 || m == 3 || m == 2);
        if (a)
            OFF(m);
    }
}

/* em20_material_sub (Gypceros) is game C: run it on a stand-in material
 * table (flMATERIAL 0x4C bytes) and clay list, then read the alphas back */
static void em20_mat(u8 *em, int clay, int n, rt_em_mat *o)
{
    static u8 mat[32][0x4C];
    static CLAY cl[32];
    struct { u8 pad[0x10]; u8 *mat; } mdl;
    void *keep = PP(em, 0x50C);
    int m;
    if (clay >= 32)
        return;
    memset(&mdl, 0, sizeof mdl);
    mdl.mat = &mat[0][0];
    cl[clay].mat_num = n;
    for (m = 0; m < n; m++)
        cl[clay].mat_no[m] = m;
    PP(em, 0x50C) = &mdl;
    em20_material_sub((EMW *)em, clay, (u8 *)cl);
    PP(em, 0x50C) = keep;
    for (m = 0; m < n; m++)
        memcpy(&o[m].alpha, &mat[m][0x10], 4);
}

/* em_material_sub (main 0x10CEA0): the cases per kind */
static void em_mat(u8 *em, int clay, int n, rt_em_mat *o)
{
    int kind = PU8(em, 2), m, sel = 0, flag = 0;
    s16 hp;
    float t;
    switch (kind) {
    case 13: case 16: case 27: case 28: case 30: case 31:  /* raptors: blink, see below */
        sel = PU8(em, 0x444 + (kind == 31 || kind == 28 || kind == 27 ? 0x50 : 0x60)) ? 0 : (s16)(PU16(&game_w, 0x1E) % 98);
        break;
    case 14: case 26:               /* Diablos / Monoblos: frame 10 of motion 0x459 */
        flag = PU16(em, 0x2DC) == 0x459 && !em_frame_check2(em, 0, 10.0f);
        break;
    case 19: case 24:
        sel = (s16)(PU16(&game_w, 0x1E) % 9);
        break;
    }
    for (m = 0; m < n; m++) {
        switch (kind) {
        case 1:                     /* Rathian: broken head / back variants, tail-cut caps */
            if (clay == 0) {
                if ((m == 0 && HAGI(1) > 0) || (m == 1 && HAGI(2) > 0) || m == 5 || (m == 6 && !TAIL_CUT))
                    OFF(m);
            } else if (clay == 1) {
                if (m == 1 && !TAIL_CUT)
                    OFF(m);
            } else if (clay == 2) {
                if (m == 3)
                    OFF(m);
            } else if (clay == 3) {
                if (((m == 0 || m == 1) && HAGI(6) < 2) || (m == 2 && HAGI(2) == 0) || (m == 3 && HAGI(1) == 0))
                    OFF(m);
            } else if (clay == 4) {
                if (m == 0 && HAGI(6) >= 2)
                    OFF(m);
            }
            break;
        case 2:                     /* Fatalis: damage by the hit points at EX+0x52 */
            hp = PS16(em, 0x444 + 0x52);
            if (clay == 1) {
                if (((m == 0 || m == 1) && hp < 0x6401) || (m == 2 && hp < 0x4B01) || (m == 3 && hp < 0x3201))
                    OFF(m);
            } else if (clay == 3) {
                if (m == 1 || m == 5)
                    o[m].tex = hp < 0x1901 ? 2 : 1;     /* mem_tex[0x9A + EMW+0x34F + k]: APX k of the model's textures */
                else if ((m == 3 || m == 8) && hp < 0x1901)
                    OFF(m);
            } else if (clay == 4) {
                if ((m == 1 && hp >= 0x6401) || (m == 2 && (hp >= 0x4B01 || hp < 0x3201)) || (m == 3 && hp >= 0x3201)
                    || (m == 0 && hp >= 0x1901))
                    OFF(m);
            }
            break;
        case 6:                     /* Yian Kut-Ku */
            if (clay == 0) {
                if ((m == 5 && !TAIL_CUT) || m == 6)
                    OFF(m);
            } else if (clay == 1) {
                if (m == 1 || ((m == 2 || m == 3) && !TAIL_CUT))
                    OFF(m);
            } else if (clay == 2) {
                if (m == 0)
                    OFF(m);
            }
            break;
        case 7:                     /* Lao-Shan Lung: broken parts */
            if (clay == 2) {
                if ((m == 0 && HAGI(6) >= 2) || (m == 1 && HAGI(5) >= 2) || (m == 2 && HAGI(0) >= 2) || (m == 3 && HAGI(0) > 0))
                    OFF(m);
            } else if (clay == 4) {
                if (m == 0 && HAGI(4) >= 3)
                    OFF(m);
            } else if (clay == 5) {
                if ((m == 0 && HAGI(6) < 2) || (m == 1 && HAGI(5) < 2) || (m == 2 && HAGI(0) < 2) || (m == 3 && HAGI(0) <= 0)
                    || (m == 4 && HAGI(4) < 3))
                    OFF(m);
            }
            break;
        case 8: case 34:            /* Cephadrome / Cephalos (one model); the drome is darker */
            if (kind == 8) {
                o[m].has_col = 1;
                o[m].col[0] = 0.39607844f;      /* 0x3ECACACB */
                o[m].col[1] = 0.37647063f;      /* 0x3EC0C0C1 */
                o[m].col[2] = 0.25490198f;      /* 0x3E828283 */
            }
            if ((clay == 0 && (m == 2 || (m == 3 && !TAIL_CUT))) || (clay == 1 && m == 2 && !TAIL_CUT) || (clay == 2 && m == 2))
                OFF(m);
            break;
        case 11:                    /* Rathalos */
            if (clay == 0) {
                if ((m == 3 && HAGI(1) > 0) || (m == 4 && HAGI(2) > 0))
                    OFF(m);
            } else if (clay == 1) {
                if (m == 1 && !TAIL_CUT)
                    OFF(m);
            } else if (clay == 2) {
                if (((m == 4 || m == 0) && HAGI(6) < 2) || m == 1 || (m == 5 && HAGI(1) == 0) || (m == 6 && HAGI(2) == 0))
                    OFF(m);
            } else if (clay == 3) {
                if (m == 4 || (m == 5 && !TAIL_CUT))
                    OFF(m);
            } else if (clay == 4) {
                if (m == 0 && HAGI(6) >= 2)
                    OFF(m);
            }
            break;
        case 13: case 16: case 27: case 28: case 30: case 31:
            if (m == (kind == 30 || kind == 16 || kind == 13 ? 5 : 4))   /* the other kind's crest / claws */
                OFF(m);
            if ((u32)sel < 4 ? (m == 3 || m == 1 || m == 0) : sel < 6 ? (m == 3 || m == 2 || m == 0)
                : sel < 8 ? (m == 2 || m == 1 || m == 0) : (m == 3 || m == 2 || m == 1))      /* eyes */
                OFF(m);
            if (m == 9 && PU16(em, 0x2DC) != 0x410 && PU16(em, 0x2DC) != 0x415)
                OFF(m);
            break;
        case 14: case 26: {         /* Diablos / Monoblos: horns by EX+0x1A (broken count), tail-cut caps */
            int h = PU8(em, 0x444 + 0x1A);
            if (clay == 0) {
                if (kind == 14 && m == 3 && h >= 2 && (!flag || h != 2))
                    OFF(m);
                if (m == 4 && h > 0 && (!flag || h != 1))
                    OFF(m);
                if (m == (kind == 14 ? 6 : 7))
                    OFF(m);
                if (kind == 26 && m == 0) {     /* Monoblos: reddens with EX+0x1B (s8) / 60 */
                    t = (f32)PS8(em, 0x45F) / 60.0f;
                    o[m].has_col = 1;
                    o[m].col[0] = 1.0f - 0.17254902f * t;   /* 0x3E30B0B0 */
                    o[m].col[1] = o[m].col[2] = 1.0f - 0.8039216f * t;  /* 0x3F4DCDCE */
                }
            } else if (clay == 1) {
                if (m == 2 && !TAIL_CUT)
                    OFF(m);
            } else if (clay == 2) {
                int hh = PU8(em, 0x444 + 0x1A);
                if (m == 4 || (m == 5 && !TAIL_CUT)
                    || (m == (kind == 14 ? 7 : 6) && (hh == 0 || (flag && hh == 1)))
                    || (kind == 14 && m == 6 && (hh < 2 || (flag && hh == 2))))
                    OFF(m);
            }
            break;
        }
        case 15:                    /* Khezu */
            if ((clay == 1 && m == 1 && !TAIL_CUT) || (clay == 3 && m == 1) || (clay == 4 && (m == 3 || (m == 4 && !TAIL_CUT))))
                OFF(m);
            break;
        case 17:                    /* Gravios */
            if ((clay == 0 && m == 3) || (clay == 1 && m == 2 && !TAIL_CUT)
                || (clay == 2 && ((m == 4 && HAGI(6) >= 2) || (m == 5 && HAGI(6) > 0) || m == 6 || (m == 7 && !TAIL_CUT)))
                || (clay == 3 && ((m == 0 && HAGI(6) < 2) || (m == 1 && HAGI(6) == 0))))
                OFF(m);
            break;
        case 19: case 24:           /* eye blink every 9 ticks, mouth by motion */
            if (m >= 2 && m <= 4) {
                if (sel == 0 || sel == 1 ? (m == 4 || m == 3) : sel >= 3 && sel <= 6 ? (m == 3 || m == 2) : (m == 4 || m == 2))
                    OFF(m);
            } else if (m == 1 || (m >= 5 && m <= 7)) {
                int mo = PU16(em, 0x2DC);
                if (mo == 0x3E9 || mo == 0x3EA || mo == 0x3EB || mo == 0x3EE || mo == 0x3EF || mo == 0x3F2 || mo == 0x3F3 || mo == 0x3F4) {
                    if (mo == 0x3F4 && !em_frame_check2(em, 0, 72.0f)) {
                        if (m == 7 || m == 6)
                            OFF(m);
                    } else if (PU16(&game_w, 0x1E) & 1) {
                        if (m == 7)
                            OFF(m);
                    } else if (m == 6 || m == 5 || m == 1) {
                        OFF(m);
                    }
                } else if (m == 7 || m == 6) {
                    OFF(m);
                }
            }
            break;
        case 21:                    /* Plesioth */
            if ((clay == 0 && (m == 2 || (m == 3 && !TAIL_CUT))) || (clay == 1 && m == 2 && !TAIL_CUT) || (clay == 2 && m == 2))
                OFF(m);
            break;
        case 22:                    /* Basarios */
            if ((clay == 0 && m == 2) || (clay == 1 && m == 1 && !TAIL_CUT)
                || (clay == 2 && ((m == 4 && HAGI(6) > 0) || m == 5 || (m == 6 && !TAIL_CUT)))
                || (clay == 3 && ((m == 0 && HAGI(6) < 2) || (m == 1 && HAGI(6) == 0))))
                OFF(m);
            break;
        }
    }
}

/* eft09 (game C, src/game/eft/eft09.c): tail_off (called by em_tail_off_sub when the cut-tail damage
 * action starts) sets the effect's arg = 1, pos = the world position of node 43 (the tail) and
 * u0A.ang = its yaw (calc_mat_angY), and puts a carving point there (Em_tail_hagi_point_set, moved
 * to pos every tick until carved out). eft09_t then draws clay 1 alone, its tail bones posed from
 * their bind pose under the root Scale(EMW+0xB8) * RotY(ang + 0x4000) * Trans(pos), while the
 * monster is active (x01) and the effect lives on this stage. 1 = draw it; root yaw in radians. */
int rt_em_cut_tail(const void *emp, float pos[3], float *yaw)
{
    const u8 *em = (const u8 *)emp;
    const EFTW *t = (const EFTW *)PP(em, 0x878);
    if (!t || t->owner != (EMW *)em || t->type != 9 || t->mode != 1 || t->arg == 0 || !PU8(em, 1)
        || t->stg != game_w.stage)
        return 0;
    memcpy(pos, t->pos, 3 * sizeof(float));
    *yaw = (float)((t->u0A.ang + 0x4000) & 0xFFFF) * (6.2831853f / 65536.0f);
    if (getenv("RT_EM_MAT_TRACE")) {
        static const void *said[20];
        int k;
        for (k = 0; k < 20 && said[k] && said[k] != em; k++)
            ;
        if (k < 20 && !said[k]) {
            said[k] = em;
            fprintf(stderr, "em-tail: kind %d tail cut at %.0f %.0f %.0f yaw %04X pick %d\n", PU8(em, 2), pos[0], pos[1], pos[2],
                    t->u0A.ang & 0xFFFF, (s8)t->x07);
        }
    }
    return 1;
}

int rt_em_materials(const void *emp, int clay, int n, rt_em_mat *o)
{
    u8 *em = (u8 *)emp;
    int kind = PU8(em, 2), m;
    if (n > 32)
        n = 32;
    for (m = 0; m < n; m++) {
        o[m].alpha = PF(em, 0x798);
        o[m].has_col = 0;
        o[m].tex = -1;
    }
    if (kind == 3 && clay != PU8(em, 0x11))
        return 0;
    /* clay 1 of kinds 1/6/8/11/14/15/17/21/22/26 is the tail: em20_init clears its flag and eft09 draws
     * it (eft09_t): with the body's tail bones until it is cut off (EFTW arg 0), then lying where it
     * was cut (rt_em_cut_tail); the host draws the uncut tail with the body */
    if (clay < 0x20 && !PU8(em, 0x4E6 + (kind == 3 ? 0 : clay))) {
        const EFTW *t = (const EFTW *)PP(em, 0x878);
        if (!(clay == 1 && t && t->owner == (EMW *)em && t->type == 9 && t->arg == 0))
            return 0;
    }
    if (kind == 9 || kind == 18 || kind == 23)
        em09_mat(em, clay, n, o);
    else if (kind == 20)
        em20_mat(em, clay, n, o);
    else
        em_mat(em, clay, n, o);
    return 1;
}
#undef HAGI
#undef TAIL_CUT
#undef OFF

/* enemy_trans: the monster's draw prim; the host draws the monster model
 * itself (viewer.c), so the prim does nothing here. */
void enemy_trans(void *prim) { (void)prim; }

/* em_work_set (0x16A010): takes a model work for the monster and copies the
 * model data game_w+0x88[mdl_no]; the host has its own model and only needs
 * the motion model work (frame.h FRMDL) for frame_init/frame_move. */
void em_work_set(EMW *em)
{
    if (!em->mdl)
        rt_motion_attach(em);
}

/* push_em_work (0x169EB0): frees the draw prim and model work, clears the
 * first 0x12 bytes (be_flag ... ) */
void release_prim(s16);
void push_em_work(EMW *em)
{
    if (PS32(em, 0x564))
        release_prim(PS16(em, 0x568));
    memset(em, 0, 0x12);
}

/* pull_enemy_work (0x16A1A0): first free em_work slot, cleared and set up
 * (slot number at +0x13 and +0xC, +0x88D = -1, +0x798 = 1.0). */
void *rt_pull_enemy_work(void)
{
    int i;
    for (i = 0; i < 20; i++) {
        EMW *em = &em_work[i];
        if (em->be_flag == 0) {
            void *mdl = em->mdl;            /* host: keep the motion model work */
            memset(em, 0, sizeof *em);
            em->x13 = (u8)i;
            em->be_flag = 1;
            em->x10 = 1;
            PS8(em, 0x88D) = -1;
            em->id = (u16)i;
            em->mdl = mdl;
            PU8(em, 0x8C3) = PU8(&game_w, 0xD1);
            PF(em, 0x798) = 1.0f;
            return em;
        }
    }
    return NULL;
}

/* ------------------------------------------------ quest (main f_quest, not built) */
WEAK void Quest_enemy_die(EMW *em) { fprintf(stderr, "rt_em: monster %d (kind %d) down\n", em->id, em->kind); }
void Ext_pick_point_clr(int);
/* Em_hagi_point_clr (0x229A50) */
WEAK void Em_hagi_point_clr(EMW *em)
{
    Ext_pick_point_clr(PS8(em, 0x88D));
    PS8(em, 0x88D) = -1;
}

/* ------------------------------------------------ network (offline: nothing) */
WEAK void net_receive_em_act(EMW *em) { (void)em; }
WEAK void net_send_em(EMW *em, int a, int b) { (void)em; (void)a; (void)b; }

/* ------------------------------------------------ em_cmd (game.bin 0x55B060.., agent D) */
WSTUB(em_cmd_init)
WEAK void NextStage_No_Set(EMW *em) { (void)em; }

/* Em_Mode_Chg (0x566500): switch the monster's mode +0x888 (0 calm, 1
 * attack mode with timer +0x886); back to calm clears the hate/target
 * work and the players' +0x7EE marks it set. Returns 0 if unchanged. */
WEAK int Em_Mode_Chg(EMW *em, int mode, int timer)
{
    int i;
    u8 *pl;
    if (PU8(em, 0x888) == (u8)mode)
        return 0;
    if ((s16)timer > 0)
        PS16(em, 0x886) = (s16)timer;
    PU8(em, 0x888) = (u8)mode;
    PU8(em, 0x83B) &= 0xC0;
    if (PU8(em, 0x888) == 0) {
        PU8(em, 0x88C) = 0;
        PU8(em, 0x88F) = 0;
        for (i = 0, pl = (u8 *)player_work; i < 4; i++, pl += 0xA00) {
            PS32(em, 0x8F4 + 4 * i) = 0;
            PS32(em, 0x918 + 4 * i) = 0;
            PS16(em, 0x890 + 2 * i) = 0;
            if ((PU8(em, 0x7EE) & (1 << i)) && pl[0] && pl[0x7EE])
                pl[0x7EE] = 0;
        }
        PU8(em, 0x7EE) = 0;
    }
    return 1;
}

/* em_cdm_act_flag_ck (0x565DC0): +0x82B 1: target the first player whose
 * bit in +0x914 is set (+0x880/+0x881 = 1, +0x883 = index); none
 * flagged: +0x880 = 0. */
WEAK void em_cdm_act_flag_ck(EMW *em)
{
    int n = PU8(&game_w, 0xD3), i, c = 0;
    if (PU8(em, 0x82B) != 1) {
        if (PU8(em, 0x82B) == 0)
            PU8(em, 0x880) = 0;
        return;
    }
    for (i = 0; i < n; i++)
        if (PU8(em, 0x914) & (1 << i))
            c++;
    if (c == 0) {
        PU8(em, 0x880) = 0;
        return;
    }
    PU8(em, 0x880) = 1;
    PU8(em, 0x881) = 1;
    PU8(em, 0x882) = 0;
    for (i = 0; i < n; i++)
        if (PU8(em, 0x914) & (1 << i))
            break;
    PU8(em, 0x883) = (u8)i;
}

/* ------------------------------------------------ callees of em01 AI / em_cmd / Em_Dmg_Sys */
/* em_dur_set (main 0x10A0F0): refill part n's durability (+0x308 of the
 * 8-byte part slot) from em_dur_tbl[kind][n], if the part is on. */
void em_dur_set(EMW *em, int n)
{
    u8 *e = (u8 *)em + 8 * n;
    if ((unsigned)n >= 9 || e[0x304] == 0)
        return;
    *(s16 *)(e + 0x308) = em_dur_tbl[em->kind][n];
}

/* GetWaterData (main 0x16AB80): byte +0xC of the ground entry the work
 * stands on (ground_tbl_add[stage +0x736][+0x70C]): non-zero = water.
 * em_cmd_water_ck calls it with no arguments (a0 = em left over); its
 * build passes em (build_pc.sh). */
extern u8 *ground_tbl_add[];
int GetWaterData(void *w)
{
    u8 *p = w, *t = ground_tbl_add[p[0x736]];
    return t ? t[p[0x70C] * 16 + 0xC] : 0;
}

/* Quest side (main f_quest; f_quest_nm.c is not on the PC yet): carving
 * points are not set up (no carving yet), so they report "none". */
WEAK s8 Em_hagi_point_set(EMW *em, int n) { (void)n; PS8(em, 0x88D) = -1; return -1; }
WEAK int Em_hagi_point_cnt_ck(EMW *em) { (void)em; return -1; }
WEAK void Quest_enemy_capture(EMW *em) { fprintf(stderr, "rt_em: monster %d (kind %d) captured\n", em->id, em->kind); }
/* Quest_enemy_hagi_set (main 0x2276C0): quest_w+0x13C |= b */
WEAK void Quest_enemy_hagi_set(int a, int b) { (void)a; quest_w.x13C |= b; }
/* WyvernAreaMove (menu16.c): the map's monster-moved marker; no map yet. */
WEAK void WyvernAreaMove(void *em) { (void)em; }
/* wyvern_kill_cnt_up (ud_nm.c): online-only kill counter. */
WEAK void wyvern_kill_cnt_up(void *u, int n) { (void)u; (void)n; }

/* ------------------------------------------------ quest monster set-up
 * The parts of the quest start (main f_quest, Quest_init 0x2263xx /
 * Em_direct_set) the PC needs to put a quest's monster on the stage:
 * the mission file questName[no] (AFS_DATA, Meltw) is read into a host
 * mission_area and quest_w's table pointers set from its header as
 * Quest_init does (x64 header, x74 per-stage monster lists, x78 the
 * quest's own (big) monsters, x80 stage data, x94 info, x14E). */
void rt_quest_mem_init(void);
void Quest_init(void);
void Quest_start(void);
void Start_item_init(void);
void Quest_timer_reset(void);
void Quest_em_init_set(int stage);
s32 *Em_data_com_adrs_get(s32 *p, int which);

/* --quest N: the game's own quest start. Quest_init (free-hunt tables),
 * then Quest_start as game11 runs it (select_w+0xAC = quest number): it
 * loads the mission file questName[no] into mission_area, points quest_w
 * at its tables, sets the stage (Quest_pl_stage_init), the time limit and
 * the monster states (quest_em_init). src/main/quest/f_quest*_nm.c. */
s32 *Em_data_st_adrs_get(s32 *p, int id, int which, s8 idx);
int rt_quest_load(int no)
{
    if (no <= 0 || no >= 0xB2) {
        rt_warn("quest %d: not a valid quest number", no);
        return -1;
    }
    rt_log("quest %d: loading the mission file (--quest or host start)", no);
    rt_quest_mem_init();
    Quest_init();
    game_w.master = 0;
    game_w.pl_num = 1;
    game_w.pl_state[0] = 1;
    *((u8 *)&select_w + 0xAC) = (u8)no;
    *((u8 *)&select_w + 0xAD) = 0;
    Quest_start();
    Start_item_init();      /* game11: the quest's supply box (game_w+0x128 list, dsp03) */
    {   /* game11 next: the quest's event demos (first sight of a monster:
         * Kut-Ku 148, Cephadrome 154, Monoblos 171 ...; evdemo.c). The
         * monster waits (game_w+0x21F) until its demo has run. */
        void EvDemoInitialize(void);
        extern u8 event_demo[];
        if (PU16(&game_w, 0x2C) == 0)
            PU16(&game_w, 0x2C) = (u16)no;
        EvDemoInitialize();
        if (getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "rt_quest: quest %d event demo slot: %d\n", PU16(&game_w, 0x2C), event_demo[4]);
    }
    if (getenv("RT_QEM_DUMP")) {    /* test aid: every stage's monster list of this quest */
        int st;
        int var;
        for (var = 0; var < 4 && (var == 0 || quest_w.x74[var] != 0); var++)       /* wave variants: program op 32 (0x20) sets quest_w.x3A, the list used from then on */
        for (st = 1; st < 0x58; st++) {
            QEM *l = (QEM *)Em_data_st_adrs_get(quest_w.x74, st, 1, (s8)var);
            if (l == NULL || l == (QEM *)-1)
                continue;
            fprintf(stderr, "rt_quest: quest %d stage %d kinds:", no, st + 100 * var);
            for (; l->id >= 0; l++)
                fprintf(stderr, " %d", l->id);
            fprintf(stderr, "\n");
            fprintf(stderr, "rt_quest: quest %d stage %d counts:", no, st + 100 * var);      /* kind:x04 (kills it can give, respawns): x05 (counts for x34) */
            for (l = (QEM *)Em_data_st_adrs_get(quest_w.x74, st, 1, (s8)var); l->id >= 0; l++)
                fprintf(stderr, " %d:%d:%d", l->id, l->x04, l->x05);
            fprintf(stderr, "\n");
        }
    }
    if (getenv("RT_QUEST_TRACE")) {
        int i;
        fprintf(stderr, "rt_quest: quest %d supply box:", no);
        for (i = 0; i < 32 && PU16(&game_w, 0x128 + 4 * i); i++)
            fprintf(stderr, " %d:%d", PU16(&game_w, 0x128 + 4 * i), PS16(&game_w, 0x12A + 4 * i));
        fprintf(stderr, "\n");
        if (quest_w.x6C) {      /* the condition program (QCMD: cmd a b c), up to its end (-1 / -2) */
            QCMD *q = (QCMD *)quest_w.x6C;
            fprintf(stderr, "rt_quest: quest %d type %d reward %d fee %d program:", no, quest_w.x00, quest_w.x14, quest_w.x18);
            for (i = 0; i < 48; i++, q++) {
                fprintf(stderr, " %d/%d/%d/%d", q->cmd, q->a, q->b, q->c);
                if (q->cmd == 0x1F)
                    break;
            }
            fprintf(stderr, "\n");
        }
    }
    /* game13's start of the hunt: mode 2 (game2), timers */
    game_w.mode = 2;
    game_w.step = 0;
    Quest_timer_reset();
    return quest_w.no == no && quest_w.x94 ? 0 : -1;
}

/* no quest (free play in the viewer): Quest_init's free-hunt tables, so
 * the HUD and quest helpers have their data */
void rt_quest_free_hunt(void)
{
    rt_quest_mem_init();
    Quest_init();
}

/* the quest's own monsters (QEM list 1 of quest_w.x78), NULL for none */
static void *quest_com_list(int which)
{
    s32 *p;
    if (quest_w.no == 0)
        return NULL;
    p = Em_data_com_adrs_get(quest_w.x78, which);
    return p == NULL || p == (s32 *)-1 ? NULL : p;
}

/* The quest's first own monster (QEM, 0x3C bytes): its stage (QEM+7)
 * is where the hunt takes place; -1 without one. */
int rt_quest_monster_stage(int *kind)
{
    QEM *q = quest_com_list(1);
    if (!q || q->id < 0)
        return -1;
    if (kind)
        *kind = q->id;
    return (u8)q->x07;
}

/* Em_direct_set (main 0x2273xx, f_quest_nm.c) for one QEM entry: a free
 * em_work, the entry's kind, variant, stage, hunger/thirst/sleep,
 * position and angle, then enemy_mv's first step (em_init: em01_init sets
 * the hit points). Model slot 0 (game_w+0x28[0]) is the host's em01 model. */
int enemy_mv(EMW *em);
EMW *rt_monster_spawn_qem(const QEM *q)
{
    EMW *em = rt_pull_enemy_work();
    if (!em)
        return NULL;
    game_w.x28[0] = (u8)q->id;
    em->mdl_no = 0;
    em->kind = (u8)q->id;
    em->type = (u8)q->x02;
    PU8(em, 0x9EB) = (u8)q->x2C;
    em->stg = game_w.stage;
    em->hungry = q->x0C;
    em->thirst = q->x10;
    em->x8A0 = q->x14;
    PU8(em, 0x95B) = (u8)q->x06;
    em->pos[0] = q->pos[0];
    em->pos[1] = q->pos[1];
    em->pos[2] = q->pos[2];
    em->ang[1] = q->x1C;
    enemy_mv(em);
    return em;
}

/* The quest's own monster on the current stage (quest loaded with
 * rt_quest_load); without a quest, kind `kind` at pos facing ang_y.
 * Returns the em_work index or -1. */
int rt_monster_spawn(int kind, const float pos[3], int ang_y)
{
    QEM *q, dflt;
    EMW *em;
    if (quest_w.no != 0) {
        /* the quest's monsters, as the game sets them up when the stage
         * is entered: station_em_set (the quest's own) + Quest_next_em_set
         * (the stage's small monsters) -> Em_direct_set */
        int i;
        Quest_em_init_set(game_w.stage);
        for (i = 0; i < 20; i++)
            if (em_work[i].be_flag && getenv("RT_EM_TRACE"))
                fprintf(stderr, "rt_em: quest monster %d kind %d stage %d at %.0f %.0f %.0f hp %d\n",
                        i, em_work[i].kind, em_work[i].stg, em_work[i].pos[0], em_work[i].pos[1],
                        em_work[i].pos[2], PS16(&em_work[i], 0x302));
        em = &em_work[0];
        if (!em->be_flag)
            return -1;
        if (getenv("RT_EM_POS")) {      /* test aid: monster 0 at the given x,z */
            sscanf(getenv("RT_EM_POS"), "%f,%f", &em->pos[0], &em->pos[2]);
        }
        if (getenv("RT_EM_HP"))         /* test aid: monster 0's hit points */
            PS16(em, 0x302) = (s16)atoi(getenv("RT_EM_HP"));
        return 0;
    }
    if (getenv("RT_EM_KIND"))      /* test aid: free play with another monster kind */
        kind = atoi(getenv("RT_EM_KIND"));
    q = &dflt;
    memset(&dflt, 0, sizeof dflt);
    dflt.id = (s16)kind;
    dflt.pos[0] = pos[0];
    dflt.pos[1] = pos[1];
    dflt.pos[2] = pos[2];
    dflt.x1C = ang_y & 0xFFFF;
    em = rt_monster_spawn_qem(q);
    if (!em)
        return -1;
    em->ang[1] = q->x1C;            /* em_status_init zeroes the angle in free hunts */
    if (getenv("RT_EM_TRACE"))
        fprintf(stderr, "rt_em: monster %d kind %d at %.0f %.0f %.0f ang %04X hp %d/%d\n",
                em->id, em->kind, em->pos[0], em->pos[1], em->pos[2], em->ang[1] & 0xFFFF,
                PS16(em, 0x302), PS16(em, 0x792));
    if (getenv("RT_EM_TRACE")) {
        int i;
        fprintf(stderr, "rt_em: part durability (kind %d):", PU8(em, 0x953));
        for (i = 0; i < 9; i++)
            fprintf(stderr, " %d", PU8(em, 0x304 + 8 * i) ? PS16(em, 0x308 + 8 * i) : -1);
        fprintf(stderr, "\n");
    }
    return em->id;
}

void rt_em_world_mat(EMW *em);
/* One game tick of monster no: the game's enemy_mv (src/main/em/f_em_nm.c). */
/* test aid RT_EM_POKE="kind:offset:value[:size][@tick];...": every game tick (or only at player tick `tick`), before the
 * monster's AI runs, write a byte (size 2: s16, 4: 32 bits, e.g. a float's bits 0x3F000000 = 0.5) of every monster of
 * that kind: broken parts (hagi counts), hit points, the 0x798 fade, a tail cut (0x957 + a hit at 0x38D) */
int rt_tick_count(void);
static void em_poke(u8 *em)
{
    const char *p = getenv("RT_EM_POKE");
    while (p && *p) {
        int k = -1, off = 0, v = 0, sz = 1, t = -1;
        const char *at = strchr(p, '@'), *end = strchr(p, ';');
        if (at && (!end || at < end))
            t = atoi(at + 1);
        if (sscanf(p, "%i:%i:%i:%i", &k, &off, &v, &sz) >= 3 && k == PU8(em, 2) && off > 0 && off < 0xA10
            && (t < 0 || t == rt_tick_count())) {
            if (sz == 4)
                PS32(em, off) = (s32)v;
            else if (sz == 2)
                PS16(em, off) = (s16)v;
            else
                PU8(em, off) = (u8)v;
        }
        p = end ? end + 1 : NULL;
    }
}

int rt_monster_tick(int no)
{
    EMW *em = &em_work[no];
    if (!em->be_flag)
        return 0;
    if (getenv("RT_EM_POKE"))
        em_poke((u8 *)em);
    if (no == 0 && getenv("RT_EM_PIN")) {   /* test aid "x,z": monster 0 is put back there every tick (it can still turn and act) */
        float px, pz;
        if (sscanf(getenv("RT_EM_PIN"), "%f,%f", &px, &pz) == 2) {
            em->pos[0] = px;
            em->pos[2] = pz;
        }
    }
    if (getenv("RT_EM_TRACE"))
        printf("em%d: stg %d step %d act %d/%d/%d char %d frame %.1f pos %.0f %.0f %.0f ang %04X hp %d mode %d mt %d/%.0f/%d pt %d tr %d/%d\n",
                no, em->stg, em->x04, PU8(em, 0x14), PU8(em, 0x15), PU8(em, 0x05), PS16(em, 0x2DC), PF(em, 0x19C),
                em->pos[0], em->pos[1], em->pos[2], em->ang[1] & 0xFFFF, PS16(em, 0x302), PU8(em, 0x888),
                PS32(em, 0x194), PF(em, 0x1A8), PS32(em, 0x1C8), PS16(em, 0x56A), PU8(em, 0x9EA), PU8(em, 0x959));
    {
        /* the world matrix at EMW+0x60, as enemy_mk (0x10AEB0) builds it in trans():
         * the host poses the skeleton itself, but game code reads this matrix
         * (demo cameras relative to the monster, em10's throw direction) */
        int r;
        /* RT_EM_BLIND=1 (test aid): the monster's eyes are shut every tick (x88B = 0 makes
         * em_eye_search_set clear x88C), so it stays idle; frog fishing needs an idle Plesioth */
        if (getenv("RT_EM_BLIND"))
            PU8(em, 0x88B) = 0;
        r = enemy_mv(em);
        if (em->be_flag)
            rt_em_world_mat(em);
        return r;
    }
}

void flmatMakeScale(FLMAT *m, f32 x, f32 y, f32 z);
FLMAT *cpRotMatrixYXZ2(s32 *ang, FLMAT *m);
void flmatSetTrans(FLMAT *m, f32 x, f32 y, f32 z);
void flmatMul33_2(FLMAT *a, FLMAT *b);
void rt_em_world_mat(EMW *em)
{
    FLMAT m, sc;
    flmatMakeScale(&sc, em->scale[0], em->scale[1], em->scale[2]);
    cpRotMatrixYXZ2(em->ang, &m);
    flmatSetTrans(&m, em->pos[0], em->pos[1], em->pos[2]);
    flmatMul33_2(&m, &sc);
    memcpy((u8 *)em + 0x60, &m, 0x40);
}

/* em_sleep_eff_set (game 0x53xxxx, em_master_nm.c): sleep bubbles at
 * joint a every 90 ticks (3 puffs, 10 apart). Callers pass (em, joint,
 * pos, scale); the PS2 definition reads two ints and leaves the scale in
 * f12 for Eft06_set2, which x86 cannot do: this is the PC definition. */
void Eft06_set2(f32 scale, void *chr, s16 arg, int joint, f32 *pos);
void em_sleep_eff_set(EMW *em, int a, f32 *pos, f32 scale)
{
    u16 t = *(u16 *)((u8 *)&game_w + 0x1E) % 90;
    if (t == 0 || t == 10 || t == 20)
        Eft06_set2(scale, em, 4, a, pos);
}

/* drawn by the host: the work is in use and on the current stage */
int rt_monster_shown(int no)
{
    EMW *em = &em_work[no];
    /* +0x1E: a village NPC (Lb_npc_set), not the quest's monster */
    return em->be_flag && !((u8 *)em)[0x1E] && em->stg == game_w.stage;
}

/* all em_work slots free (the host's quest restart) */
void rt_monster_clear_all(void)
{
    int i;
    for (i = 0; i < 20; i++)
        em_work[i].be_flag = 0;
}

/* main 0x225E90 / 0x225EA0 (not decompiled yet; written from the asm):
 * the escape camera demos of Lao-Shan Lung (em07) and Fatalis (em02).
 * F_DragonEscapeCamera picks demo 10 when EMW+0x388 is 2, else 31. */
void DemoCameraRequest(s8 no, s32 arg);
void RedDragonEscapeCamera(EMW *em)
{
    DemoCameraRequest(28, (s32)em);
}

void F_DragonEscapeCamera(EMW *em)
{
    DemoCameraRequest(((u8 *)em)[0x388] == 2 ? 10 : 31, (s32)em);
}
