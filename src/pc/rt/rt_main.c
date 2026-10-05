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

/* clr_flash (0x15C4B0, f_stage): clears the screen-flash state. */
s16 flash_flag, flash_timer;
void clr_flash(void)
{
    flash_flag = 0;
    flash_timer = 0;
}

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

/* hit_point_cyl (0x290560): not ported yet (only set13 kind 4, stage 0x15). */
int hit_point_cyl(f32 *p, f32 *c, f32 r, f32 y0, f32 y1)
{
    static int once;
    if (!once++)
        fprintf(stderr, "rt: hit_point_cyl not ported yet (returns 0)\n");
    return 0;
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

/* Pl_stg_ck / Em_stg_ck (0x151FF0 / 0x152010): is this player / monster
 * (byte +0x736) on the current stage (game_w+0x14)? */
#include "game.h"
u8 Pl_stg_ck(void *p) { return ((u8 *)p)[0x736] == game_w.stage; }
u8 Em_stg_ck(void *p) { return ((u8 *)p)[0x736] == game_w.stage; }

/* frame_check2 (0x126500): 0 while motion slot n of the object is not
 * running (+0x1C4 set), else whether frame f <= the slot's frame
 * (+0x19C + n * 0x50). */
int frame_check2(void *p, int n, f32 f)
{
    u8 *b = p;
    if (*(s32 *)(b + 0x1C4) != 0)
        return 0;
    return f <= *(f32 *)(b + 0x19C + n * 0x50);
}

/* flvecApplyMat33_2(v, m): v = v * m (3x3), in place. */
void flvecApplyMat33(f32 *out, f32 *v, f32 (*m)[4]);
void flvecApplyMat33_2(f32 *v, f32 (*m)[4]) { flvecApplyMat33(v, v, m); }

/* Not ported yet: camera quake (CameraWork; the viewer has its own camera),
 * monster sound, and the effects/shells some set objects spawn. */
#define STUB_ONCE(name) { static int once; if (!once++) fprintf(stderr, "rt: %s not ported yet (skipped)\n", name); }
void set_quake_sub(int kind, f32 *pos) { (void)kind; (void)pos; }
void set_quake_sub2(int kind) { (void)kind; }
void Em_se_req2(void *em, int a, int b, f32 *pos, int c, int d) { (void)em; (void)a; (void)b; (void)pos; (void)c; (void)d; }
void Shell22_set2(f32 *pos, int a, int b, int c) STUB_ONCE("Shell22_set2")
void Eft17_set_ex(f32 *pos, int a, int b, f32 s) STUB_ONCE("Eft17_set_ex")
void Eft13_set_pos(f32 s, f32 *pos, int a) STUB_ONCE("Eft13_set_pos")
