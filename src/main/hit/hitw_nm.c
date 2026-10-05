/* hitw_nm (not built): HitWallPlayer (0x0011CA80): runs the wall test for one
 * player or monster entity (ent+0x10 != 0: monster). For every sphere of its
 * collision list (push00 for players, a per-kind table D_63FC50 (game.bin) /
 * D_6103A0 (lobby.bin) for monsters; entries {x, y, z, r}, r == -1 ends) it
 * builds the segment from the old position (ent+0x5A0 + offset) to the new
 * one (ent+0xAC + offset) and calls GetWallHitBitPl / GetWallHitBitEm. Kind 7
 * monsters turn the offsets with the facing. The mask excludes some wall
 * kinds (0x8004 / 0xC004 for players with the gate flag, ...). Guesses. */
#include "types.h"
#include "game.h"

/* entity (player 0xA00 / monster 0xA10 bytes): the fields this file reads */
typedef struct HENT {
    u8 _pad000[2];
    u8 kind;            /* 0x002 */
    u8 _pad003[0x10 - 3];
    u8 mon;             /* 0x010 non-zero: monster */
    u8 _pad011[0xA4 - 0x11];
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
    u8 _pad5AC[0x74C - 0x5AC];
    s32 hitdir;         /* 0x74C */
    u8 _pad750[0x95E - 0x750];
    u16 wmask;          /* 0x95E */
} HENT;

u8 Pl_stg_ck(void *);
void cpRotMatrixYXZ2(s32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void GetWallHitBitEm(f32, f32 *, int, void *);
void GetWallHitBitPl(f32, f32 *, int, void *);
extern f32 push00[][4];
extern f32 *D_63FC50[];
extern f32 *D_6103A0[];

void HitWallPlayer(HENT *ent, int keep) {
    s32 ang[3];
    f32 o[3];
    f32 seg[6];
    f32 m[16];
    f32 *p;
    f32 r;
    u16 mask;

    if (!(keep & 0xFF)) {
        ent->hitdir = 0;
    }
    if (ent->x4D4 != 0 && (Pl_stg_ck(ent) & 0xFF)) {
        ent->x59C = ent->x59C & 0xFE;
        p = push00[0];
        if (ent->mon != 0) {
            if (game_w.x1DC == 0) {
                p = D_63FC50[ent->kind];
            } else {
                p = D_6103A0[ent->kind];
            }
        }
        while (p[3] != -1.0f) {
            r = p[3] * ent->scl[0];
            if (ent->mon != 0 && ent->kind == 7) {
                ang[0] = 0;
                ang[1] = ent->ang_y;
                ang[2] = 0;
                cpRotMatrixYXZ2(ang, m);
                flvecApplyMat33(o, p, m);
                seg[0] = ent->old[0] + o[0] * ent->scl[0];
                seg[1] = ent->old[1] + o[1] * ent->scl[1];
                seg[2] = ent->old[2] + o[2] * ent->scl[2];
                seg[3] = ent->pos[0] + o[0] * ent->scl[0];
                seg[4] = ent->pos[1] + o[1] * ent->scl[1];
                seg[5] = ent->pos[2] + o[2] * ent->scl[2];
            } else {
                seg[0] = ent->old[0] + p[0] * ent->scl[0];
                seg[1] = ent->old[1] + p[1] * ent->scl[1];
                seg[2] = ent->old[2] + p[2] * ent->scl[2];
                seg[3] = ent->pos[0] + p[0] * ent->scl[0];
                seg[4] = ent->pos[1] + p[1] * ent->scl[1];
                seg[5] = ent->pos[2] + p[2] * ent->scl[2];
            }
            if (ent->mon != 0) {
                mask = ent->wmask;
                if (ent->st == 2) mask |= 0x200;
                if (game_w.gate_open != 0) mask |= 0x4000;
                GetWallHitBitEm(r, seg, mask, ent);
            } else if (game_w.gate_open != 0) {
                GetWallHitBitPl(r, seg, 0xC004, ent);
            } else {
                GetWallHitBitPl(r, seg, 0x8004, ent);
            }
            p += 4;
        }
    }
}
