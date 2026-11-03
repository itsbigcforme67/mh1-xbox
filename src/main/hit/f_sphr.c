/* f_sphr - SLPM_654.95 0x00114AE0-0x0011CDE4: stage collision ("HITS" wall and
 * ground files), one translation unit in the original's address order with its
 * LOCAL functions static (WallHitInit, GroundHitInit, BlockPlaceCgeck,
 * sphr_face_o3/o4, FaceLinePos, PushAdjust3, hosei_sub, add_vec_sub2,
 * check_slide, check_angle).
 *  - load_stage_hit loads the wall and ground files of a stage and turns their
 *    offsets into pointers; diorama_w keeps the grids.
 *  - GetWallHitBitPl/Em/2: swept-sphere wall tests (sphr_face_o3/o4 per
 *    polygon, PushAdjust3/hosei_sub work out the push-back).
 *  - GetGround*: ground heights under a point, GetWater/Tenjo/YouganHit the
 *    water, ceiling and lava polygons, GetWallHitLine/GetEyeHitLine line of
 *    sight, GetFloorSlide the slide on steep floors, HitWallPlayer the per
 *    entity wall pass. Names are the original symbols; meanings are guesses
 *    from the code.
 * 25 of the 34 functions match; the 9 others are kept as original bytes on the
 * PS2 (config/c_rawfuncs.txt) with their near-match C in the #else branch,
 * which the PC/Xbox build compiles. Data tables (slide_tbl, push00, the three
 * initialised offset tables @180/@425/@638) are still in the asm data. */
#include "types.h"
#include "hit3.h"
#include "game.h"
#include "flow.h"

/* GetWallHitLine/GetEyeHitLine: the original only compares against the first
 * remembered polygon. */
#define GCSX ((u32)diorama_w.gcsx)
#define GCSZ ((u32)diorama_w.gcsz)
#define SEEN(poly, n, seen, found)           \
    found = ((n) > 0 && (poly) == (seen)[0]);

#define EF(e, T, o) (*(T *)((u8 *)(e) + MH2_OFS(e, o)))

typedef struct STAGE_SZ {
    u8 _pad00[0x10];
    f32 sx;             /* 0x10 stage size x */
    f32 sz;             /* 0x14 stage size z */
    f32 floor;          /* 0x18 */
    f32 floor2;         /* 0x1C */
} STAGE_SZ;

typedef struct HV3 { f32 x, y, z; } HV3;
typedef struct SV3 { s32 x, y, z; } SV3;

typedef struct GATTR {
    u8 kind;
    u8 b1;
    u16 h2;
} GATTR;

typedef struct WMAT {
    u16 flags;          /* 0x00 1 = wall touched, 2 = special wall */
    u16 ang;            /* 0x02 direction of the contact relative to the facing */
    f32 *n;             /* 0x04 polygon normal */
    u8 kind;            /* 0x08 polygon b1 */
    u8 _pad09[3];
} WMAT;                 /* 0xC */

/* entity (player 0xA00 / monster 0xA10 bytes): the fields HitWallPlayer reads */
#ifdef VERSION_MH2
/* MH2's places (PLW 0x970 / EMW 0x9D0 bytes), each read from MH2's own functions of this file: GetGroundHitStatusAreaPl/Em
 * (sub_main 0x32D7A0, 0x32DD20), GetGroundHitArea(Upper), GetWallHitBitPl/Em (0x328CD0, 0x3294A0), HitWallPlayer 0x330EB0,
 * GetEyeHitLine 0x32F220. One function serves both structs, so these hold for PLW and EMW. */
typedef struct HENT {
    u8 _pad000[2];
    u8 kind;            /* 0x002 */
    u8 _pad003[0xC - 3];
    u16 x0C;            /* 0x00C */
    u8 _pad00E[0x10 - 0xE];
    u8 mon;             /* 0x010 */
    u8 _pad011[0x14 - 0x11];
    u8 x14;             /* 0x014 */
    u8 x15;             /* 0x015 */
    u8 _pad016[0xA4 - 0x16];
    s32 ang_y;          /* 0x0A4 */
    u8 _pad0A8[4];
    f32 pos[3];         /* 0x0AC */
    f32 scl[3];         /* 0x0B8 */
    u8 _pad0C4[0x390 - 0xC4];
    u8 st;              /* 0x390 (MH1 0x388) */
    u8 _pad391[0x437 - 0x391];
    u8 x4D4;            /* 0x437 (MH1 0x4D4) */
    u8 _pad438[0x564 - 0x438];
    u8 x59C;            /* 0x564 (MH1 0x59C) */
    u8 _pad565[3];
    f32 old[3];         /* 0x568 (MH1 0x5A0) */
    u8 _pad574[0x584 - 0x574];
    f32 x600;           /* 0x584 (MH1 0x600) */
    u8 x604;            /* 0x588 (MH1 0x604) */
    u8 _pad589[0x660 - 0x589];
    GATTR gat;          /* 0x660 (MH1 0x70C) */
    u8 _pad664[0x680 - 0x664];
    u16 stg;            /* 0x680 (MH1 0x736, a byte there; MH2 reads a halfword) */
    u8 _pad682[0x698 - 0x682];
    s32 hitdir;         /* 0x698 (MH1 0x74C) */
    u8 _pad69C[0x72C - 0x69C];
    f32 x7E0;           /* 0x72C (MH1 0x7E0) */
    u8 _pad730[0x734 - 0x730];
    u8 x7E8;            /* 0x734 (MH1 0x7E8) */
    u8 gkind;           /* 0x735 (MH1 0x7E9) */
    u8 _pad736[0x8B1 - 0x736];
    u8 x95D;            /* 0x8B1 (MH1 0x95D) */
    u16 wmask;          /* 0x8B2 (MH1 0x95E) */
    u8 _pad8B4[0x984 - 0x8B4];
    u16 size_pct;       /* 0x984 (MH1 0xA02), monster only */
} HENT;
#else
typedef struct HENT {
    u8 _pad000[2];
    u8 kind;            /* 0x002 */
    u8 _pad003[0xC - 3];
    u16 x0C;            /* 0x00C player index (row of pl_wall_mat) */
    u8 _pad00E[0x10 - 0xE];
    u8 mon;             /* 0x010 non-zero: monster */
    u8 _pad011[0x14 - 0x11];
    u8 x14;             /* 0x014 zero, with x15 in 0x27-0x2B: wall mask 0x7FFF (GetWallHitBitPl) */
    u8 x15;             /* 0x015 */
    u8 _pad016[0xA4 - 0x16];
    s32 ang_y;          /* 0x0A4 */
    u8 _pad0A8[4];
    f32 pos[3];         /* 0x0AC */
    f32 scl[3];         /* 0x0B8 */
    u8 _pad0C4[0x388 - 0xC4];
    u8 st;              /* 0x388 */
    u8 _pad389[0x4D4 - 0x389];
    u8 x4D4;            /* 0x4D4 */
    u8 _pad4D5[0x59C - 0x4D5];
    u8 x59C;            /* 0x59C */
    u8 _pad59D[3];
    f32 old[3];         /* 0x5A0 */
    u8 _pad5AC[0x600 - 0x5AC];
    f32 x600;           /* 0x600 monster base scale (G HitWallPlayer: the wall spheres use it for sizes 60-200 %) */
    u8 x604;            /* 0x604 non-zero: report the ground attribute b1 as 2 */
    u8 _pad605[0x70C - 0x605];
    GATTR gat;          /* 0x70C attribute of the ground under the entity (last frame) */
    u8 _pad710[0x736 - 0x710];
    u8 stg;             /* 0x736 stage number */
    u8 _pad737[0x74C - 0x737];
    s32 hitdir;         /* 0x74C */
    u8 _pad750[0x7E0 - 0x750];
    f32 x7E0;           /* 0x7E0 subtracted from the water height (monster ground query) */
    u8 _pad7E4[0x7E8 - 0x7E4];
    u8 x7E8;            /* 0x7E8 cleared by GetGroundHitStatusAreaPl/Em */
    u8 gkind;           /* 0x7E9 special ground kind under the entity (HKIND x0E) */
    u8 _pad7EA[0x95D - 0x7EA];
    u8 x95D;            /* 0x95D monster touches a wall whose kind has flag byte 1 set (GetWallHitBitEm) */
    u16 wmask;          /* 0x95E */
    u8 _pad960[0xA02 - 0x960];
    u16 size_pct;       /* 0xA02 (G 0x9F8: the PC keeps MH1's monster layout, see EMW in include/em.h) monster size in percent (60-200) */
} HENT;
#endif

void UnitNormalVectorCCW(f32 *, f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
void NvecFloatAdjust(f32 *, f32 *, void *);
f32 flAbs(f32);
f32 flSqrt(f32);
f32 flArcCos(f32);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void flvecNormalize(f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void cpRotMatrixYXZ2(s32 *, f32 *);
u16 calc_vec_ang(f32, f32, f32, f32);
int PointHitCheckF3(f32 *, f32 *);
int Pl_stg_ck(void *);
STAGE_SZ *Stage_data_get(int);
void load_file_mdl(s32, s32);

extern s32 stage_hit_area_w;
extern s32 stage_hit_area_f;
extern s32 stage_hit_data_w[];
extern s32 stage_hit_data_f[];
extern HKIND *ground_tbl_add[];
extern u8 *wall_tbl_add[];
extern WMAT pl_wall_mat[][21];
extern s8 hited_wall_no[];
extern f32 slide_tbl[];
extern f32 lit_180_002E85D0[8];
extern f32 lit_425_002E85F0[8];
extern f32 lit_638_002E8610[8];
extern f32 push00[][4];
extern f32 *D_63FC50[];
extern f32 *D_6103A0[];

int GroundFieldInCheck(f32 *);
int WallFieldInCheck(f32 *);
s32 *GetWallTblAdrs(f32 *);
s32 *GetGroundTblAdrs(f32 *);
void GetWallHitBitPl(f32, f32 *, u16, HENT *);
void GetWallHitBitEm(f32, f32 *, u16, HENT *);
static void WallHitInit(u32 *);
static void GroundHitInit(u32 *);
static int BlockPlaceCgeck(f32 *);
static int sphr_face_o3(HSWEEP *, HPOLY *, f32 *);
static int sphr_face_o4(HSWEEP *, HPOLY *, f32 *);
static int FaceLinePos(f32 *, f32 *, f32 *, f32 *);
int NormalClipFace(f32 *, f32 *, f32 *);
static int PushAdjust3(HSWEEP *, f32 *, u8 *);
static int hosei_sub(HSWEEP *, f32 *, s8, u8 *);
static void add_vec_sub2(f32 *, f32 *, f32);
static int check_slide(HPOLY *, u8 *, s32 *, f32 *, f32);
static void check_angle(f32 *, s32 *);

#ifndef VERSION_MHG
void load_stage_hit(int stg) {
    int off = stg * 4;
    s32 a;

    a = stage_hit_area_w;
    load_file_mdl(a, *(s32 *)((u8 *)stage_hit_data_w + off));
    WallHitInit((u32 *)a);
    a = stage_hit_area_f;
    load_file_mdl(a, *(s32 *)((u8 *)stage_hit_data_f + off));
    GroundHitInit((u32 *)a);
}
#else
/* Monster Hunter G (main 0x1046E0, from the asm; not matched yet): one file per stage,
 * lwgNNN.bin, a link of the wall HITS (entry 0) and the ground HITS (entry 1), loaded
 * to data_load_ptr and copied to the two areas. stage_hit_data_w is G's per-stage
 * table of lwg file numbers. */
extern void *data_load_ptr;
void *GetLinkFileAddress(u8 *, int);
long GetLinkFileSize(u8 *, int);
void *memcpy(void *, const void *, unsigned int);
void load_stage_hit(int stg) {
    u8 *buf = data_load_ptr;
    s32 a;

    load_file_mdl((s32)buf, *(s32 *)((u8 *)stage_hit_data_w + stg * 4));
    a = stage_hit_area_w;
    memcpy((void *)a, GetLinkFileAddress(buf, 0), GetLinkFileSize(buf, 0));
    WallHitInit((u32 *)a);
    a = stage_hit_area_f;
    memcpy((void *)a, GetLinkFileAddress(buf, 1), GetLinkFileSize(buf, 1));
    GroundHitInit((u32 *)a);
}
#endif

static void WallHitInit(u32 *h) {
    u8 *base = (u8 *)h + 8;
    u32 i;
    u32 j;
    s32 **cell;
    int k;

    diorama_w.wcsx = h[2];
    diorama_w.wcsz = h[3];
    diorama_w.wnx = h[4];
    diorama_w.wnz = h[5];
    diorama_w.wtbl = (s32 **)(base + h[8]);
    diorama_w.warea = base + h[9];
    for (i = 0; i < *(u32 *)(base + 8); i++) {
        for (j = 0; j < *(u32 *)(base + 0xC); j++) {
            cell = &diorama_w.wtbl[j + i * *(u32 *)(base + 0xC)];
            *cell = (s32 *)((u8 *)*cell + (s32)base);
            for (k = 0; (*cell)[k] != -1; k++) {
                (*cell)[k] += (s32)diorama_w.warea;
            }
        }
    }
}

static void GroundHitInit(u32 *h) {
    u8 *base = (u8 *)h + 8;
    u32 i;
    u32 j;
    s32 **cell;
    int k;

    diorama_w.gcsx = h[2];
    diorama_w.gcsz = h[3];
    diorama_w.gnx = h[4];
    diorama_w.gnz = h[5];
    diorama_w.gtbl = (s32 **)(base + h[8]);
    diorama_w.garea = base + h[9];
    for (i = 0; i < *(u32 *)(base + 8); i++) {
        for (j = 0; j < *(u32 *)(base + 0xC); j++) {
            cell = &diorama_w.gtbl[j + i * *(u32 *)(base + 0xC)];
            *cell = (s32 *)((u8 *)*cell + (s32)base);
            for (k = 0; (*cell)[k] != -1; k++) {
                (*cell)[k] += (s32)diorama_w.garea;
            }
        }
    }
}

/* GetWallHitBitPl: 2 x 2 cells, player wall mask 0x7FFF (stage 0x27-0x2B in
 * the arena: 0xFFFF otherwise). r is the player radius. */
/* GetWallHitBitPl: original bytes kept (build/raw/GetWallHitBitPl.inc, config/c_rawfuncs.txt); the C below is the
 * near-match the PC build uses (51 lines off, tools/align.py). */
#ifdef __MWERKS__
asm void GetWallHitBitPl(f32 r, f32 *seg, u16 mask, HENT *pl)
{
#include "GetWallHitBitPl.inc"
}
#else
void GetWallHitBitPl(f32 r, f32 *seg, u16 mask, HENT *pl) {
    HV3 c;
    HSWEEP sw;
    HPOLY *seen[20];
    f32 ofs[8] = { -1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f };
    u8 flag;
    int m;
    int i;
    int cx;
    int cz;
    int quad;
    int k;
    int j;
    int found;
    int nseen;
    int nwall;
    HPOLY *hp;
    HPOLY **cell;
    f32 *pos;
    u16 wm;
    u8 n;
    HPOLY *hp2;
    u8 special;
    s8 w;
    u32 dir;
    u16 f;
    u8 kd;
    u16 a;
    f32 *nn;

    flag = 0;
    if (!WallFieldInCheck(seg + 3)) return;
    hit_poly_num = 0;
    hited_poly_num = 0;
    *(HV3 *)sw.p0 = *(HV3 *)(seg + 3);
    *(HV3 *)sw.p1 = *(HV3 *)seg;
    nseen = 0;
    nwall = 0;
    found = 0;
    pos = pl->pos;
    sw.r = r;
    sw.w20 = 1;
    sw.w22 = 1;
    sw.len = flvecCalcDistance(sw.p0, sw.p1);
    if (pl->x14 != 0 || pl->x15 < 0x27 || pl->x15 > 0x2B) {
        wm = 0xFFFF;
    } else {
        wm = 0x7FFF;
    }
    quad = BlockPlaceCgeck(seg + 3);
    cx = (int)(seg[3] / (f32)diorama_w.wcsx);
    cz = (int)(seg[5] / (f32)diorama_w.wcsz);
    c = *(HV3 *)(seg + 3);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            c.x = (f32)diorama_w.wcsx * ((f32)cx + ofs[quad * 2]);
            c.z = (f32)diorama_w.wcsz * ((f32)cz + ofs[quad * 2 + 1]);
            c.x += (f32)((diorama_w.wcsx >> 1) + diorama_w.wcsx * i);
            c.z += (f32)((diorama_w.wcsz >> 1) + diorama_w.wcsz * j);
            if (WallFieldInCheck(&c.x) == 1) {
                cell = (HPOLY **)GetWallTblAdrs(&c.x);
                for (k = 0; (s32)cell[k] != -1; k++) {
                    hp = cell[k];
                    if ((hp->h2 & wm) & mask) continue;
                    if (nseen != 0) {
                        found = 0;
                        for (m = 0; m < nseen; m++) {
                            if (hp == seen[m]) found = 1;
                        }
                        if (found) continue;
                    }
                    if (nwall != 0) {
                        found = 0;
                        for (m = 0; m < nwall; m++) {
                            if (hp == hit_wall[m]) found = 1;
                        }
                    }
                    if (found) continue;
                    if (sphr_face_o3(&sw, hp, pos)) {
                        hit_wall[nwall++] = hp;
                    }
                    seen[nseen++] = hp;
                    if (nseen >= 20) nseen = 0;
                }
            }
        }
    }
    PushAdjust3(&sw, pos, &flag);
    if (flag) flag = 0;
    n = 0;
    pl_wall_mat[pl->x0C][0].flags = 0;
    special = 0;
    if (hited_poly_num != 0) {
        for (k = 0; k < hited_poly_num; k++) {
            w = hited_wall_no[k];
            hp2 = hit_wall[w];
            dir = (u16)((u16)((u16)(calc_vec_ang(hit_near_point[w][0], hit_near_point[w][2], pos[0], pos[2]) + 0x4000) - pl->ang_y) - 0x400);
            pl_wall_mat[pl->x0C][n].flags = 1;
            if (*(wall_tbl_add[game_w.stage] + hp2->kind * 8) != 0) {
                special = 1;
                pl_wall_mat[pl->x0C][n].flags |= 2;
            }
            pl_wall_mat[pl->x0C][n].kind = hp2->b1;
            pl_wall_mat[pl->x0C][n].ang = dir;
            pl_wall_mat[pl->x0C][n].n = hp2->n;
            n++;
            if (special && n != 1) {
                special = 0;
                f = pl_wall_mat[pl->x0C][0].flags;
                kd = pl_wall_mat[pl->x0C][0].kind;
                nn = pl_wall_mat[pl->x0C][0].n;
                a = pl_wall_mat[pl->x0C][0].ang;
                pl_wall_mat[pl->x0C][0].flags = pl_wall_mat[pl->x0C][(u8)(n - 1)].flags;
                pl_wall_mat[pl->x0C][0].kind = pl_wall_mat[pl->x0C][(u8)(n - 1)].kind;
                pl_wall_mat[pl->x0C][0].ang = pl_wall_mat[pl->x0C][(u8)(n - 1)].ang;
                pl_wall_mat[pl->x0C][0].n = pl_wall_mat[pl->x0C][(u8)(n - 1)].n;
                pl_wall_mat[pl->x0C][(u8)(n - 1)].flags = f;
                pl_wall_mat[pl->x0C][(u8)(n - 1)].kind = kd;
                pl_wall_mat[pl->x0C][(u8)(n - 1)].ang = a;
                pl_wall_mat[pl->x0C][(u8)(n - 1)].n = nn;
            }
            pl->hitdir |= 1 << (dir >> 11);
        }
        pl_wall_mat[pl->x0C][n].flags = 0;
    }
}
#endif

/* GetWallHitBitEm: monsters; the grid area is sized by the sphere (n x n
 * cells, 2x2 when small). */
void GetWallHitBitEm(f32 r, f32 *seg, u16 mask, HENT *em) {
    HV3 c;
    HSWEEP sw;
    HPOLY *seen[20];
    f32 ofs[8] = { -1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f };
    u8 flag;
    int m;
    int i;
    int cx;
    int cz;
    int quad;
    int n;
    int k;
    int j;
    int found;
    int nseen;
    int nwall;
    HPOLY *hp;
    HPOLY **cell;
    f32 *pos;
    s8 w;
    u32 dir;
    u8 n8;
    HPOLY *hp2;

    if (!WallFieldInCheck(seg + 3)) return;
    hit_poly_num = 0;
    hited_poly_num = 0;
    pos = em->pos;
    *(HV3 *)sw.p0 = *(HV3 *)(seg + 3);
    *(HV3 *)sw.p1 = *(HV3 *)seg;
    nseen = 0;
    nwall = 0;
    found = 0;
    sw.r = r;
    sw.w20 = 1;
    sw.w22 = 1;
    sw.len = flvecCalcDistance(sw.p0, sw.p1);
    if (diorama_w.wcsx <= diorama_w.wcsz) {
        n = (int)(2.0f * r / (f32)diorama_w.wcsx) + 2;
    } else {
        n = (int)(2.0f * r / (f32)diorama_w.wcsz) + 2;
    }
    if (n == 2) quad = BlockPlaceCgeck(seg + 3);
    cx = (int)(seg[3] / (f32)diorama_w.wcsx);
    cz = (int)(seg[5] / (f32)diorama_w.wcsz);
    c = *(HV3 *)(seg + 3);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (n == 2) {
                c.x = (f32)diorama_w.wcsx * ((f32)cx + ofs[quad * 2]);
                c.z = (f32)diorama_w.wcsz * ((f32)cz + ofs[quad * 2 + 1]);
                c.x += (f32)((diorama_w.wcsx >> 1) + diorama_w.wcsx * i);
                c.z += (f32)((diorama_w.wcsz >> 1) + diorama_w.wcsz * j);
            } else {
                c.x = (f32)cx * (f32)diorama_w.wcsx - (f32)(n / 2) * (f32)diorama_w.wcsx;
                c.z = (f32)cz * (f32)diorama_w.wcsz - (f32)(n / 2) * (f32)diorama_w.wcsz;
                c.x += (f32)(diorama_w.wcsx * i);
                c.z += (f32)(diorama_w.wcsz * j);
            }
            if (WallFieldInCheck(&c.x) == 1) {
                cell = (HPOLY **)GetWallTblAdrs(&c.x);
                for (k = 0; (s32)cell[k] != -1; k++) {
                    hp = cell[k];
                    if (hp->h2 & mask) continue;
                    if (nseen != 0) {
                        found = 0;
                        for (m = 0; m < nseen; m++) {
                            if (hp == seen[m]) found = 1;
                        }
                        if (found) continue;
                    }
                    if (nwall != 0) {
                        found = 0;
                        for (m = 0; m < nwall; m++) {
                            if (hp == hit_wall[m]) found = 1;
                        }
                    }
                    if (found) continue;
                    if (sphr_face_o3(&sw, hp, pos)) {
                        hit_wall[nwall++] = hp;
                    }
                    seen[nseen++] = hp;
                    if (nseen >= 20) nseen = 0;
                }
            }
        }
    }
    PushAdjust3(&sw, pos, &flag);
    em->x95D = 0;
    for (k = 0; k < hited_poly_num; k++) {
        w = hited_wall_no[k];
        hp2 = hit_wall[w];
        dir = (u16)((u16)((u16)(calc_vec_ang(hit_near_point[w][0], hit_near_point[w][2], pos[0], pos[2]) + 0x4000) - em->ang_y) - 0x400);
        em->hitdir |= 1 << (dir >> 11);
        if ((wall_tbl_add[game_w.stage] + hp2->kind * 8)[1] != 0) {
            em->x95D = 1;
        }
    }
}

int GetWallHitBit2(f32 r, f32 *a, f32 *b, f32 *pos, u16 mask) {
    HV3 c;
    HSWEEP sw;
    HPOLY *seen[20];
    f32 ofs[8] = { -1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f };
    u8 flag;
    int k;
    int m;
    int i;
    int n;
    int j;
    int found;
    int nseen;
    int nwall;
    int quad;
    HPOLY *pl;
    HPOLY **cell;

    if (!WallFieldInCheck(b)) return -1;
    hit_poly_num = 0;
    *(HV3 *)sw.p0 = *(HV3 *)b;
    *(HV3 *)sw.p1 = *(HV3 *)a;
    nseen = 0;
    nwall = 0;
    found = 0;
    sw.r = r;
    sw.w20 = 0;
    sw.w22 = 0;
    sw.len = flvecCalcDistance(sw.p0, sw.p1);
    if ((u32)diorama_w.wcsx <= (u32)diorama_w.wcsz) {
        n = (int)((sw.len + 2.0f * r) / (f32)(u32)diorama_w.wcsx) + 2;
    } else {
        n = (int)((sw.len + 2.0f * r) / (f32)(u32)diorama_w.wcsz) + 2;
    }
    if (n == 2) quad = BlockPlaceCgeck(b);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (n == 2) {
                c = *(HV3 *)a;
                c.x += (f32)(u32)diorama_w.wcsx * ofs[quad * 2];
                c.z += (f32)(u32)diorama_w.wcsz * ofs[quad * 2 + 1];
                c.x += (f32)(u32)(diorama_w.wcsx * i);
                c.z += (f32)(u32)(diorama_w.wcsz * j);
            } else {
                c = *(HV3 *)a;
                c.x -= (f32)(n / 2) * (f32)(u32)diorama_w.wcsx;
                c.z -= (f32)(n / 2) * (f32)(u32)diorama_w.wcsz;
                c.x += (f32)(u32)(diorama_w.wcsx * i);
                c.z += (f32)(u32)(diorama_w.wcsz * j);
            }
            if (WallFieldInCheck(&c.x) == 1) {
                cell = (HPOLY **)GetWallTblAdrs(&c.x);
                for (k = 0; (s32)cell[k] != -1; k++) {
                    pl = cell[k];
                    if (pl->h2 & mask) continue;
                    if (nseen != 0) {
                        found = 0;
                        for (m = 0; m < nseen; m++) {
                            if (pl == seen[m]) found = 1;
                        }
                        if (found) continue;
                    }
                    if (nwall != 0) {
                        found = 0;
                        for (m = 0; m < nwall; m++) {
                            if (pl == hit_wall[m]) found = 1;
                        }
                    }
                    if (found) continue;
                    if (sphr_face_o4(&sw, pl, pos)) {
                        hit_wall[nwall++] = pl;
                    }
                    seen[nseen++] = pl;
                    if (nseen >= 20) nseen = 0;
                }
            }
        }
    }
    return PushAdjust3(&sw, pos, &flag);
}

static int BlockPlaceCgeck(f32 *p) {
    f32 x;
    int iz;
    int r;
    u32 t;
    int ix;
    f32 z;
    u32 cx;
    u32 cz;

    z = p[2];
    r = 0;
    cz = diorama_w.wcsz;
    iz = (int)(z / cz);
    x = p[0];
    cx = diorama_w.wcsx;
    ix = (int)(x / cx);
    t = (cx >> 1) + ix * cx;
    if (!(x < t)) r = 1;
    t = (cz >> 1) + iz * cz;
    if (!(z < t)) r += 2;
    return r;
}

int GroundFieldInCheck(f32 *p) {
    f32 x = p[0];
    f32 z;

    if (x < 8.0f || (z = p[2]) < 8.0f) return 0;
    if (x < (f32)((u32)diorama_w.gcsx * (u32)diorama_w.gnx) - 8.0f && z < (f32)((u32)diorama_w.gcsz * (u32)diorama_w.gnz) - 8.0f) return 1;
    return 0;
}

int WallFieldInCheck(f32 *p) {
    f32 x = p[0];
    f32 z;

    if (x < 8.0f || (z = p[2]) < 8.0f) return 0;
    if (x < (f32)((u32)diorama_w.wcsx * (u32)diorama_w.wnx) - 8.0f && z < (f32)((u32)diorama_w.wcsz * (u32)diorama_w.wnz) - 8.0f) return 1;
    return 0;
}

int AreaFieldInCheck(int stg, f32 *p) {
    STAGE_SZ *sd = Stage_data_get(stg & 0xFFFF);
    f32 x = p[0];
    f32 z;

    if (x < 8.0f || (z = p[2]) < 8.0f) return 0;
    if (x < sd->sx - 8.0f && z < sd->sz - 8.0f) return 1;
    return 0;
}

s32 *GetWallTblAdrs(f32 *p) {
    f32 csz = (u32)diorama_w.wcsz;
    f32 csx = (u32)diorama_w.wcsx;
    int iz = (int)(p[2] / csz);
    int ix = (int)(p[0] / csx);

    return diorama_w.wtbl[iz + ix * diorama_w.wnz];
}

/* sphr_face_o3 (0x00116A00): same test for players (no radius-adjusted height
 * test, nearest points keep their own y). */
/* sphr_face_o3: original bytes kept (build/raw/sphr_face_o3.inc, config/c_rawfuncs.txt); the C below is the
 * near-match the PC build uses (33 lines off, tools/align.py). */
#ifdef __MWERKS__
static asm int sphr_face_o3(HSWEEP *sw, HPOLY *pl, f32 *pos)
{
#include "sphr_face_o3.inc"
}
#else
static int sphr_face_o3(HSWEEP *sw, HPOLY *pl, f32 *pos) {
    HV3 q;
    f32 e[3];
    HV3 d;
    HV3 best;
    f32 c[3];
    HV3 vt[4];
    HPOLY *hd;
    f32 t;
    f32 dist;
    int r;
    s8 side;
    HV3 *v;
    int i;
    int r2;
    int i2;

    if (sw->w22 != 0) {
        t = pos[1];
        if (!(t < pl->v[0][1]) && !(t < pl->v[1][1]) && !(t < pl->v[2][1])) return 0;
    }
    PointToPoint(e, sw->p1, pl->v[0]);
    if ((s8)!(flvecInnerProduct(e, pl->n) > 0.0f)) return 0;
    PointToPoint(e, sw->p0, pl->v[0]);
    t = flvecInnerProduct(e, pl->n);
    if (!(t <= 0.0f)) {
        side = 0;
    } else {
        side = 1;
    }
    d = *(HV3 *)sw->p0;
    if (flAbs(t) < sw->r) {
        ScaleVector(&q.x, pl->n, t);
        PointToPoint(&q.x, sw->p0, &q.x);
    } else {
        if (!FaceLinePos(sw->p0, pl->n, pl->v[0], &q.x)) return 0;
        side |= 2;
    }
    if (NormalClipFace(&q.x, pl->v[0], pl->n)) {
        if (hit_poly_num >= 19) return 0;
        hd = &hit_decision[hit_poly_num];
        *hd = *pl;
        NvecFloatAdjust(hd->n, hd->n, hd);
        *(HV3 *)hit_hosei_base[hit_poly_num] = d;
        hit_side[hit_poly_num] = side;
        hit_area_out[hit_poly_num] = 0;
        *(HV3 *)hit_near_point[hit_poly_num] = q;
        hit_poly_num++;
        return 1;
    }
    dist = 8000.0f;
    hit_area_out[hit_poly_num] = 1;
    v = vt;
    vt[0] = *(HV3 *)pl->v[0];
    vt[1] = *(HV3 *)pl->v[1];
    vt[2] = *(HV3 *)pl->v[2];
    vt[3] = *(HV3 *)pl->v[0];
    SetVector(c, q.x, q.y, q.z);
    if (!(side & 2)) {
        for (i = 0; i < 3; i++, v++) {
            r = 0;
            PointToPoint(e, sw->p0, &v->x);
            PointToPoint(&d.x, &v[1].x, &v->x);
            flvecNormalize(&d.x);
            if (flvecInnerProduct(e, &d.x) < 0.0f) r |= 1;
            PointToPoint(e, sw->p0, &v[1].x);
            t = flvecInnerProduct(e, &d.x);
            if (!(t <= 0.0f)) r |= 2;
            switch (r) {
            case 0:
                ScaleVector(&q.x, &d.x, t);
                AddVector(&q.x, &q.x, &v[1].x);
                break;
            case 1:
                q = v[0];
                break;
            case 2:
                q = v[1];
                break;
            case 3:
                return 0;
            }
            t = flvecCalcDistance(sw->p0, &q.x);
            if (t < dist) {
                dist = t;
                best = q;
            }
        }
    } else {
        for (i2 = 0; i2 < 3; i2++, v++) {
            r2 = 0;
            PointToPoint(e, c, &v->x);
            PointToPoint(&d.x, &v[1].x, &v->x);
            flvecNormalize(&d.x);
            if (flvecInnerProduct(e, &d.x) < 0.0f) r2 |= 1;
            PointToPoint(e, c, &v[1].x);
            t = flvecInnerProduct(e, &d.x);
            if (!(t <= 0.0f)) r2 |= 2;
            switch (r2) {
            case 0:
                ScaleVector(&q.x, &d.x, t);
                AddVector(&q.x, &q.x, &v[1].x);
                break;
            case 1:
                q = v[0];
                break;
            case 2:
                q = v[1];
                break;
            case 3:
                return 0;
            }
            t = flvecCalcDistance(c, &q.x);
            if (t < dist) {
                dist = t;
                best = q;
            }
        }
    }
    if (dist < sw->r) {
        if (hit_poly_num >= 19) return 0;
        hd = &hit_decision[hit_poly_num];
        *hd = *pl;
        NvecFloatAdjust(hd->n, hd->n, hd);
        *(HV3 *)hit_hosei_base[hit_poly_num] = *(HV3 *)sw->p0;
        hit_side[hit_poly_num] = side;
        *(HV3 *)hit_near_point[hit_poly_num] = best;
        *(HV3 *)hit_kouten[hit_poly_num] = *(HV3 *)c;
        hit_poly_num++;
        return 1;
    }
    return 0;
}
#endif

/* sphr_face_o4 (0x00117190): monsters. */
/* sphr_face_o4: original bytes kept (build/raw/sphr_face_o4.inc, config/c_rawfuncs.txt); the C below is the
 * near-match the PC build uses (37 lines off, tools/align.py). */
#ifdef __MWERKS__
static asm int sphr_face_o4(HSWEEP *sw, HPOLY *pl, f32 *pos)
{
#include "sphr_face_o4.inc"
}
#else
static int sphr_face_o4(HSWEEP *sw, HPOLY *pl, f32 *pos) {
    HV3 q;
    f32 e[3];
    HV3 d;
    HV3 best;
    f32 c[3];
    HV3 vt[4];
    HPOLY *hd;
    f32 t;
    f32 dist;
    int r;
    s8 side;
    HV3 *v;
    int i;
    int r2;
    int i2;

    if (sw->w22 != 0) {
        t = pos[1] - sw->r;
        if (!(t <= pl->v[0][1]) && !(t <= pl->v[1][1]) && !(t <= pl->v[2][1])) return 0;
    }
    PointToPoint(e, sw->p1, pl->v[0]);
    if ((s8)!(flvecInnerProduct(e, pl->n) > 0.0f)) return 0;
    PointToPoint(e, sw->p0, pl->v[0]);
    t = flvecInnerProduct(e, pl->n);
    if (!(t <= 0.0f)) {
        side = 0;
    } else {
        side = 1;
    }
    d = *(HV3 *)sw->p0;
    if (flAbs(t) < sw->r) {
        ScaleVector(&q.x, pl->n, t);
        PointToPoint(&q.x, sw->p0, &q.x);
    } else {
        if (!FaceLinePos(sw->p0, pl->n, pl->v[0], &q.x)) return 0;
        side |= 2;
    }
    if (NormalClipFace(&q.x, pl->v[0], pl->n)) {
        if (hit_poly_num >= 19) return 0;
        hd = &hit_decision[hit_poly_num];
        *hd = *pl;
        NvecFloatAdjust(hd->n, hd->n, hd);
        *(HV3 *)hit_hosei_base[hit_poly_num] = d;
        hit_side[hit_poly_num] = side;
        hit_area_out[hit_poly_num] = 0;
        *(HV3 *)hit_near_point[hit_poly_num] = q;
        hit_near_point[hit_poly_num][1] = d.y;
        hit_poly_num++;
        return 1;
    }
    dist = 8000.0f;
    hit_area_out[hit_poly_num] = 1;
    v = vt;
    vt[0] = *(HV3 *)pl->v[0];
    vt[1] = *(HV3 *)pl->v[1];
    vt[2] = *(HV3 *)pl->v[2];
    vt[3] = *(HV3 *)pl->v[0];
    SetVector(c, q.x, q.y, q.z);
    if (!(side & 2)) {
        for (i = 0; i < 3; i++, v++) {
            r = 0;
            PointToPoint(e, sw->p0, &v->x);
            PointToPoint(&d.x, &v[1].x, &v->x);
            flvecNormalize(&d.x);
            if (flvecInnerProduct(e, &d.x) < 0.0f) r |= 1;
            PointToPoint(e, sw->p0, &v[1].x);
            t = flvecInnerProduct(e, &d.x);
            if (!(t <= 0.0f)) r |= 2;
            switch (r) {
            case 0:
                ScaleVector(&q.x, &d.x, t);
                AddVector(&q.x, &q.x, &v[1].x);
                break;
            case 1:
                q = v[0];
                break;
            case 2:
                q = v[1];
                break;
            case 3:
                return 0;
            }
            t = flvecCalcDistance(sw->p0, &q.x);
            if (t < dist) {
                dist = t;
                best = q;
            }
        }
    } else {
        for (i2 = 0; i2 < 3; i2++, v++) {
            r2 = 0;
            PointToPoint(e, c, &v->x);
            PointToPoint(&d.x, &v[1].x, &v->x);
            flvecNormalize(&d.x);
            if (flvecInnerProduct(e, &d.x) < 0.0f) r2 |= 1;
            PointToPoint(e, c, &v[1].x);
            t = flvecInnerProduct(e, &d.x);
            if (!(t <= 0.0f)) r2 |= 2;
            switch (r2) {
            case 0:
                ScaleVector(&q.x, &d.x, t);
                AddVector(&q.x, &q.x, &v[1].x);
                break;
            case 1:
                q = v[0];
                break;
            case 2:
                q = v[1];
                break;
            case 3:
                return 0;
            }
            t = flvecCalcDistance(c, &q.x);
            if (t < dist) {
                dist = t;
                best = q;
            }
        }
    }
    if (dist < sw->r) {
        if (hit_poly_num >= 19) return 0;
        hd = &hit_decision[hit_poly_num];
        *hd = *pl;
        NvecFloatAdjust(hd->n, hd->n, hd);
        *(HV3 *)hit_hosei_base[hit_poly_num] = *(HV3 *)sw->p0;
        hit_side[hit_poly_num] = side;
        *(HV3 *)hit_near_point[hit_poly_num] = best;
        *(HV3 *)hit_kouten[hit_poly_num] = *(HV3 *)c;
        hit_near_point[hit_poly_num][1] = hit_kouten[hit_poly_num][1] = hit_hosei_base[hit_poly_num][1];
        hit_poly_num++;
        return 1;
    }
    return 0;
}
#endif

/* 0x00117970: where does the edge e0-e1 reach plane (normal n) near p?
 * Returns 0 when both ends are on the same side, 1 / 2 and the crossing
 * point in out otherwise. */
static int FaceLinePos(f32 *e, f32 *n, f32 *p, f32 *out) {
    f32 v[4];
    f32 d[6];
    f32 a, b, t;
    int r;

    PointToPoint(d, e + 3, e);
    PointToPoint((d + 3), e, e + 3);
    r = 0;
    PointToPoint(v, e, p);
    if (!((a = flvecInnerProduct(v, n)) < 0.0f)) r |= 1;
    PointToPoint(v, e + 3, p);
    if (!((b = flvecInnerProduct(v, n)) < 0.0f)) r |= 2;
    switch (r) {
    case 0:
    case 3:
        r = 0;
        break;
    case 1:
        t = flvecInnerProduct((d + 3), n);
        if (t == 0.0f || t == -0.0f) {
            ScaleVector(v, n, a);
        } else {
            ScaleVector(v, (d + 3), a / t);
        }
        PointToPoint(out, e, v);
        break;
    case 2:
        t = flvecInnerProduct(d, n);
        if (t == 0.0f || t == -0.0f) {
            ScaleVector(v, n, b);
        } else {
            ScaleVector(v, d, b / t);
        }
        PointToPoint(out, e + 3, v);
        break;
    }
    return r;
}

/* 0x00117BA0: is point p inside the triangle seen from direction n (all three
 * edge normals on the same side)? */
int NormalClipFace(f32 *p, f32 *tri, f32 *n) {
    f32 e[3];

    UnitNormalVectorCCW(tri, tri + 3, p, e);
    if (flvecInnerProduct(e, n) < 0.0f) return 0;
    UnitNormalVectorCCW(tri + 3, tri + 6, p, e);
    if (flvecInnerProduct(e, n) < 0.0f) return 0;
    UnitNormalVectorCCW(tri + 6, tri, p, e);
    if (flvecInnerProduct(e, n) < 0.0f) return 0;
    return 1;
}

/* Edge-sharing test: do polygons pa and pb share two vertices? If so
 * *odd = the vertex of pb that is not shared; returns 1, else 0. */
static inline int share_edge(HPOLY *pa, HPOLY *pb, f32 *odd) {
    u8 idx[4];
    int n = 0;
    int i, j, k;
    int mask;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (pa->v[i][0] == pb->v[j][0] && pa->v[i][1] == pb->v[j][1] && pa->v[i][2] == pb->v[j][2]) {
                idx[n++] = j;
                break;
            }
        }
    }
    if (n < 2) return 0;
    mask = (1 << idx[0]) | (1 << idx[1]);
    for (k = 0; k < 3; k++) {
        if (!(mask & (1 << k))) break;
    }
    if (k == 3) return 1;           /* all shared: nothing to measure */
    SetVector(odd, pb->v[k][0], pb->v[k][1], pb->v[k][2]);
    return 2;
}

/* PushAdjust3: original bytes kept (build/raw/PushAdjust3.inc, config/c_rawfuncs.txt); the C below is the
 * near-match the PC build uses (1027 lines off, tools/align.py). */
#ifdef __MWERKS__
static asm int PushAdjust3(HSWEEP *sw, f32 *pos, u8 *flagp)
{
#include "PushAdjust3.inc"
}
#else
static int PushAdjust3(HSWEEP *sw, f32 *pos, u8 *flagp) {
    s8 A[0x20];         /* face contacts (hit_area_out == 0) */
    s8 B[0x20];         /* edge contacts */
    s8 cov[0x20];       /* edge contacts that are covered */
    s8 why[0x20];       /* why: 1 same normal, 2 behind a face, 3 same point */
    f32 v[3];
    f32 e[3];
    f32 c[3];
    f32 d[3];
    f32 box[12];
    f32 L;
    int nA = 0; /* A[]/B[] zeroed below: the original reads A[k] past nA (stack garbage) in the cover pass */
    int nB = 0;
    int nd = 0;
    int i, j, k;
    int ia, ib;
    int r;
    HPOLY *pa;
    HPOLY *pb;
    f32 t;

    for (i = 0; i < 0x20; i++) {
        A[i] = 0;
        B[i] = 0;
    }
    switch (hit_poly_num) {
    case 0:
        break;
    case 1:
        if (hosei_sub(sw, v, 0, flagp) != 0) {
            SetVector(e, v[0], v[1], v[2]);
            L = flvecCalcLength(e);
            if (sw->len != 0.0f) {
                t = 1.8f * sw->len;
                if (!(L <= t)) L = t;
            }
            flvecNormalize(e);
            ScaleVector(v, e, L);
            pos[0] += v[0];
            pos[1] += v[1];
            pos[2] += v[2];
            hited_wall_no[hited_poly_num] = 0;
            hited_poly_num++;
        }
        break;
    default:
        for (i = 0; i < hit_poly_num; i++) {
            if (hit_area_out[i] == 0) {
                A[nA++] = i;
            } else {
                B[nB++] = i;
            }
        }
        if (nA != 0 && nB != 0) {
            for (ia = 0; ia < nA; ia++) {
                for (ib = 0; ib < nB; ib++) {
                    /* (the original scans cov[] for B[ib] here and ignores the result) */
                    pa = &hit_decision[B[ib]];
                    pb = &hit_decision[A[ia]];
                    if (pb->n[0] == pa->n[0] && pb->n[1] == pa->n[1] && pb->n[2] == pa->n[2]) {
                        why[nd] = 1;
                        cov[nd] = B[ib];
                        nd++;
                    } else if (pb->n[1] != 1.0f && pb->n[1] != -1.0f && pa->n[1] != 1.0f && pa->n[1] != -1.0f) {
                        r = share_edge(pb, pa, c);
                        if (r == 2) {
                            PointToPoint(d, c, hit_decision[A[ia]].v[0]);
                            if (flvecInnerProduct(d, hit_decision[A[ia]].n) <= 0.0f) {   /* behind the face (the asm records when the product is <= 0) */
                                why[nd] = 2;
                                cov[nd] = B[ib];
                                nd++;
                            }
                        }
                    }
                }
            }
        }
        if (nB >= 2) {
            for (i = 0; i < nB - 1; i++) {
                for (k = 0, r = 0; k < nd; k++) {
                    if (B[i] == cov[k]) r = 1;
                }
                if (r != 1) {
                    for (j = 1; j < nB; j++) {
                        for (k = 0, r = 0; k < nd; k++) {
                            if (B[j] == cov[k]) r = 1;
                        }
                        if (r != 1 && B[i] != B[j]) {
                            if (hit_near_point[B[i]][0] == hit_near_point[B[j]][0] &&
                                hit_near_point[B[i]][1] == hit_near_point[B[j]][1] &&
                                hit_near_point[B[i]][2] == hit_near_point[B[j]][2]) {
                                why[nd] = 3;
                                cov[nd] = B[j];
                                nd++;
                            }
                        }
                    }
                }
            }
        }
        if (nd > 0 && nd < nB) {
            for (k = 0; k < nd; k++) {    /* nd grows inside: the bound is re-read */
                if (why[k] == 1) {
                    for (j = 0; j < nB; j++) {
                        if (B[j] != cov[k]) {
                            pa = &hit_decision[cov[k]];
                            pb = &hit_decision[B[j]];
                            /* the original pairs covered entry k with A[k] here (index kept as written) */
                            if (pa->n[1] != 1.0f && pa->n[1] != -1.0f && pb->n[1] != 1.0f && pb->n[1] != -1.0f) {
                                r = share_edge(pa, pb, c);
                                if (r == 2) {
                                    PointToPoint(d, c, hit_decision[A[k]].v[0]);
                                    if (flvecInnerProduct(d, hit_decision[A[k]].n) <= 0.0f) {   /* behind the face */
                                        why[nd] = 2;
                                        cov[nd] = B[j];
                                        nd++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        for (i = 0; i < 12; i++) box[i] = 0.0f;
        for (i = 0; i < nA; i++) {
            if (hosei_sub(sw, v, A[i], flagp) != 0) {
                add_vec_sub2(v, box, sw->r);
                hited_wall_no[hited_poly_num] = A[i];
                hited_poly_num++;
            }
        }
        for (i = 0; i < nB; i++) {
            r = 0;
            for (k = 0; k < nd; k++) {
                if (cov[k] == B[i]) r = 1;
            }
            if (r == 0 && hosei_sub(sw, v, B[i], flagp) != 0) {
                add_vec_sub2(v, box, sw->r);
                hited_wall_no[hited_poly_num] = B[i];
                hited_poly_num++;
            }
        }
        for (k = 0; k < 3; k++) {
            if (box[k] != 0.0f && box[3 + k] != 0.0f) {
                v[k] = box[9 + k] + (box[6 + k] + ((box[k] + box[3 + k]) / 2.0f));
            } else if (box[3 + k] == 0.0f) {
                v[k] = box[k] + box[6 + k];
            } else {
                v[k] = box[3 + k] + box[9 + k];
            }
        }
        SetVector(e, v[0], v[1], v[2]);
        L = flvecCalcLength(e);
        if (sw->len != 0.0f) {
            t = 1.8f * sw->len;
            if (!(L <= t)) L = t;
        }
        flvecNormalize(e);
        ScaleVector(v, e, L);
        pos[0] += v[0];
        pos[1] += v[1];
        pos[2] += v[2];
        break;
    }
    return hit_poly_num;
}
#endif

static int hosei_sub(HSWEEP *sw, f32 *out, s8 idx, u8 *flagp) {
    f32 e[4];
    f32 b[4];
    f32 a[4];
    f32 c[4];
    f32 len;
    s8 h;

    switch (hit_area_out[idx]) {
    default:
        SetVector(e, hit_decision[idx].n[0], hit_decision[idx].n[1], hit_decision[idx].n[2]);
        h = 0;
        if (sw->w20 == 1) {
            if (!(e[0] == 0.0f && e[1] != 0.0f && e[2] == 0.0f)) {
                h = 1;
                e[1] = 0.0f;
                flvecNormalize(e);
            }
        }
        ScaleVector(b, e, sw->r);
        AddVector(a, hit_near_point[idx], b);
        if (h) a[1] = hit_hosei_base[idx][1];
        PointToPoint(b, a, hit_hosei_base[idx]);
        len = flvecCalcLength(b);
        flvecNormalize(b);
        ScaleVector(a, b, len);
        break;
    case 1:
        switch (hit_side[idx]) {
        case 0:
            PointToPoint(a, hit_hosei_base[idx], hit_near_point[idx]);
            break;
        case 1:
        default:
            PointToPoint(a, hit_kouten[idx], hit_near_point[idx]);
            break;
        }
        h = 0;
        if (sw->w20 == 1) {
            if (!(a[0] == 0.0f && a[1] != 0.0f && a[2] == 0.0f)) {
                a[1] = 0.0f;
                h = 1;
            }
        }
        flvecNormalize(a);
        ScaleVector(b, a, sw->r);
        AddVector(a, hit_near_point[idx], b);
        SetVector(c, hit_hosei_base[idx][0], hit_hosei_base[idx][1], hit_hosei_base[idx][2]);
        if (h == 1) c[1] = a[1];
        PointToPoint(b, a, c);
        len = flvecCalcLength(b);
        if (len == 0.0f) return 0;
        flvecNormalize(b);
        ScaleVector(a, b, len);
        break;
    }
    if (hit_side[idx] == 1) *flagp = 1;
    SetVector(out, a[0], a[1], a[2]);
    return 1;
}

/* 0x00119010: push box v (size 6 floats: min x,y,z / max x,y,z, then extents)
 * outwards by v with limit r. Guess from the code. */
static void add_vec_sub2(f32 *v, f32 *b, f32 r) {
    f32 x;

    x = v[0];
    if (!(x < 0.0f)) {
        if (b[0] + b[6] < x) {
            if (x <= r) {
                b[0] = x;
            } else {
                b[0] = r;
                b[6] = v[0] - r;
            }
        }
    } else if (!(b[3] + b[9] <= x)) {
        if (-x <= r) {
            b[3] = x;
        } else {
            b[3] = -r;
            b[9] = v[0] + r;
        }
    }
    x = v[1];
    if (!(x < 0.0f)) {
        if (b[1] + b[7] < x) {
            if (x <= r) {
                b[1] = x;
            } else {
                b[1] = r;
                b[7] = v[1] - r;
            }
        }
    } else if (!(b[4] + b[10] <= x)) {
        if (-x <= r) {
            b[4] = x;
        } else {
            b[4] = -r;
            b[10] = v[1] + r;
        }
    }
    x = v[2];
    if (!(x < 0.0f)) {
        if (b[2] + b[8] < x) {
            if (x <= r) {
                b[2] = x;
                return;
            }
            b[2] = r;
            b[8] = v[2] - r;
        }
    } else if (!(b[5] + b[11] <= x)) {
        if (-x <= r) {
            b[5] = x;
            return;
        }
        b[5] = -r;
        b[11] = v[2] + r;
    }
}

f32 GetGroundHit(f32 *pos) {
    f32 h[8];
    HV3 tri[3];
    f32 hf[8];
    HV3 pt;
    HPOLY **cell;
    int k;
    s8 n = 0;
    HPOLY *pl;
    s8 any;
    s8 i;
    s8 c;
    f32 r;

    h[0] = pos[1];
    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (k = 0; (s32)cell[k] != -1; k++) {
            pl = cell[k];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && n < 5) {
                h[n++] = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
            }
        }
    }
    if (n != 1) {
        any = 0;
        c = 0;
        for (i = 0; i < n; i++) {
            if (h[i] <= 50.0f + pos[1]) {
                any = 1;
                hf[c++] = h[i];
            }
        }
        if (any == 1) {
            if (c == 1) {
                r = hf[0];
            } else {
                for (i = 0; i < c; i++) {
                    if (i == 0) {
                        h[0] = hf[i];
                    } else if (h[0] < hf[i]) {
                        h[0] = hf[i];
                    }
                }
                r = h[0];
            }
        } else {
            for (i = 0; i < n; i++) {
                if (i == 0) {
                    hf[0] = h[i];
                } else if (!(hf[0] <= h[i])) {
                    hf[0] = h[i];
                }
            }
            r = hf[0];
        }
    } else {
        r = h[0];
    }
    if (r == -0.0f) r = 0.0f;
    return r;
}

f32 GetGroundShellHit(f32 *pos) {
    f32 h[8];
    HV3 tri[3];
    f32 hf[8];
    HV3 pt;
    HPOLY **cell;
    int k;
    s8 n = 0;
    HPOLY *pl;
    s8 any;
    s8 i;
    s8 c;
    f32 r;

    h[0] = pos[1];
    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (k = 0; (s32)cell[k] != -1; k++) {
            pl = cell[k];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && n < 5) {
                HKIND *kd;
                int w, x;
                f32 v = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                kd = &ground_tbl_add[game_w.stage][pl->kind];
                x = kd->x0D;
                w = kd->water;
                if (w == 0 && x == 0) {
                    h[n++] = v;
                }
            }
        }
    }
    if (n != 1) {
        any = 0;
        c = 0;
        for (i = 0; i < n; i++) {
            if (h[i] <= 100.0f + pos[1]) {
                any = 1;
                hf[c++] = h[i];
            }
        }
        if (any == 1) {
            if (c == 1) {
                r = hf[0];
            } else {
                for (i = 0; i < c; i++) {
                    if (i == 0) {
                        h[0] = hf[i];
                    } else if (h[0] < hf[i]) {
                        h[0] = hf[i];
                    }
                }
                r = h[0];
            }
        } else {
            for (i = 0; i < n; i++) {
                if (i == 0) {
                    hf[0] = h[i];
                } else if (!(hf[0] <= h[i])) {
                    hf[0] = h[i];
                }
            }
            r = hf[0];
        }
    } else {
        r = h[0];
    }
    if (r == -0.0f) r = 0.0f;
    return r;
}

/* 0x001198B0 */
int GetGroundHitArea(HENT *ent, f32 *pos, f32 *out) {
    f32 h[8];
    HV3 tri[3];
    f32 hf[8];
    HV3 pt;
    HPOLY **cell;
    int k;
    s8 n = 0;
    HPOLY *pl;
    s8 any;
    s8 i;
    s8 c;

    if (!(Pl_stg_ck(ent) & 0xFF)) {
        *out = Stage_data_get(ent->stg)->floor;
        return -1;
    }
    h[0] = pos[1];
    ent->gkind = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (k = 0; (s32)cell[k] != -1; k++) {
            pl = cell[k];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && n < 5) {
                HKIND *kd;
                int w, x;
                f32 v = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                kd = &ground_tbl_add[ent->stg][pl->kind];
                w = kd->water;
                x = kd->x0E;
                if (x != 0) ent->gkind = x;
                if (w == 0) {
                    h[n++] = v;
                }
            }
        }
    } else {
        *out = ((STAGE_SZ *)stage_work.x48)->floor2;
        return 0;
    }
    if (n != 1) {
        any = 0;
        c = 0;
        for (i = 0; i < n; i++) {
            if (h[i] <= 50.0f + pos[1]) {
                any = 1;
                hf[c++] = h[i];
            }
        }
        if (any == 1) {
            if (c == 1) {
                *out = hf[0];
            } else {
                for (i = 0; i < c; i++) {
                    if (i == 0) {
                        h[0] = hf[i];
                    } else if (h[0] < hf[i]) {
                        h[0] = hf[i];
                    }
                }
                *out = h[0];
            }
        } else {
            for (i = 0; i < n; i++) {
                if (i == 0) {
                    hf[0] = h[i];
                } else if (!(hf[0] <= h[i])) {
                    hf[0] = h[i];
                }
            }
            *out = hf[0];
        }
    } else {
        *out = h[0];
    }
    if (*out == -0.0f) *out = 0.0f;
    return 1;
}

/* 0x00119CA0: like GetGroundHitArea but looks for the ground ABOVE y (the
 * lowest height above it, else the highest). */
int GetGroundHitAreaUpper(HENT *ent, f32 *pos, f32 *out) {
    f32 h[8];
    HV3 tri[3];
    f32 hf[8];
    HV3 pt;
    HPOLY **cell;
    int k;
    s8 n = 0;
    HPOLY *pl;
    s8 any;
    s8 i;
    s8 c;

    if (!(Pl_stg_ck(ent) & 0xFF)) {
        *out = Stage_data_get(ent->stg)->floor;
        return -1;
    }
    h[0] = pos[1];
    ent->gkind = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (k = 0; (s32)cell[k] != -1; k++) {
            pl = cell[k];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && n < 5) {
                HKIND *kd;
                int w, x;
                f32 v = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                kd = &ground_tbl_add[ent->stg][pl->kind];
                w = kd->water;
                x = kd->x0E;
                if (x != 0) ent->gkind = x;
                if (w == 0) {
                    h[n++] = v;
                }
            }
        }
    } else {
        *out = ((STAGE_SZ *)stage_work.x48)->floor2;
        return 0;
    }
    if (n != 1) {
        any = 0;
        c = 0;
        for (i = 0; i < n; i++) {
            if (!(h[i] <= pos[1])) {
                any = 1;
                hf[c++] = h[i];
            }
        }
        if (any == 0) {
            for (i = 0; i < n; i++) {
                if (i == 0) {
                    hf[0] = h[i];
                } else if (hf[0] < h[i]) {
                    hf[0] = h[i];
                }
            }
            *out = hf[0];
        } else if (c == 1) {
            *out = hf[0];
        } else {
            for (i = 0; i < c; i++) {
                if (i == 0) {
                    h[0] = hf[i];
                } else if (!(h[0] <= hf[i])) {
                    h[0] = hf[i];
                }
            }
            *out = h[0];
        }
    } else {
        *out = h[0];
    }
    if (*out == -0.0f) *out = 0.0f;
    return 1;
}

/* 0x0011A080: player version: also the polygon attribute (GATTR) of the
 * chosen height; *flag is cleared when off the ground. */
int GetGroundHitStatusAreaPl(HENT *ent, f32 *pos, GATTR *at, f32 *out, f32 *flag) {
    GATTR a[8];
    f32 h[8];
    HV3 tri[3];
    f32 hf[8];
    GATTR af[8];
    HV3 pt;
    HPOLY **cell;
    int k;
    s8 n = 0;
    HPOLY *pl;
    s8 any;
    s8 i;
    s8 c;

    ent->x7E8 = 0;
    if (!(Pl_stg_ck(ent) & 0xFF)) {
        *out = Stage_data_get(ent->stg)->floor;
        *flag = 0.0f;
        at->kind = 0;
        at->b1 = 0;
        at->h2 = 0;
        return -1;
    }
    h[0] = pos[1];
    a[0].kind = ent->gat.kind;
    a[0].b1 = ent->gat.b1;
    a[0].h2 = ent->gat.h2;
    ent->gkind = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (k = 0; (s32)cell[k] != -1; k++) {
            pl = cell[k];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && n < 5) {
                f32 v = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                if (ground_tbl_add[ent->stg][pl->kind].water == 0) {
                    h[n] = v;
                    a[n].kind = pl->kind;
                    a[n].b1 = pl->b1;
                    a[n].h2 = pl->h2;
                    n++;
                }
            }
        }
    } else {
        *out = ((STAGE_SZ *)stage_work.x48)->floor2;
        *flag = 0.0f;
        at->kind = 0;
        at->b1 = 0;
        at->h2 = 0;
        return 0;
    }
    if (n != 1) {
        any = 0;
        c = 0;
        for (i = 0; i < n; i++) {
            if (h[i] <= 50.0f + pos[1]) {
                any = 1;
                af[c] = a[i];
                hf[c++] = h[i];
            }
        }
        if (any == 1) {
            if (c == 1) {
                *out = hf[0];
                at->kind = af[0].kind;
                at->b1 = af[0].b1;
                at->h2 = af[0].h2;
            } else {
                for (i = 0; i < c; i++) {
                    if (i == 0) {
                        h[0] = hf[i];
                        a[0].kind = af[i].kind;
                        a[0].b1 = af[i].b1;
                        a[0].h2 = af[i].h2;
                    } else if (h[0] < hf[i]) {
                        h[0] = hf[i];
                        a[0].kind = af[i].kind;
                        a[0].b1 = af[i].b1;
                        a[0].h2 = af[i].h2;
                    }
                }
                *out = h[0];
                at->kind = a[0].kind;
                at->b1 = a[0].b1;
                at->h2 = a[0].h2;
            }
        } else {
            for (i = 0; i < n; i++) {
                if (i == 0) {
                    hf[0] = h[i];
                    af[0].kind = a[i].kind;
                    af[0].b1 = a[i].b1;
                    af[0].h2 = a[i].h2;
                } else if (!(hf[0] <= h[i])) {
                    hf[0] = h[i];
                    af[0].kind = a[i].kind;
                    af[0].b1 = a[i].b1;
                    af[0].h2 = a[i].h2;
                }
            }
            *out = hf[0];
            at->kind = af[0].kind;
            at->b1 = af[0].b1;
            at->h2 = af[0].h2;
        }
    } else {
        *out = h[0];
        at->kind = a[0].kind;
        at->b1 = a[0].b1;
        at->h2 = a[0].h2;
    }
    if (*out == -0.0f) *out = 0.0f;
    if (ent->x604 != 0) at->b1 = 2;
    return 1;
}

/* 0x0011A600: monster version. A kind with x0E set marks a special surface
 * (ent+0x7E9 = its value, attribute kept in sp), a kind with water set
 * gives the water height in *flag (relative to ent+0x7E0) and attribute. */
int GetGroundHitStatusAreaEm(HENT *ent, f32 *pos, GATTR *at, f32 *out, f32 *flag) {
    GATTR a[8];
    f32 h[8];
    HV3 tri[3];
    f32 hf[8];
    GATTR af[8];
    HV3 pt;
    HPOLY **cell;
    int k;
    s8 n = 0;
    HPOLY *pl;
    s8 any;
    s8 i;
    s8 c;
    u8 w;
    u8 inw;
    u8 sk_kind;
    u8 sk_b1;
    u16 sk_h2;
    u8 wt_kind;
    u8 wt_b1;
    u16 wt_h2;
    STAGE_SZ *sd;

    ent->x7E8 = 0;
    w = 0;
    if (!(Pl_stg_ck(ent) & 0xFF)) {
        sd = Stage_data_get(ent->stg);
        if (ent->mon != 0 && ent->kind == 21) {
            *out = sd->floor2;
        } else {
            *out = sd->floor;
        }
        *flag = 0.0f;
        at->kind = 0;
        at->b1 = 0;
        at->h2 = 0;
        return -1;
    }
    h[0] = pos[1];
    a[0].kind = ent->gat.kind;
    a[0].b1 = ent->gat.b1;
    a[0].h2 = ent->gat.h2;
    ent->gkind = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (k = 0; (s32)cell[k] != -1; k++) {
            pl = cell[k];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && n < 5) {
                HKIND *kd;
                int wa, x;
                f32 v = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                kd = &ground_tbl_add[ent->stg][pl->kind];
                wa = kd->water;
                x = kd->x0E;
                if (x != 0) {
                    ent->gkind = x;
                    *flag = v;
                    sk_kind = pl->kind;
                    sk_b1 = pl->b1;
                    sk_h2 = pl->h2;
                }
                if (wa != 0) {
                    w = 1;
                    v -= ent->x7E0;
                    *flag = v;
                    wt_kind = pl->kind;
                    wt_b1 = pl->b1;
                    wt_h2 = pl->h2;
                } else {
                    h[n] = v;
                    a[n].kind = pl->kind;
                    a[n].b1 = pl->b1;
                    a[n].h2 = pl->h2;
                    n++;
                }
            }
        }
    } else {
        *out = ((STAGE_SZ *)stage_work.x48)->floor2;
        *flag = 0.0f;
        at->kind = 0;
        at->b1 = 0;
        at->h2 = 0;
        return 0;
    }
    if (n != 1) {
        any = 0;
        c = 0;
        for (i = 0; i < n; i++) {
            if (ent->gkind != 0) {
                if (h[i] <= 100.0f + pos[1]) {
                    any = 1;
                    af[c] = a[i];
                    hf[c++] = h[i];
                }
            } else if (h[i] <= 50.0f + pos[1]) {
                any = 1;
                af[c] = a[i];
                hf[c++] = h[i];
            }
        }
        if (any == 1) {
            if (c == 1) {
                *out = hf[0];
                at->kind = af[0].kind;
                at->b1 = af[0].b1;
                at->h2 = af[0].h2;
            } else {
                for (i = 0; i < c; i++) {
                    if (i == 0) {
                        h[0] = hf[i];
                        a[0].kind = af[i].kind;
                        a[0].b1 = af[i].b1;
                        a[0].h2 = af[i].h2;
                    } else if (h[0] < hf[i]) {
                        h[0] = hf[i];
                        a[0].kind = af[i].kind;
                        a[0].b1 = af[i].b1;
                        a[0].h2 = af[i].h2;
                    }
                }
                *out = h[0];
                at->kind = a[0].kind;
                at->b1 = a[0].b1;
                at->h2 = a[0].h2;
            }
        } else {
            for (i = 0; i < n; i++) {
                if (i == 0) {
                    hf[0] = h[i];
                    af[0].kind = a[i].kind;
                    af[0].b1 = a[i].b1;
                    af[0].h2 = a[i].h2;
                } else if (!(hf[0] <= h[i])) {
                    hf[0] = h[i];
                    af[0].kind = a[i].kind;
                    af[0].b1 = a[i].b1;
                    af[0].h2 = a[i].h2;
                }
            }
            *out = hf[0];
            at->kind = af[0].kind;
            at->b1 = af[0].b1;
            at->h2 = af[0].h2;
        }
    } else {
        *out = h[0];
        at->kind = a[0].kind;
        at->b1 = a[0].b1;
        at->h2 = a[0].h2;
    }
    inw = 0;
    if (w) {
        if (*out < *flag) {
            inw = 1;
            at->kind = wt_kind;
            at->b1 = wt_b1;
            at->h2 = wt_h2;
        }
    }
    if (ent->gkind != 0) {
        at->kind = sk_kind;
        at->b1 = sk_b1;
        at->h2 = sk_h2;
    }
    ent->x7E8 = inw;
    if (*out == -0.0f) *out = 0.0f;
    return 1;
}

u8 GetWaterHit(f32 *pos, f32 *out) {
    HV3 tri[3];
    HV3 pt;
    u8 r = 0;
    HPOLY *pl;
    HPOLY **cell;
    int i;
    f32 h;

    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (i = 0; (s32)cell[i] != -1; i++) {
            pl = cell[i];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1) {
                h = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                if (ground_tbl_add[game_w.stage][pl->kind].water != 0) {
                    *out = h;
                    r = 1;
                }
            }
        }
    }
    if (*out == -0.0f) *out = 0.0f;
    return r;
}

u8 GetTenjoHit(f32 *pos, f32 *out, HPOLY *attr) {
    HV3 tri[3];
    HV3 pt;
    u8 r = 0;
    HPOLY *pl;
    HPOLY **cell;
    int i;

    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (i = 0; (s32)cell[i] != -1; i++) {
            pl = cell[i];
            pt.x = pos[0];
            pt.y = pos[2];
            tri[0].x = pl->v[0][0];
            tri[0].y = pl->v[0][2];
            tri[1].x = pl->v[1][0];
            tri[1].y = pl->v[1][2];
            tri[2].x = pl->v[2][0];
            tri[2].y = pl->v[2][2];
            if (pl->n[1] < 0.0f && PointHitCheckF3(&tri[0].x, &pt.x) == 1) {
                r = 1;
                *out = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                attr->kind = pl->kind;
                attr->b1 = pl->b1;
                attr->h2 = pl->h2;
            }
        }
    }
    if (*out == -0.0f) *out = 0.0f;
    return r;
}

u8 GetYouganHit(f32 *pos) {
    HV3 tri[3];
    HV3 pt;
    u8 r = 0;
    HPOLY *pl;
    HPOLY **cell;
    int i;

    if (GroundFieldInCheck(pos) == 1) {
        cell = (HPOLY **)GetGroundTblAdrs(pos);
        for (i = 0; (s32)cell[i] != -1; i++) {
            {
                pl = cell[i];
                pt.x = pos[0];
                pt.y = pos[2];
                tri[0].x = pl->v[0][0];
                tri[0].y = pl->v[0][2];
                tri[1].x = pl->v[1][0];
                tri[1].y = pl->v[1][2];
                tri[2].x = pl->v[2][0];
                tri[2].y = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && ground_tbl_add[game_w.stage][pl->kind].lava != 0) {
                    r = 1;
                }
            }
        }
    }
    return r;
}

s32 *GetGroundTblAdrs(f32 *p) {
    f32 csz = (u32)diorama_w.gcsz;
    f32 csx = (u32)diorama_w.gcsx;
    int iz = (int)(p[2] / csz);
    int ix = (int)(p[0] / csx);

    return (s32 *)diorama_w.gtbl[iz + ix * diorama_w.gnz];
}

/* GetWallHitLine: original bytes kept (build/raw/GetWallHitLine.inc, config/c_rawfuncs.txt); the C below is the
 * near-match the PC build uses (124 lines off, tools/align.py). */
#ifdef __MWERKS__
asm int GetWallHitLine(f32 *a, f32 *b, f32 *out, u16 mask)
{
#include "GetWallHitLine.inc"
}
#else
int GetWallHitLine(f32 *a, f32 *b, f32 *out, u16 mask) {
    HV3 hp;
    f32 d[3];
    HV3 dir;
    HV3 cen;
    HV3 q;
    f32 e[6];
    HPOLY *seen[20];
    HPOLY **cell;
    HPOLY *pl;
    f32 best;
    f32 margin;
    f32 t;
    int cx;
    int cz;
    int nx;
    int nz;
    int sx;
    int sz;
    int i;
    int j;
    int ix;
    int iz;
    int ns;
    int hit;
    int k;

    hit = 0;
    best = flvecCalcDistance(a, b);
    if (!WallFieldInCheck(a)) {
        SetVector(out, a[0], a[1], a[2]);
        return 1;
    }
    if (!WallFieldInCheck(b)) {
        SetVector(out, b[0], b[1], b[2]);
        return 1;
    }
    cx = (int)(a[0] / (f32)diorama_w.gcsx);
    cz = (int)(a[2] / (f32)diorama_w.gcsz);
    nx = (int)(b[0] / (f32)diorama_w.gcsx) - cx;
    nz = (int)(b[2] / (f32)diorama_w.gcsz) - cz;
    if (nx < 0) {
        sx = -1;
        nx = 1 - nx;
    } else {
        sx = 1;
        nx = nx + 1;
    }
    if (nz < 0) {
        sz = -1;
        nz = 1 - nz;
    } else {
        sz = 1;
        nz = nz + 1;
    }
    SetVector(&e[3], a[0], a[1], a[2]);
    SetVector(e, b[0], b[1], b[2]);
    PointToPoint(d, b, a);
    SetVector(&dir.x, d[0], 0.0f, d[2]);
    flvecNormalize(&dir.x);
    if (dir.x == -0.0f) dir.x = 0.0f;
    if (dir.z == -0.0f) dir.z = 0.0f;
    ns = 0;
    margin = (f32)(diorama_w.gcsz / 10) + ((f32)(diorama_w.gcsx / 10) + flSqrt((f32)diorama_w.gcsx * (f32)diorama_w.gcsx + (f32)(diorama_w.gcsz * diorama_w.gcsz)));
    for (i = 0, ix = 0; i < nx; i++) {
        for (j = 0, iz = 0; j < nz; j++) {
            cen.x = (f32)(diorama_w.gcsx >> 1) + (f32)diorama_w.gcsx * (f32)(cx + ix);
            cen.z = (f32)(diorama_w.gcsz >> 1) + (f32)diorama_w.gcsz * (f32)(cz + iz);
            cen.y = 0.0f;
            if (dir.x == 0.0f && dir.z == 0.0f) {
                t = -dir.x * (a[0] - cen.x) - dir.z * (a[2] - cen.z);
            } else {
                t = (-dir.x * (a[0] - cen.x) - dir.z * (a[2] - cen.z)) / (dir.x * dir.x + dir.z * dir.z);
            }
            q.x = a[0] + dir.x * t;
            q.z = a[2] + dir.z * t;
            q.y = 0.0f;
            if (flvecCalcDistance(&cen.x, &q.x) <= margin) {
                cell = (HPOLY **)GetWallTblAdrs(&cen.x);
                if (cell != (HPOLY **)-1 && (s32)*cell != -1) {
                    do {
                        pl = *cell;
                        if (!(pl->h2 & mask)) {
                            if (ns != 0) {
                                k = 0;
                                if (k < ns) {
                                    if (pl == seen[k]) goto next;
                                }
                            }
                            if (FaceLinePos(e, pl->n, pl->v[0], &hp.x)) {
                                if (!NormalClipFace(&hp.x, pl->v[0], pl->n)) goto next;
                                hit = 1;
                                t = flvecCalcDistance(a, &hp.x);
                                if (!(best <= t)) {
                                    best = t;
                                    SetVector(out, hp.x, hp.y, hp.z);
                                }
                            }
                            if (ns < 20) seen[ns++] = pl;
                            if (hit) return 1;
                        }
                    next:
                        cell++;
                    } while ((s32)*cell != -1);
                }
            }
            iz += sz;
        }
        ix += sx;
    }
    SetVector(out, b[0], b[1], b[2]);
    return 0;
}
#endif

/* GetEyeHitLine: original bytes kept (build/raw/GetEyeHitLine.inc, config/c_rawfuncs.txt); the C below is the
 * near-match the PC build uses (338 lines off, tools/align.py). */
#ifdef __MWERKS__
asm int GetEyeHitLine(void *ent, f32 *a, f32 *b, f32 *out, int mask)
{
#include "GetEyeHitLine.inc"
}
#else
int GetEyeHitLine(void *ent, f32 *a, f32 *b, f32 *out, int mask) {
    HPOLY *seen[20];
    f32 e[6];
    f32 dir[3];
    f32 d[3];
    f32 hp[3];
    f32 cen[3];
    f32 q[3];
    f32 best;
    int cx, cz;
    f32 margin;
    f32 t;
    int nx, nz, sx, sz;
    int ix, iz, i, j, k;
    int hit = 0;
    int ns = 0;
    int found;
    int attr;
    s32 *cp;
    HPOLY *pl;
    HKIND *hk;

    best = flvecCalcDistance(a, b);
    if (GroundFieldInCheck(a) == 0) {
        SetVector(out, a[0], a[1], a[2]);
        return 1;
    }
    if (GroundFieldInCheck(b) == 0) {
        SetVector(out, b[0], b[1], b[2]);
        return 1;
    }
    if (WallFieldInCheck(a) == 0) {
        SetVector(out, a[0], a[1], a[2]);
        return 1;
    }
    if (WallFieldInCheck(b) == 0) {
        SetVector(out, b[0], b[1], b[2]);
        return 1;
    }
    cx = (int)(a[0] / (f32)GCSX);
    cz = (int)(a[2] / (f32)GCSZ);
    nx = (int)(b[0] / (f32)GCSX) - cx;
    nz = (int)(b[2] / (f32)GCSZ) - cz;
    if (nx < 0) {
        sx = -1;
        nx = 1 - nx;
    } else {
        sx = 1;
        nx = nx + 1;
    }
    if (nz < 0) {
        sz = -1;
        nz = 1 - nz;
    } else {
        sz = 1;
        nz = nz + 1;
    }
    SetVector(e + 3, a[0], a[1], a[2]);
    SetVector(e, b[0], b[1], b[2]);
    PointToPoint(d, b, a);
    SetVector(dir, d[0], d[1], d[2]);
    flvecNormalize(dir);
    if (dir[0] == -0.0f) dir[0] = 0.0f;
    if (dir[2] == -0.0f) dir[2] = 0.0f;
    margin = (f32)(int)(GCSZ / 10) + ((f32)(int)(GCSX / 10) + flSqrt((f32)GCSX * (f32)GCSX + (f32)(GCSZ * GCSZ)));
    for (ix = 0, i = 0; ix < nx; ix++, i += sx) {
        for (iz = 0, j = 0; iz < nz; iz++, j += sz) {
            cen[0] = (f32)(GCSX >> 1) + (f32)GCSX * (f32)(cx + i);
            cen[2] = (f32)(GCSZ >> 1) + (f32)GCSZ * (f32)(cz + j);
            cen[1] = 0.0f;
            if (dir[0] == 0.0f && dir[2] == 0.0f) {
                t = (-dir[0] * (a[0] - cen[0])) - (dir[2] * (a[2] - cen[2]));
            } else {
                t = ((-dir[0] * (a[0] - cen[0])) - (dir[2] * (a[2] - cen[2]))) / (dir[0] * dir[0] + dir[2] * dir[2]);
            }
            q[0] = a[0] + dir[0] * t;
            q[2] = a[2] + dir[2] * t;
            q[1] = 0.0f;
            if (flvecCalcDistance(cen, q) <= margin) {
                cp = GetGroundTblAdrs(cen);
                if (cp != (s32 *)-1 && *cp != -1) {
                    do {
                        pl = (HPOLY *)*cp;
#ifdef VERSION_MH2
                        hk = &ground_tbl_add[((HENT *)ent)->stg][pl->kind];     /* MH2: lhu 0x680 */
#else
                        hk = &ground_tbl_add[EF(ent, u8, 0x736)][pl->kind];
#endif
                        attr = hk->water;
                        if (EF(ent, u8, 2) == 8 || EF(ent, u8, 2) == 0x22 || EF(ent, u8, 2) == 0xE || EF(ent, u8, 2) == 0x1A) {
                            attr = (attr | hk->x0E) & 0xFF;
                        }
                        if (attr == 0) {
                            SEEN(pl, ns, seen, found);
                            if (!found) {
                                if (FaceLinePos(e, pl->n, pl->v[0], hp) != 0) {
                                    if (NormalClipFace(hp, pl->v[0], pl->n) != 0) {
                                        hit = 1;
                                        t = flvecCalcDistance(a, hp);
                                        if (!(best <= t)) {
                                            best = t;
                                            SetVector(out, hp[0], hp[1], hp[2]);
                                        }
                                    } else {
                                        goto gnext;
                                    }
                                }
                                if (ns < 0x14) seen[ns++] = pl;
                                if (hit != 0) return 1;
                            }
                        }
                    gnext:
                        cp++;
                    } while (*cp != -1);
                }
                cp = GetWallTblAdrs(cen);
                if (cp != (s32 *)-1 && *cp != -1) {
                    do {
                        pl = (HPOLY *)*cp;
                        if (!(pl->h2 & (mask & 0xFFFF))) {
                            SEEN(pl, ns, seen, found);
                            if (!found) {
                                if (FaceLinePos(e, pl->n, pl->v[0], hp) != 0) {
                                    if (NormalClipFace(hp, pl->v[0], pl->n) != 0) {
                                        hit = 1;
                                        t = flvecCalcDistance(a, hp);
                                        if (!(best <= t)) {
                                            best = t;
                                            SetVector(out, hp[0], hp[1], hp[2]);
                                        }
                                    } else {
                                        goto wnext;
                                    }
                                }
                                if (ns < 0x14) seen[ns++] = pl;
                                if (hit != 0) return 1;
                            }
                        }
                    wnext:
                        cp++;
                    } while (*cp != -1);
                }
            }
        }
    }
    SetVector(out, b[0], b[1], b[2]);
    return 0;
}
#endif

int GetFloorSlide(HENT *ent, f32 *out, int flag) {
    HPOLY *pp[8];
    f32 h[8];
    HV3 tri[3];
    HPOLY *pf[8];
    f32 hf[8];
    HV3 pt;
    s32 ang;
    HPOLY **cell;
    int k;
    s8 n = 0;
    f32 *pos;
    HPOLY *pl;
    s8 any;
    s8 i;
    s8 c;
    int r;
    HPOLY *best;
    f32 hr;

    pos = ent->pos;
    ang = 0;
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;
    if (GroundFieldInCheck(pos) == 0) return -1;
    cell = (HPOLY **)GetGroundTblAdrs(pos);
    for (k = 0; (s32)cell[k] != -1; k++) {
        pl = cell[k];
        pt.x = pos[0];
        pt.y = pos[2];
        tri[0].x = pl->v[0][0];
        tri[0].y = pl->v[0][2];
        tri[1].x = pl->v[1][0];
        tri[1].y = pl->v[1][2];
        tri[2].x = pl->v[2][0];
        tri[2].y = pl->v[2][2];
        if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(&tri[0].x, &pt.x) == 1 && n < 5) {
            h[n] = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
            pp[n] = pl;
            n++;
        }
    }
    if (n) {
    if (n != 1) {
        any = 0;
        c = 0;
        for (i = 0; i < n; i++) {
            if (h[i] <= 50.0f + pos[1]) {
                any = 1;
                hf[c] = h[i];
                pf[c++] = pp[i];
            }
        }
        if (any == 1) {
            if (c == 1) {
                hr = hf[0];
                best = pf[0];
            } else {
                for (i = 0; i < c; i++) {
                    if (i == 0) {
                        h[0] = hf[i];
                        pp[0] = pf[i];
                    } else if (h[0] < hf[i]) {
                        h[0] = hf[i];
                        pp[0] = pf[i];
                    }
                }
                hr = h[0];
                best = pp[0];
            }
        } else {
            for (i = 0; i < n; i++) {
                if (i == 0) {
                    hf[0] = h[i];
                    pf[0] = pp[i];
                } else if (!(hf[0] <= h[i])) {
                    hf[0] = h[i];
                    pf[0] = pp[i];
                }
            }
            hr = hf[0];
            best = pf[0];
        }
    } else {
        best = pp[0];
        hr = h[0];
    }
    if (!(hr < pos[1])) {
        check_angle(best->n, &ang);
        r = check_slide(best, (u8 *)ent, &ang, out, slide_tbl[0]) & 0xFF;
        if (flag != 0 && r != 0) {
            pos[0] += out[0];
            pos[1] += out[1];
            pos[2] += out[2];
        }
        return 0;
    }
    return 1;
    }
    return -1;
}

/* 0x0011C920: slide vector for a polygon pl under entity e (pos at +0xAC):
 * plane distance of the point lowered by ang/10000*scale, written to out. */
static int check_slide(HPOLY *pl, u8 *e, s32 *ang, f32 *out, f32 scale) {
    f32 nn;
    int new_var; /* permuter: matching register choice */
    f32 t;
    f32 py;

    new_var = 1;
    if (*ang < 0x1500) return 0;
    if (*ang == 0x4000) *ang = 0;
    py = *(f32 *)(e + 0xB0) - (f32)((f32)*ang / 10000.0f) * scale;
    nn = pl->n[0] * pl->n[0] + pl->n[new_var] * pl->n[new_var] + pl->n[2] * pl->n[2];
    t = pl->d + (pl->n[0] * *(f32 *)(e + 0xAC) + pl->n[new_var] * py + pl->n[2] * *(f32 *)(e + 0xB4));
    out[0] = -(pl->n[0] * t / nn);
    out[new_var] = -(pl->n[new_var] * t / nn);
    out[2] = -(pl->n[2] * t / nn);
    return new_var;
}

/* 0x0011C9F0: angle (0x10000 = 360 degrees) between normal n and up. */
static void check_angle(f32 *n, s32 *out) {
    f32 up[3];

    up[0] = 0.0f;
    up[1] = 1.0f;
    up[2] = 0.0f;
    *out = (s32)(0.5f + 65536.0f * flArcCos(flvecInnerProduct(n, up)) / 6.2831855f) & 0xFFFF;
}

/* HitWallPlayer: original bytes kept (build/raw/HitWallPlayer.inc, config/c_rawfuncs.txt); the C below is the
 * near-match the PC build uses (4 lines off, tools/align.py). */
#ifdef __MWERKS__
asm void HitWallPlayer(HENT *ent, int keep)
{
#include "HitWallPlayer.inc"
}
#else
void HitWallPlayer(HENT *ent, int keep) {
    SV3 ang;
    HV3 o;
    HV3 seg[2];
    f32 m[16];
    f32 *p;
    f32 r;
    f32 sx, sy, sz;
    u16 mask;

    if (!(keep & 0xFF)) {
        ent->hitdir = 0;
    }
    if (ent->x4D4 != 0 && (Pl_stg_ck(ent) & 0xFF)) {
        ent->x59C = ent->x59C & 0xFE;
        p = push00[0];
        sx = ent->scl[0];
        sy = ent->scl[1];
        sz = ent->scl[2];
        if (ent->mon != 0) {
            if (game_w.x1DC == 0) {
                p = D_63FC50[ent->kind];     /* (the PC build names it em_hit_push_tbl: G's 52 kinds) */
#ifdef VERSION_MHG
                /* G (main 0x10CDC0): a monster of 60-200 % size uses its base scale on all three axes of the wall spheres */
                if (ent->size_pct >= 60 && ent->size_pct <= 200) {
                    sx = sy = sz = ent->x600;
                }
#endif
            } else {
                p = D_6103A0[ent->kind];
            }
        }
        while (p[3] != -1.0f) {
            r = p[3] * sx;
#ifdef VERSION_MHG
            if (ent->mon != 0 && (ent->kind == 7 || ent->kind == 50)) {
#else
            if (ent->mon != 0 && ent->kind == 7) {
#endif
                ang.x = 0;
                ang.y = ent->ang_y;
                ang.z = 0;
                cpRotMatrixYXZ2(&ang.x, m);
                flvecApplyMat33(&o.x, p, m);
                seg[0].x = ent->old[0] + o.x * sx;
                seg[0].y = ent->old[1] + o.y * sy;
                seg[0].z = ent->old[2] + o.z * sz;
                seg[1].x = ent->pos[0] + o.x * sx;
                seg[1].y = ent->pos[1] + o.y * sy;
                seg[1].z = ent->pos[2] + o.z * sz;
            } else {
                seg[0].x = ent->old[0] + p[0] * sx;
                seg[0].y = ent->old[1] + p[1] * sy;
                seg[0].z = ent->old[2] + p[2] * sz;
                seg[1].x = ent->pos[0] + p[0] * sx;
                seg[1].y = ent->pos[1] + p[1] * sy;
                seg[1].z = ent->pos[2] + p[2] * sz;
            }
            if (ent->mon != 0) {
#ifdef VERSION_MH2
                mask = ent->wmask;
#else
                mask = EF(ent, u16, 0x95E);
#endif
                if (ent->st == 2) mask |= 0x200;
                if (game_w.gate_open != 0) mask |= 0x4000;
                GetWallHitBitEm(r, &seg[0].x, mask, ent);
            } else if (game_w.gate_open != 0) {
                GetWallHitBitPl(r, &seg[0].x, 0xC004, ent);
            } else {
                GetWallHitBitPl(r, &seg[0].x, 0x8004, ent);
            }
            p += 4;
        }
    }
}
#endif


#if defined(VERSION_MHG) && !defined(__MWERKS__)
/* func_0010AE00 (G main 0x10AE00, 2016 bytes, G only; the PC build has it as C, the PS2 build keeps the original bytes):
 * ground line test from a to b. Walks the ground grid cells (cell size diorama_w.gcsx / gcsz) in the rectangle spanned by a and b, skips
 * cells whose centre is farther than 1 cell diagonal + 1/10 cell each way from the segment, and returns 1 at the first polygon of a visited cell
 * that the segment crosses (the crossing point in *out when it is nearer than |a-b|). Without a hit *out = b and the result is 0.
 * Written from the asm, the 4th argument (always 9) is not read. Quirk kept: a polygon is compared only with the FIRST one remembered. */
int func_0010AE00(f32 *a, f32 *b, f32 *out, int mode) {
    f32 seg[6];           /* b, a: the segment FaceLinePos wants */
    f32 dir[3];
    f32 pc[3], q[3], hit[3];
    f32 best, rad, diag, fx, fz, t, den;
    s32 cx0, cz0, dx, dz, sx, sz, i, j, ax, az;
    HPOLY *seen[20];
    s32 nseen = 0;
    int found = 0;
    f32 len;
    (void)mode;

    best = flvecCalcDistance(a, b);
    if (!GroundFieldInCheck(a)) return 0;
    if (!GroundFieldInCheck(b)) return 0;
    fx = (f32)diorama_w.gcsx;
    fz = (f32)diorama_w.gcsz;
    cx0 = (s32)(a[0] / fx);
    cz0 = (s32)(a[2] / fz);
    dx = (s32)(b[0] / fx) - cx0;
    dz = (s32)(b[2] / fz) - cz0;
    if (dx < 0) { sx = -1; dx = 1 - dx; } else { sx = 1; dx = dx + 1; }
    if (dz < 0) { sz = -1; dz = 1 - dz; } else { sz = 1; dz = dz + 1; }
    seg[0] = b[0]; seg[1] = b[1]; seg[2] = b[2];
    seg[3] = a[0]; seg[4] = a[1]; seg[5] = a[2];
    dir[0] = a[0] - b[0];
    dir[1] = 0.0f;
    dir[2] = a[2] - b[2];
    len = flSqrt(dir[0] * dir[0] + dir[2] * dir[2]);
    if (len > 0.0f) {
        dir[0] /= len;
        dir[2] /= len;
    }
    if (dir[0] == -0.0f) dir[0] = 0.0f;
    if (dir[2] == -0.0f) dir[2] = 0.0f;
    diag = flSqrt(fx * fx + (f32)(u32)(diorama_w.gcsz * diorama_w.gcsz));
    rad = (f32)(diorama_w.gcsz / 10) + ((f32)(diorama_w.gcsx / 10) + diag);
    ax = 0;
    for (i = 0; i < dx; i++, ax += sx) {
        az = 0;
        for (j = 0; j < dz; j++, az += sz) {
            HPOLY **list;
            pc[0] = (f32)(diorama_w.gcsx >> 1) + fx * (f32)(cx0 + ax);
            pc[1] = 0.0f;
            pc[2] = (f32)(diorama_w.gcsz >> 1) + fz * (f32)(cz0 + az);
            if (dir[0] == 0.0f && dir[2] == 0.0f) {
                t = 0.0f;
            } else {
                den = dir[0] * dir[0] + dir[2] * dir[2];
                t = (-dir[0] * (a[0] - pc[0]) - dir[2] * (a[2] - pc[2])) / den;
            }
            q[0] = a[0] + dir[0] * t;
            q[1] = 0.0f;
            q[2] = a[2] + dir[2] * t;
            if (!(flvecCalcDistance(pc, q) <= rad)) continue;
            list = (HPOLY **)GetGroundTblAdrs(pc);
            if ((s32)list == -1 || (s32)list[0] == -1) continue;
            for (; (s32)*list != -1; list++) {
                HPOLY *f = *list;
                if (nseen > 0 && f == seen[0]) continue;
                if (FaceLinePos(seg, f->n, f->v[0], hit)) {
                    if (!NormalClipFace(hit, f->v[0], f->n)) continue;
                    found = 1;
                    t = flvecCalcDistance(a, hit);
                    if (!(best <= t)) {
                        SetVector(out, hit[0], hit[1], hit[2]);
                        best = t;
                    }
                }
                if (nseen < 20) seen[nseen++] = f;
                if (found) return 1;
            }
        }
    }
    SetVector(out, b[0], b[1], b[2]);
    return 0;
}
#endif
