/*
 * rt_main.c - small main-program (SLPM_654.95) functions that the ported
 * game C calls but that are not decompiled yet, written natively from the
 * asm. Replace each with the decompiled C once it matches.
 */
#include "rt.h"
#include "types.h"

f32 flvecCalcDistance(f32 *a, f32 *b);
void flvecCopy(f32 *dst, f32 *src);

/* the thunder flash state (main 0x38A114; clr_flash and move_stage in src/main/stage use it) */
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


/* flvecApplyMat33_2(v, m): v = v * m (3x3), in place. */
void flvecApplyMat33(f32 *out, f32 *v, f32 (*m)[4]);
void flvecApplyMat33_2(f32 *v, f32 (*m)[4]) { flvecApplyMat33(v, v, m); }

/* Callees of the game tick move() (src/main/frame/f_frame_nm.c, 0x1265E0) that nothing linked defines:
 * move() is not called by the host loop (viewer.c sim_tick runs its steps, rt_game_move runs the effect,
 * shell and set lists), so these only satisfy the link. Weak, so a ported version wins when it is linked in. */
#define WEAK __attribute__((weak))
WEAK int npc_mv(void *w) { (void)w; return 1; }
WEAK void npc_mk(void *w) { (void)w; }
WEAK void yure_move(void) {}
WEAK void move_eft(void) {}
WEAK void move_shell(void) {}
WEAK void move_set(void) {}



/* Online_ck (0x162D60): system_w+0x10 != 0. The port runs offline. */
#ifndef MH1_ONLINE     /* ONLINE=1: rt_np.c (on during a co-op quest) */
int Online_ck(void) { return 0; }
#endif
