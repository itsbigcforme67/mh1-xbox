/*
 * rt_main.c - small main-program (SLPM_654.95) functions that the ported
 * game C calls but that are not decompiled yet, written natively from the
 * asm. Replace each with the decompiled C once it matches.
 */
#include "rt.h"
#include "types.h"

#include <stdio.h>

f32 flvecCalcDistance(f32 *a, f32 *b);
void flvecCopy(f32 *dst, f32 *src);

/* clr_flash (0x15C4B0): src/main/stage/f_stage.c (built). */
s16 flash_flag, flash_timer;

/* hit_cap_pk (0x28CD20): packs a capsule {p0, p1, r} for the hit tests:
 * +0 p0, +0xC p1, +0x18 r, +0x1C p1 - p0 (not normalised), +0x28 centre,
 * +0x34 bounding radius = |p0 - centre| + r. */
void hit_cap_pk(f32 *cap, f32 *pk)
{
    flvecCopy(pk, cap);
    flvecCopy(pk + 3, cap + 3);
    pk[6] = cap[6];
    pk[7] = cap[3] - cap[0];
    pk[8] = cap[4] - cap[1];
    pk[9] = cap[5] - cap[2];
    pk[10] = 0.5f * (cap[0] + cap[3]);
    pk[11] = 0.5f * (cap[1] + cap[4]);
    pk[12] = 0.5f * (cap[2] + cap[5]);
    pk[13] = flvecCalcDistance(cap, pk + 10);
    pk[13] = pk[13] + cap[6];
}

/* hit_point_cyl / hit_point_cbd: src/main/hit/hit3_nm.c (built). */

/* View-frustum culling (Create_FOV builds the clip planes in fov from the
 * camera; flCheckMeshFOV tests a sphere). Not ported: everything counts as
 * visible and the GPU clips, which only costs draw calls. out[2] gets the
 * view-space depth like the original's first step. */
u8 fov[0x48];
void Create_FOV(f32 dist, int a) { (void)dist; (void)a; }
int flCheckMeshFOV(f32 r, f32 *c, f32 *out, f32 (*view)[4], void *planes)
{
    (void)r; (void)planes;
    out[2] = c[0] * view[0][2] + c[1] * view[1][2] + c[2] * view[2][2] + view[3][2];
    return 1;
}

/* reload_tex (0x11F160): re-sends a texture list to VRAM on the PS2; the
 * host keeps every texture resident, so nothing to do. */
void reload_tex(int num, int id) { (void)num; (void)id; }

/* light_set (stage light setup) and get_tex_num (texture lookup): trans_stage
 * (src/main/stage/trans_stage.c) calls them; nothing to do on the host. */
void light_set(int n) { (void)n; }
void get_tex_num(int n) { (void)n; }

/* trans_stage_sub (0x15CD40, matches in f_stagec.c): world matrix, clay
 * attribute word (CLAY+0x88), draw clay handle (CLAY+0). */
void flSetRenderState(int, unsigned);
void clay_attr_set(int);
void flExecuteClay(int, int);
void trans_stage_sub(int m, unsigned char *cl)
{
    flSetRenderState(0x1A, (unsigned)m);
    clay_attr_set(*(int *)(cl + 0x88));
    flExecuteClay(*(int *)cl, 0);
}

/* Pl_stg_ck / Em_stg_ck (0x151FF0 / 0x152010): is this player / monster
 * (byte +0x736) on the current stage (game_w+0x14)? */
#include "game.h"

/* frame_check2 now comes from the decompiled src/main/frame/f_frame_nm.c. */

/* flvecApplyMat33_2(v, m): v = v * m (3x3), in place. */
void flvecApplyMat33(f32 *out, f32 *v, f32 (*m)[4]);
void flvecApplyMat33_2(f32 *v, f32 (*m)[4]) { flvecApplyMat33(v, v, m); }

/* (Sound: rt_snd.c. Camera quake: set_quake_sub /
 * set_quake_sub2 are the game's own now, src/main/cam.) (The effects/shells set objects spawn now run as game C.) */
#define STUB_ONCE(name) { static int once; if (!once++) fprintf(stderr, "rt: %s not ported yet (skipped)\n", name); }

/* Callees of the game tick move() (src/main/frame/f_frame_nm.c, 0x1265E0)
 * that are not ported yet. Weak, so a ported version wins when it is
 * linked in. move() itself is not called by the host loop yet. */
#define WEAK __attribute__((weak))
WEAK void player_mv(void) {}
WEAK int enemy_mv(void *w) { (void)w; return 1; }
WEAK void enemy_mk(void *w) { (void)w; }
WEAK void em_ride_sub(void *w) { (void)w; }
WEAK int npc_mv(void *w) { (void)w; return 1; }
WEAK void npc_mk(void *w) { (void)w; }
WEAK void item_check(void) {}
WEAK void body_hit(void) {}
WEAK void bgm_server(void) {}
WEAK void player_mk(void) {}
WEAK void yure_move(void) {}
WEAK void CameraMove(void) {}
WEAK void light_move(void) {}
WEAK void move_item(void) {}
WEAK void move_stage(void) {}
WEAK void Pit_mv(void) {}
/* the host runs these from rt_game_move; move() is not called yet */
WEAK void move_eft(void) {}
WEAK void move_shell(void) {}
WEAK void move_set(void) {}
WEAK void move_senko(void) {}
WEAK void move_smoke(void) {}
/* Em_max_parts_get (main 0x10B770): number of motion part groups of
 * monster kind em (em_parts_num[(s16)em]); create_em_motion builds 2 banks
 * per group. */
extern u8 em_parts_num[];
u8 Em_max_parts_get(int em) { return em_parts_num[(s16)em]; }

/* pad_timer_calc (0x1513A0) / pad_timer_calc_sub (0x151350): ticks counter
 * at PLW+0x5B8, cleared while ~sw.now & for_pad_timer_tbl[0] is non-zero,
 * else counted up to 0xFFFF. */
extern u16 for_pad_timer_tbl[];

/* Online_ck (0x162D60): system_w+0x10 != 0. The port runs offline. */
int Online_ck(void) { return 0; }
/* Cockpit_menu_chk (0x1279F0): 1 while a cockpit menu has the pad. No menus yet. */
WEAK int Cockpit_menu_chk(void) { return 0; }
/* player state changes called from pl_normal2.c (agent F's area, not ported) */
WEAK void pl_st_set(void *pl, int st) { (void)pl; (void)st; STUB_ONCE("pl_st_set") }
WEAK void to_normal(void *pl, int a, int b) { (void)pl; (void)a; (void)b; STUB_ONCE("to_normal") }
WEAK void to_normal_fly(void *pl, int a, int b) { (void)pl; (void)a; (void)b; STUB_ONCE("to_normal_fly") }
WEAK void action_timer_calc(void *pl, int a) { (void)pl; (void)a; }
