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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WEAK __attribute__((weak))
static void once(const char *n) { if (getenv("RT_TRACE")) fprintf(stderr, "rt_em: %s not ported (stand-in)\n", n); }
#define WSTUB(name) WEAK void name(EMW *em) { static int o; (void)em; if (!o++) once(#name); }

#define PU8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define PS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PF(p, o) (*(f32 *)((u8 *)(p) + (o)))

extern u8 player_work[];
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
        for (i = 0, pl = player_work; i < 4; i++, pl += 0xA00) {
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
