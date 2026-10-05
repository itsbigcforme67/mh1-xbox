/* weapon3 - SLPM_654.95 0x00168EA0-0x1691B4 (f_weapon, part 4): skeleton
 * matrices of the player model (player_mat_calc: per-joint scale, then the
 * skin transform; player_modify: world matrix from position/angle/scale),
 * player_mk (all players), get_tex_num (count of used texture slots) and
 * Material_set_sub (hand a model's materials to the renderer). The larger
 * display functions of this part are in weapon3_nm.c (near-matches).
 * Names guessed from the code. */
#include "types.h"
#include "pl.h"
#include "fl.h"

extern u32 mem_tex[];
void Pl_horm_adj(PLW *, s16);

void flmatMul(FLMAT *, f32 *, FLMAT *);
void flmatCopy(void *, void *);
void flCalcTrans(void *, FLMAT *);
void flCalcTransSI(void *, FLMAT *);
void flSetSkinTrans(void *);
void cpAng2Rad_all(void *, f32 *);

/* 0x190-byte skeleton node: matrix at +0x40, translation row at +0x70 */
typedef struct PLNODE {
    u8 _pad00[0x40];
    u8 m40[0x30];       /* 0x40 matrix (rotation part) */
    f32 pos[3];         /* 0x70 (overlaps the matrix translation row) */
    u8 _pad7C[0xC4 - 0x7C];
    s16 jnt;            /* 0xC4 */
    u8 _padC6[0x190 - 0xC6];
} PLNODE;

void player_mat_calc(PLW *pl, PLNODE *n, FLMAT *base) {
    FLMAT sc;
    FLMAT res;
    int i;
    int j;
    PLNODE *np = n;
    PLNODE *q = n;

    for (i = 0; i < 0x15; i++) {
        Pl_horm_adj(pl, np->jnt);
        flmatMakeScale(&sc, *(f32 *)((u8 *)pl->part[i] + 0x98), *(f32 *)((u8 *)pl->part[i] + 0x9C), *(f32 *)((u8 *)pl->part[i] + 0xA0));
        flmatMul(&res, (f32 *)np->m40, &sc);
        np->pos[0] = ((f32 *)res)[12];
        np->pos[1] = ((f32 *)res)[13];
        np->pos[2] = ((f32 *)res)[14];
        np++;
    }
    flCalcTransSI(n, base);
    flSetSkinTrans(n);
    for (j = 0; j < 0x15; j++) {
        flmatCopy((u8 *)pl->part[q->jnt] + 0x40, q);
        q++;
    }
}

void player_modify(PLW *pl) {
    f32 ang[3];
    FLMAT m;
    u8 *mdl;

    if (pl->x01 != 0) {
        mdl = *(u8 **)((u8 *)pl + 0x50C);
        cpAng2Rad_all(&pl->ang, ang);
        flmatMakeScale(&m, pl->scl[0], pl->scl[1], pl->scl[2]);
        flmatRotXYZ33(&m, ang[0], ang[1], ang[2]);
        flmatSetTrans(&m, pl->pos[0], pl->pos[1], pl->pos[2]);
        flmatCopy(pl->rot, &m);
        flCalcTrans(*(void **)(mdl + 0x44), &m);
        player_mat_calc(pl, *(PLNODE **)(mdl + 0x44), &m);
    }
}

void player_mk(void) {
    PLW *p = player_work;
    int i;
    for (i = 0; i < 8; i++) {
        if (p->be_flag != 0) player_modify(p);
        p++;
    }
}

int get_tex_num(int n) {
    int i = 0;
    int c = 0;
    u32 *p = &mem_tex[n];
    for (; i < 0x1E; i++) {
        if (p[i] != 0) c++;
    }
    return c;
}

typedef struct MATSET { s32 x0; s32 num; s32 idx[1]; } MATSET;

void Material_set_sub(u8 *base, MATSET *m) {
    int i;
    for (i = 0; i < m->num; i++) {
        flSetRenderState((i + 0x3A) & 0xFF, (int)(base + m->idx[i] * 0x4C));
    }
}
