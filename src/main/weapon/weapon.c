/* weapon - SLPM_654.95 0x00163E40 (f_weapon, part 2): weapon / equipment
 * parts display. SetPartsTrans walks a tree of parts (child at +0xD0,
 * sibling at +0xCC), builds each part's world matrix from the player's joint
 * matrices (PLW.part[]) and hands it to the renderer. weapon_dat_make* read
 * a key-frame table (frame, values...) and interpolate it at the player's
 * current frame (PLW+0x19C), table ends with frame -1. Struct names and
 * meanings are guesses from the code. */
#include "types.h"
#include "pl.h"
#include "fl.h"

#ifndef NULL
#define NULL 0
#endif

typedef struct PARTS {
    FLMAT m00;          /* 0x00 (translation at 0x30) */
    FLMAT m40;          /* 0x40 */
    FLMAT m80;          /* 0x80 */
    s16 joint;          /* 0xC0? read as 0xC4 below */
    u8 _padC2[2];
    s16 j;              /* 0xC4 index into ptmat_tbl */
    u8 _padC6[0xCC - 0xC6];
    struct PARTS *next; /* 0xCC sibling */
    struct PARTS *child;/* 0xD0 */
} PARTS;

extern s16 *ptmat_tbl[];

void flmatCopy(f32 *, f32 *);
void flmatInit(FLMAT *);
void flmatMul(f32 *, f32 *, f32 *);
void flmatMul33(f32 *, f32 *, f32 *);
void plmatCopy33(f32 *, f32 *);
s32 frame_check2(f32, PLW *, int);

void SetPartsTrans(PARTS *n, PLW *pl, s16 type, int a3) {
    FLMAT w;
    FLMAT m;
    s16 k;

    k = ptmat_tbl[type][n->j];
    if (k > 0) {
        if (k < 0x40) {
            flmatCopy((f32 *)m, (f32 *)((u8 *)pl->part[k & 0x3F] + 0x40));
        } else {
            flmatCopy((f32 *)m, (f32 *)n);
            plmatCopy33((f32 *)m, (f32 *)((u8 *)pl->part[k & 0x3F] + 0x40));
            flmatMul33((f32 *)m, (f32 *)&n->m40, (f32 *)((u8 *)pl->part[k & 0x3F] + 0x40));
        }
    } else {
        flmatInit(&m);
        m[3][0] = n->m00[3][0];
        m[3][1] = n->m00[3][1];
        m[3][2] = n->m00[3][2];
    }
    flmatMul((f32 *)w, (f32 *)&n->m80, (f32 *)m);
    flSetRenderState((n->j + 0x1A) & 0xFF, (u32)w);
    if (n->child != NULL) {
        SetPartsTrans(n->child, pl, type, a3);
    }
    if (n->next != NULL) {
        SetPartsTrans(n->next, pl, type, a3);
    }
}

void SetPartsTrans2(PARTS *n, PLW *pl, s16 type, int a3, f32 *pm) {
    FLMAT w;
    FLMAT m;
    s16 k;

    k = ptmat_tbl[type][n->j];
    if (k > 0) {
        if (k < 0x40) {
            flmatCopy((f32 *)m, (f32 *)((u8 *)pl->part[k & 0x3F] + 0x40));
        } else {
            flmatMul((f32 *)m, (f32 *)&n->m40, pm);
        }
    } else {
        flmatInit(&m);
        m[3][0] = n->m00[3][0];
        m[3][1] = n->m00[3][1];
        m[3][2] = n->m00[3][2];
    }
    flmatMul((f32 *)w, (f32 *)&n->m80, (f32 *)m);
    flSetRenderState((n->j + 0x1A) & 0xFF, (u32)w);
    if (n->child != NULL) {
        SetPartsTrans2(n->child, pl, type, a3, (f32 *)m);
    }
    if (n->next != NULL) {
        SetPartsTrans2(n->next, pl, type, a3, pm);
    }
}

f32 weapon_dat_make(PLW *pl, f32 *t) {
    while (1) {
        if (t[0] == -1.0f) {
            return t[1];
        }
        if (frame_check2(t[0], pl, 0) != 0 && frame_check2(t[2], pl, 0) == 0) {
            return t[1] + (t[3] - t[1]) * ((*(f32 *)((u8 *)pl + 0x19C) - t[0]) / (t[2] - t[0]));
        }
        t += 2;
    }
}

void weapon_dat_make2(PLW *pl, f32 *t, f32 *out) {
    while (1) {
        if (t[0] == -1.0f) {
            out[0] = t[1];
            out[1] = t[2];
            out[2] = t[3];
            return;
        }
        if (frame_check2(t[0], pl, 0) != 0 && frame_check2(t[4], pl, 0) == 0) {
            out[0] = t[1] + (t[5] - t[1]) * ((*(f32 *)((u8 *)pl + 0x19C) - t[0]) / (t[4] - t[0]));
            out[1] = t[2] + (t[6] - t[2]) * ((*(f32 *)((u8 *)pl + 0x19C) - t[0]) / (t[4] - t[0]));
            out[2] = t[3] + (t[7] - t[3]) * ((*(f32 *)((u8 *)pl + 0x19C) - t[0]) / (t[4] - t[0]));
            return;
        }
        t += 4;
    }
}

void weapon_dat_make3(PLW *pl, f32 *t, f32 *a, f32 *b) {
    while (1) {
        if (t[0] == -1.0f) {
            *a = t[1];
            *b = t[2];
            return;
        }
        if (frame_check2(t[0], pl, 0) != 0 && frame_check2(t[3], pl, 0) == 0) {
            *a = t[1] + (t[4] - t[1]) * ((*(f32 *)((u8 *)pl + 0x19C) - t[0]) / (t[3] - t[0]));
            *b = t[2] + (t[5] - t[2]) * ((*(f32 *)((u8 *)pl + 0x19C) - t[0]) / (t[3] - t[0]));
            return;
        }
        t += 3;
    }
}
