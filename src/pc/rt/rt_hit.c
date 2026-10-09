/*
 * rt_hit.c - what the game's stage collision C (src/main/hit/shit*.c,
 * f_sphr 0x114AE0-0x11CA74, agent D) needs from the PS2 side, so the
 * game's own ground queries, wall sweeps and PushAdjust3 run on the PC.
 *
 * Here: the hit file areas and load_file_mdl (AFS entry -> Meltw ->
 * memory, through the host's loader), and small maths helpers of main
 * that are not decompiled, written from the asm:
 *   NormalClipF3 / NormalClipCheckF3 / PointHitCheckF3 (0x120950-0x120C7C),
 *   UnitNormalVectorCCW (0x1208D0), NvecFloatAdjust (0x120C80),
 *   cpAng2Rad_all / cpRotMatrixYXZ2 (0x120310), flConvertRtoS (0x173320),
 *   Stage_data_get (0x226900).
 * The work areas (diorama_w, hit_decision, pl_wall_mat ...) and data tables
 * (ground_tbl_add, push00, em_hit_push_tbl ...) are in tables.txt.
 */
#include "rt.h"
#include "types.h"
#include "fl.h"
#include "hit3.h"
#include "quest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint8_t St_data[];           /* 0x2FAAF0, 98 x 32 bytes (tables.txt) */

/* ------------------------------------------------------------ files */
static uint8_t *(*file_loader)(int idx, size_t *n);

void rt_set_file_loader(uint8_t *(*fn)(int idx, size_t *n))
{
    file_loader = fn;
}

/* AFS_DATA entry idx, Meltw-decompressed, in a new host buffer (free it) */
uint8_t *rt_file_load(int idx, size_t *n)
{
    return file_loader && idx >= 0 ? file_loader(idx, n) : NULL;
}

/* The PS2 loads the wall / ground HITS files into fixed RAM areas
 * (stage_hit_area_w / _f hold their addresses). Here: two host buffers.
 * The largest files on the disc, decompressed: lg045.bin 164352 bytes
 * (ground), lw032.bin 81204 (wall), so 512 KB each leaves room (was 4 MB
 * each; docs/xbox.md memory). RT_MEM reports how much the game used. */
#define HIT_AREA_SIZE (512u << 10)
s32 stage_hit_area_w;
s32 stage_hit_area_f;
static uint8_t *hit_area[2];

/* Host buffers standing in for the PS2's fixed load areas: their size, and
 * how much of them files used (RT_MEM: rt_area_report). load_file_mdl
 * refuses a file that would run past the end of its area. */
#define MAXAREA 8
static struct { uint8_t *p; size_t size, high; const char *name; } areas[MAXAREA];
void rt_area_register(void *p, size_t size, const char *name)
{
    int k;
    for (k = 0; k < MAXAREA; k++)
        if (!areas[k].p || areas[k].p == p) {
            areas[k].p = p;
            areas[k].size = size;
            areas[k].name = name;
            return;
        }
}
/* 0 if n bytes at dst fit (or dst is in no known area) */
static int area_check(const uint8_t *dst, size_t n)
{
    int k;
    for (k = 0; k < MAXAREA && areas[k].p; k++)
        if (dst >= areas[k].p && dst < areas[k].p + areas[k].size) {
            size_t end = (size_t)(dst - areas[k].p) + n;
            if (end > areas[k].size)
                return -1;
            if (end > areas[k].high)
                areas[k].high = end;
            return 0;
        }
    return 0;
}
void rt_area_report(void)
{
    int k;
    for (k = 0; k < MAXAREA && areas[k].p; k++)
        fprintf(stderr, "memstat: area %-24s %8zu KB, files used up to %zu KB\n", areas[k].name,
                areas[k].size >> 10, areas[k].high >> 10);
}

/* load_file_mdl (0x11ED20): AFS entry idx, Meltw-decompressed, to dst.
 * 1 loaded, 0 failed, -1 no file (idx < 0). */
int load_file_mdl(s32 dst, s32 idx)
{
    size_t n = 0;
    uint8_t *p;
    int k;
    if (idx < 0)
        return -1;
    if (!file_loader || !(p = file_loader(idx, &n)))
        return 0;
    (void)k;
    if (area_check((const uint8_t *)dst, n) != 0) {
        fprintf(stderr, "rt: file %d (%zu bytes) does not fit its load area\n", idx, n);
        free(p);
        return 0;
    }
    memcpy((void *)dst, p, n);
    if (idx >= 1999 && idx < 1999 + 179 && rt_text_active())    /* a mission file m001..: quest text */
        rt_text_quest(idx - 1998, (uint8_t *)dst, n, 0x40000);
    free(p);
    return 1;
}

void load_stage_hit(int stg);       /* src/main/hit/shit1_nm.c */

/* Load the stage's wall and ground collision with the game's own
 * load_stage_hit (WallHitInit / GroundHitInit fix the cell pointers). */
int rt_load_stage_hit(int stage)
{
    int k;
    for (k = 0; k < 2; k++)
        if (!hit_area[k]) {
            if (!(hit_area[k] = malloc(HIT_AREA_SIZE)))
                return -1;
            rt_area_register(hit_area[k], HIT_AREA_SIZE, k ? "ground collision" : "wall collision");
        }
    if (getenv("RT_MEM")) {         /* fill pattern for the high-water check below */
        memset(hit_area[0], 0xCD, HIT_AREA_SIZE);
        memset(hit_area[1], 0xCD, HIT_AREA_SIZE);
    }
    memset(hit_area[0], 0xFF, 64);
    memset(hit_area[1], 0xFF, 64);
    stage_hit_area_w = (s32)hit_area[0];
    stage_hit_area_f = (s32)hit_area[1];
    if (!quest_w.x80)           /* the quest's stage table; St_data as quest init (0x226BD0) sets it */
        quest_w.x80 = (s32 *)St_data;
    memset(&diorama_w, 0, sizeof diorama_w);
    load_stage_hit(stage);
    if (getenv("RT_MEM"))
        for (k = 0; k < 2; k++) {
            size_t top = HIT_AREA_SIZE;
            while (top > 0 && hit_area[k][top - 1] == 0xCD)
                top--;
            fprintf(stderr, "memstat: stage %d %s collision area used %zu of %u bytes\n", stage,
                    k ? "ground" : "wall", top, HIT_AREA_SIZE);
        }
    return diorama_w.gtbl ? 0 : -1;
}

/* ------------------------------------------------------------ helpers */
/* Stage_data_get (0x226900): src/main/quest/f_quest0_nm.c */

/* NormalClipF3 (0x120950): z of the 2D cross product (b - a) x (c - a);
 * points are (x, y) pairs (callers pass x/z). When the two products are
 * one float ulp apart the result is exactly 0 (the original compares the
 * bit patterns). */
__attribute__((weak)) f32 NormalClipF3(f32 *a, f32 *b, f32 *c)
{
    union { f32 f; s32 i; } t1, t2;
    t1.f = (b[0] - a[0]) * (c[1] - a[1]);
    t2.f = (c[0] - a[0]) * (b[1] - a[1]);
    if (t1.i == t2.i + 1)
        t1.f = t2.f;
    if (t1.i == t2.i - 1)
        t1.f = t2.f;
    return t1.f - t2.f;
}

/* NormalClipCheckF3 (0x120A00): p inside triangle abc? 0 no (or a
 * degenerate triangle), 1 inside a clockwise triangle, 2 inside a
 * counter-clockwise one (edges count as inside). */
__attribute__((weak)) int NormalClipCheckF3(f32 *a, f32 *b, f32 *c, f32 *p)
{
    f32 d = NormalClipF3(a, b, c);
    if (d == 0.0f)
        return 0;
    if (d < 0.0f) {
        if (!(NormalClipF3(a, b, p) <= 0.0f)) return 0;
        if (!(NormalClipF3(b, c, p) <= 0.0f)) return 0;
        return NormalClipF3(c, a, p) <= 0.0f;
    }
    if (NormalClipF3(a, b, p) < 0.0f) return 0;
    if (NormalClipF3(b, c, p) < 0.0f) return 0;
    if (NormalClipF3(c, a, p) < 0.0f) return 0;
    return 2;
}

/* PointHitCheckF3 (0x120BA0): tri = 3 points with a stride of 3 floats
 * (x, z, -), p = (x, z). 1 when p is inside. The orientation test uses the
 * truncated integer of the cross product, as the original. */
__attribute__((weak)) int PointHitCheckF3(f32 *tri, f32 *p)
{
    int cw = (int)NormalClipF3(tri, tri + 3, tri + 6) <= 0;
    int r = NormalClipCheckF3(tri, tri + 3, tri + 6, p) & 0xFF;
    s8 v = 0;
    if (r != 0) {
        if (cw)
            v = r == 1 ? 1 : -1;
        else
            v = r == 2 ? 1 : -1;
    }
    return v > 0;
}

void PointToPoint(f32 *, f32 *, f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
void flvecNormalize(f32 *);
f32 flvecCalcLength(f32 *);
f32 flAbs(f32);

/* UnitNormalVectorCCW (0x1208D0): out = unit normal of triangle abc;
 * 0 when it has no length. */
__attribute__((weak)) int UnitNormalVectorCCW(f32 *a, f32 *b, f32 *c, f32 *out)
{
    f32 v1[4], v2[4];
    PointToPoint(v1, a, b);
    PointToPoint(v2, c, b);
    flvecOuterProduct(out, v2, v1);
    flvecNormalize(out);
    return flvecCalcLength(out) != 0.0f;
}

/* NvecFloatAdjust (0x120C80): out = v with components below 0.001 in
 * magnitude set to 0. */
__attribute__((weak)) void NvecFloatAdjust(f32 *out, f32 *v)
{
    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
    if (flAbs(v[0]) < 0.001f) out[0] = 0.0f;
    if (flAbs(v[1]) < 0.001f) out[1] = 0.0f;
    if (flAbs(v[2]) < 0.001f) out[2] = 0.0f;
}

void flmatInit(FLMAT *);
void RotateX(FLMAT *, f32);
void RotateY(FLMAT *, f32);
void RotateZ(FLMAT *, f32);

/* cpAng2Rad (u16 angle * 2pi/0x10000) for three s32 angles */
__attribute__((weak)) void cpAng2Rad_all(s32 *ang, f32 *out)
{
    int k;
    for (k = 0; k < 3; k++)
        out[k] = 9.58738e-05f * (f32)(ang[k] & 0xFFFF);
}

/* cpRotMatrixYXZ2 (0x120310): m = rotation by Y, then X, then Z
 * (each RotateN pre-multiplies). */
__attribute__((weak)) FLMAT *cpRotMatrixYXZ2(s32 *ang, FLMAT *m)
{
    f32 r[3];
    cpAng2Rad_all(ang, r);
    flmatInit(m);
    RotateY(m, r[1]);
    RotateX(m, r[0]);
    RotateZ(m, r[2]);
    return m;
}

/* flConvertRtoS (0x173320): radians to a 0x10000-per-turn angle. */
u32 flConvertRtoS(f32 r)
{
    f32 d = 180.0f * r / 3.1415927f;
    f32 s;
    if (d < 0.0f)
        d += 360.0f;
    s = 182.04445f * d;
    if (s >= 2147483648.0f)
        return (u32)(s32)(s - 2147483648.0f) | 0x80000000u;
    return (u32)(s32)s;
}

/* em_hit_push_tbl of lobby.bin (D_6103A0, monsters in the lobby; not
 * loaded on the PC): every kind gets an empty sphere list. */
static f32 no_spheres[4] = { 0, 0, 0, -1.0f };
f32 *D_6103A0[64] = {
#define E8 no_spheres, no_spheres, no_spheres, no_spheres, no_spheres, no_spheres, no_spheres, no_spheres
    E8, E8, E8, E8, E8, E8, E8, E8
#undef E8
};

f32 GetGroundHit(f32 *pos);

int rt_ground_y(float x, float z, float ymax, float *y)
{
    f32 p[3];
    f32 r;
    p[0] = x;
    p[1] = ymax - 50.0f;        /* GetGroundHit takes polygons up to pos.y + 50 */
    p[2] = z;
    if (!diorama_w.gtbl)
        return 0;
    r = GetGroundHit(p);
    if (r == p[1])              /* nothing under (x, z): pos.y comes back */
        return 0;
    *y = r;
    return 1;
}

/* Debug: the wall polygons of the cell under pos (RT_HIT_TRACE). */
s32 GetWallTblAdrs(f32 *p);
void rt_hit_dump(f32 *pos)
{
    s32 *cp = (s32 *)GetWallTblAdrs(pos);
    fprintf(stderr, "wall grid cs %d,%d n %d,%d; cell at %.0f,%.0f:\n", diorama_w.wcsx, diorama_w.wcsz,
            diorama_w.wnx, diorama_w.wnz, pos[0], pos[2]);
    for (; *cp != -1; cp++) {
        HPOLY *h = (HPOLY *)*cp;
        fprintf(stderr, "  kind %d b1 %d h2 %04X v %.0f,%.0f,%.0f %.0f,%.0f,%.0f %.0f,%.0f,%.0f n %.2f %.2f %.2f d %.0f\n",
                h->kind, h->b1, h->h2, h->v[0][0], h->v[0][1], h->v[0][2], h->v[1][0], h->v[1][1], h->v[1][2],
                h->v[2][0], h->v[2][1], h->v[2][2], h->n[0], h->n[1], h->n[2], h->d);
    }
}

/* ------------------------------------------------------------ monsters */
/* The collision part of em_move (main 0x10BF30, read from the asm): the
 * old position is saved to +0x5A0 before the move (unless +0x9F1), then
 * after frame_move: HitWallPlayer(em, 0) (spheres em_hit_push_tbl[kind]),
 * GetGroundHitStatusAreaEm(em, pos, em+0x70C, em+0x5AC, em+0x7E4); on a
 * special surface (+0x7E9, not kinds 8/0xE/0x1A/0x22) its height +0x7E4
 * is used; unless the monster is in state 2 or 4 (+0x388) its y is put on
 * that height and its X/Z tilt (+0xA0/+0xA8) cleared. The +0x95C / +0x7D6
 * bookkeeping and the off-stage branch (+0x8C3) are left out. */
#define EF(e, T, o) (*(T *)((u8 *)(e) + (o)))
void HitWallPlayer(void *ent, int keep);
int GetGroundHitStatusAreaEm(void *ent, f32 *pos, void *attr, f32 *out, f32 *flag);

void rt_monster_save_old(void *em)
{
    if (EF(em, u8, 0x9F1) == 0) {
        EF(em, f32, 0x5A0) = EF(em, f32, 0xAC);
        EF(em, f32, 0x5A4) = EF(em, f32, 0xB0);
        EF(em, f32, 0x5A8) = EF(em, f32, 0xB4);
    }
}

void rt_monster_collide(void *em)
{
    u8 st, kind;
    HitWallPlayer(em, 0);
    GetGroundHitStatusAreaEm(em, (f32 *)((u8 *)em + 0xAC), (u8 *)em + 0x70C,
                             (f32 *)((u8 *)em + 0x5AC), (f32 *)((u8 *)em + 0x7E4));
    st = EF(em, u8, 0x388);
    if (st == 2 || st == 4)
        return;
    kind = EF(em, u8, 0x2);
    if (EF(em, u8, 0x7E9) != 0 && kind != 8 && kind != 0xE && kind != 0x1A && kind != 0x22)
        EF(em, f32, 0x5AC) = EF(em, f32, 0x7E4);
    EF(em, f32, 0xB0) = EF(em, f32, 0x5AC);
    EF(em, s32, 0xA0) = 0;
    EF(em, s32, 0xA8) = 0;
}
