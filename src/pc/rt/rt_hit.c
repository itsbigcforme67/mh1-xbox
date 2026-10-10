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
/* Stage_data_get (0x226900): src/main/quest/qstb02.c */

void PointToPoint(f32 *, f32 *, f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
void flvecNormalize(f32 *);
f32 flvecCalcLength(f32 *);
f32 flAbs(f32);

void flmatInit(FLMAT *);
void RotateX(FLMAT *, f32);
void RotateY(FLMAT *, f32);
void RotateZ(FLMAT *, f32);

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
