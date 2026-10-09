/* shit11_nm (not built): wall collision of players and monsters against the
 * stage wall HITS (f_sphr 0x00114D90-0x00115EB0). Both build an HSWEEP from
 * the entity's old/new position, look at the 2x2 (or more) grid cells
 * around the new position, test every wall polygon (sphr_face_o3: players AND
 * monsters through GetWallHitBitEm; sphr_face_o4 is GetWallHitBit2's) and let PushAdjust3 move the entity out. The
 * touched polygons end up in hited_wall_no[]; players get them (angle
 * relative to their facing, kind, normal) in pl_wall_mat[id] (up to 21
 * entries, 0xC bytes, terminated by flags = 0, a "special" wall first),
 * monsters get a bit mask of 32 directions (ent+0x74C) and a special-wall
 * flag (ent+0x95D). Guesses from the code; the original arithmetic for the
 * cell start (BlockPlaceCgeck quadrant plus offsets) is kept as written. */
#include "types.h"
#include "hit3.h"
#include "game.h"

#define EF(e, T, o) (*(T *)((u8 *)(e) + (o)))

int WallFieldInCheck(f32 *);
s32 *GetWallTblAdrs(f32 *);
int BlockPlaceCgeck(f32 *);
int PushAdjust3(HSWEEP *, f32 *, u8 *);
int sphr_face_o3(HSWEEP *, HPOLY *, f32 *);
int sphr_face_o4(HSWEEP *, HPOLY *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
u16 calc_vec_ang(f32, f32, f32, f32);
extern f32 lit_180_002E85D0[8];
extern f32 lit_425_002E85F0[8];
extern s8 hited_wall_no[];

typedef struct WMAT {
    u16 flags;          /* 0x00 1 = wall touched, 2 = special wall */
    u16 ang;            /* 0x02 direction of the contact relative to the facing */
    f32 *n;             /* 0x04 polygon normal */
    u8 kind;            /* 0x08 polygon b1 */
    u8 _pad09[3];
} WMAT;                 /* 0xC */
extern WMAT pl_wall_mat[][21];
extern u8 *wall_tbl_add[];

/* GetWallHitBitPl: 2 x 2 cells, player wall mask 0x7FFF (stage 0x27-0x2B in
 * the arena: 0xFFFF otherwise). r is the player radius. */
void GetWallHitBitPl(f32 r, f32 *seg, int mask, void *pl) {
    HSWEEP sw;
    f32 q[8];
    f32 p[2][2];
    f32 pt[3];
    HPOLY *ring[20];
    u8 flag = 0;
    int nr = 0;
    int nh = 0;
    int found = 0;
    int ix, iz, i;
    int quad;
    int ix0, iz0;
    u32 m2;
    f32 *pos;
    s32 *cp;
    HPOLY *poly;
    int n;
    int sp;
    WMAT *wm;
    s8 a;
    u16 ang;
    WMAT t;

    for (i = 0; i < 8; i++) q[i] = lit_180_002E85D0[i];
    if (WallFieldInCheck(seg + 3) == 0) return;
    hit_poly_num = 0;
    hited_poly_num = 0;
    pos = (f32 *)((u8 *)pl + 0xAC);
    sw.p0[0] = seg[3];
    sw.p0[1] = seg[4];
    sw.p0[2] = seg[5];
    sw.p1[0] = seg[0];
    sw.p1[1] = seg[1];
    sw.p1[2] = seg[2];
    sw.r = r;
    sw.w20 = 1;
    sw.w22 = 1;
    sw.len = flvecCalcDistance(sw.p0, sw.p1);
    if (EF(pl, u8, 0x14) != 0 || EF(pl, u8, 0x15) < 0x27 || EF(pl, u8, 0x15) >= 0x2C) {
        m2 = 0xFFFF;
    } else {
        m2 = 0x7FFF;
    }
    quad = BlockPlaceCgeck(seg + 3);
    ix0 = (int)(seg[3] / (f32)(u32)diorama_w.wcsx);
    iz0 = (int)(seg[5] / (f32)(u32)diorama_w.wcsz);
    for (ix = 0; ix < 2; ix++) {
        for (iz = 0; iz < 2; iz++) {
            pt[0] = (f32)(u32)diorama_w.wcsx * ((f32)ix0 + q[quad * 2]);
            pt[2] = (f32)(u32)diorama_w.wcsz * ((f32)iz0 + q[quad * 2 + 1]);
            pt[0] += (f32)(((u32)diorama_w.wcsx >> 1) + (u32)diorama_w.wcsx * ix);
            pt[2] += (f32)(((u32)diorama_w.wcsz >> 1) + (u32)diorama_w.wcsz * iz);
            if (WallFieldInCheck(pt) == 1) {
                cp = GetWallTblAdrs(pt);
                if (*cp != -1) {
                    do {
                        poly = (HPOLY *)*cp;
                        if (!((mask & 0xFFFF) & (poly->h2 & m2))) {
                            if (nr != 0) {
                                found = 0;
                                for (i = 0; i < nr; i++) {
                                    if (poly == ring[i]) found = 1;
                                }
                            }
                            if (nr == 0 || found == 0) {
                                if (nh != 0) {
                                    found = 0;
                                    for (i = 0; i < nh; i++) {
                                        if (poly == hit_wall[i]) found = 1;
                                    }
                                }
                                if (found == 0) {
                                    if (sphr_face_o3(&sw, poly, pos) != 0) {
                                        hit_wall[nh++] = poly;
                                    }
                                    ring[nr++] = poly;
                                    if (nr >= 0x14) nr = 0;
                                }
                            }
                        }
                        cp++;
                    } while (*cp != -1);
                }
            }
        }
    }
    PushAdjust3(&sw, pos, &flag);
    if (flag != 0) flag = 0;
    n = 0;
    wm = pl_wall_mat[EF(pl, u16, 0xC)];
    wm[0].flags = 0;
    sp = 0;
    if (hited_poly_num != 0) {
        for (i = 0; i < hited_poly_num; i++) {
            a = hited_wall_no[i];
            poly = hit_wall[a];
            ang = (u16)(((((calc_vec_ang(hit_near_point[a][0], hit_near_point[a][2], pos[0], pos[2]) & 0xFFFF) + 0x4000) & 0xFFFF) - EF(pl, s32, 0xA4)) & 0xFFFF) - 0x400;
            wm[i].flags = 1;
            if (wall_tbl_add[game_w.stage][poly->kind * 8] != 0) {
                sp = 1;
                wm[i].flags |= 2;
            }
            n = (n + 1) & 0xFF;
            wm[i].kind = poly->b1;
            wm[i].ang = ang;
            wm[i].n = poly->n;
            if (sp != 0) {
                sp = 0;
                if (n != 1) {
                    t = wm[0];
                    wm[0] = wm[n - 1];
                    wm[n - 1] = t;
                }
            }
            EF(pl, s32, 0x74C) |= 1 << (ang >> 11);
        }
        wm[n].flags = 0;
    }
}

/* GetWallHitBitEm: monsters; the grid area is sized by the sphere (n x n
 * cells, 2x2 when small). */
void GetWallHitBitEm(f32 r, f32 *seg, int mask, void *em) {
    HSWEEP sw;
    f32 q[8];
    f32 pt[3];
    HPOLY *ring[20];
    u8 tmp[8];
    int nr = 0;
    int nh = 0;
    int found = 0;
    int ix, iz, i;
    int quad = 0;
    int ix0, iz0;
    int n2;
    int half;
    f32 *pos;
    f32 cs;
    s32 *cp;
    HPOLY *poly;
    s8 a;
    u16 ang;

    for (i = 0; i < 8; i++) q[i] = lit_425_002E85F0[i];
    if (WallFieldInCheck(seg + 3) == 0) return;
    hit_poly_num = 0;
    hited_poly_num = 0;
    pos = (f32 *)((u8 *)em + 0xAC);
    sw.p0[0] = seg[3];
    sw.p0[1] = seg[4];
    sw.p0[2] = seg[5];
    sw.p1[0] = seg[0];
    sw.p1[1] = seg[1];
    sw.p1[2] = seg[2];
    sw.r = r;
    sw.w20 = 1;
    sw.w22 = 1;
    sw.len = flvecCalcDistance(sw.p0, sw.p1);
    if ((u32)diorama_w.wcsz >= (u32)diorama_w.wcsx) {
        cs = (u32)diorama_w.wcsx;
    } else {
        cs = (u32)diorama_w.wcsz;
    }
    n2 = (int)((2.0f * r) / cs) + 2;
    if (n2 == 2) quad = BlockPlaceCgeck(seg + 3);
    ix0 = (int)(seg[3] / (f32)(u32)diorama_w.wcsx);
    iz0 = (int)(seg[5] / (f32)(u32)diorama_w.wcsz);
    if (n2 > 0) {
        half = n2 / 2;
        for (ix = 0; ix < n2; ix++) {
            for (iz = 0; iz < n2; iz++) {
                if (n2 == 2) {
                    pt[0] = (f32)(u32)diorama_w.wcsx * ((f32)ix0 + q[quad * 2]);
                    pt[2] = (f32)(u32)diorama_w.wcsz * ((f32)iz0 + q[quad * 2 + 1]);
                    pt[0] += (f32)(((u32)diorama_w.wcsx >> 1) + (u32)diorama_w.wcsx * ix);
                    pt[2] += (f32)(((u32)diorama_w.wcsz >> 1) + (u32)diorama_w.wcsz * iz);
                } else {
                    pt[0] = (f32)ix0 * (f32)(u32)diorama_w.wcsx - (f32)half * (f32)(u32)diorama_w.wcsx;
                    pt[2] = (f32)iz0 * (f32)(u32)diorama_w.wcsz - (f32)half * (f32)(u32)diorama_w.wcsz;
                    pt[0] += (f32)((u32)diorama_w.wcsx * ix);
                    pt[2] += (f32)((u32)diorama_w.wcsz * iz);
                }
                if (WallFieldInCheck(pt) == 1) {
                    cp = GetWallTblAdrs(pt);
                    if (*cp != -1) {
                        do {
                            poly = (HPOLY *)*cp;
                            if (!(poly->h2 & (mask & 0xFFFF))) {
                                if (nr != 0) {
                                    found = 0;
                                    for (i = 0; i < nr; i++) {
                                        if (poly == ring[i]) found = 1;
                                    }
                                }
                                if (nr == 0 || found == 0) {
                                    if (nh != 0) {
                                        found = 0;
                                        for (i = 0; i < nh; i++) {
                                            if (poly == hit_wall[i]) found = 1;
                                        }
                                    }
                                    if (found == 0) {
                                        if (sphr_face_o3(&sw, poly, pos) != 0) { /* the original calls o3 here too (o4 is only used by GetWallHitBit2) */
                                            hit_wall[nh++] = poly;
                                        }
                                        ring[nr++] = poly;
                                        if (nr >= 0x14) nr = 0;
                                    }
                                }
                            }
                            cp++;
                        } while (*cp != -1);
                    }
                }
            }
        }
    }
    PushAdjust3(&sw, pos, tmp);
    EF(em, s8, 0x95D) = 0;
    for (i = 0; i < hited_poly_num; i++) {
        a = hited_wall_no[i];
        poly = hit_wall[a];
        ang = (u16)(((((calc_vec_ang(hit_near_point[a][0], hit_near_point[a][2], pos[0], pos[2]) & 0xFFFF) + 0x4000) & 0xFFFF) - EF(em, s32, 0xA4)) & 0xFFFF) - 0x400;
        EF(em, s32, 0x74C) |= 1 << (ang >> 11);
        if (wall_tbl_add[game_w.stage][poly->kind * 8 + 1] != 0) {
            EF(em, s8, 0x95D) = 1;
        }
    }
}
