/* Ground material lookups and the frame memory stack. SLPM_654.95 0x0016A870-0x0016AC90
 * (g_RollView, second half). ground_tbl_add[stage] is an array of 16 byte per-ground-kind records;
 * the helpers pick per-stage camera/shadow/light/shagami/water data for a player (or a
 * monster for GetEmMaterialData). Names guessed from the table names. */
#include "types.h"

typedef struct GKIND {          /* 16 bytes per ground kind */
    u8 cam;                     /* 0x0 camera data id (GetGroundCameraData) */
    u8 x01;
    u8 shadow;                  /* 0x2 index into shadow_tbl_add[stage] (0 = none) */
    u8 light;                   /* 0x3 index into light_tbl_add[stage] */
    u8 _pad04[5];
    u8 shagami;                 /* 0x9 index into shagami_tbl_add[stage] */
    u8 _pad0A[2];
    u8 water;                   /* 0xC GetWaterData */
    u8 _pad0D[3];
} GKIND;

typedef struct GAREA {          /* 24 bytes: sphere in the stage, picks a diffuse set by distance */
    f32 pos[3];                 /* 0x00 */
    f32 r_in;                   /* 0x0C */
    f32 r_out;                  /* 0x10 */
    u16 set_in;                 /* 0x14 diffuse set (32 bytes each) used inside r_in */
    u16 set_out;                /* 0x16 used between r_in and r_out */
} GAREA;

typedef struct PWK {            /* player/monster work, fields used here */
    u8 _pad000[2];
    u8 kind;                    /* 0x002 monster kind */
    u8 _pad003[0xAC - 3];
    f32 pos[3];                 /* 0x0AC */
    u8 _pad0B8[0x415 - 0xB8];
    u8 cam;                     /* 0x415 */
    u8 _pad416[0x70C - 0x416];
    u8 gkind;                   /* 0x70C ground kind under the actor */
    u8 _pad70D[0x710 - 0x70D];
    u8 *diffuse;                /* 0x710 */
    u8 shagami;                 /* 0x714 */
    u8 _pad715[0x736 - 0x715];
    u8 stg;                     /* 0x736 */
} PWK;

typedef struct FMSTK {          /* frame memory stack: low end grows up, high end grows down */
    u32 base;                   /* 0x00 */
    u32 lo;                     /* 0x04 aligned start */
    u32 hi;                     /* 0x08 aligned end */
    u32 cur;                    /* 0x0C next low allocation */
    u32 top;                    /* 0x10 next high allocation (grows down) */
    u32 align;                  /* 0x14 */
} FMSTK;

extern GKIND *ground_tbl_add[];
extern GAREA *shadow_tbl_add[];
extern GAREA *light_tbl_add[];
extern u8 *diffuse_tbl_add[];
extern GAREA *shagami_tbl_add[];
f32 flvecCalcDistance(f32 *, f32 *);

void GetGroundCameraData(u8 kind, u8 *out, PWK *w) {
    *out = ground_tbl_add[w->stg][kind].cam;
}

void GetPlayerDiffuseData(u8 kind, f32 *pos, PWK *w) {
    u8 **out = &w->diffuse;
    GKIND *k = &ground_tbl_add[w->stg][kind];
    u8 *diff = diffuse_tbl_add[w->stg];
    int shadow = k->shadow;
    int light = k->light;
    GAREA *a;
    f32 d;

    if (diff == 0) {
        *out = 0;
    } else if (shadow != 0 && shadow_tbl_add[w->stg] != 0) {
        a = shadow_tbl_add[w->stg] + (u8)shadow;
        d = flvecCalcDistance(a->pos, pos);
        if (d <= a->r_out) {
            if (d <= a->r_in) {
                *out = diff + a->set_in * 32;
            } else {
                *out = diff + a->set_out * 32;
            }
        } else {
            *out = 0;
        }
    } else if (light != 0 && light_tbl_add[w->stg] != 0) {
        a = light_tbl_add[w->stg] + (u8)light;
        d = flvecCalcDistance(a->pos, pos);
        if (d <= a->r_out) {
            if (d <= a->r_in) {
                *out = diff + a->set_in * 32;
            } else {
                *out = diff + a->set_out * 32;
            }
        } else {
            *out = 0;
        }
    } else {
        *out = 0;
    }
}

void GetPlayerShagamiData(u8 kind, f32 *pos, PWK *w) {
    u8 *out = &w->shagami;
    GKIND *k;
    u8 idx = (&ground_tbl_add[w->stg][kind])->shagami;
    GAREA *a;

    if (!idx) {
        *out = 0;
    } else if (shagami_tbl_add[w->stg] == 0) {
        *out = 0;
    } else {
        a = (GAREA *)((u8 *)shagami_tbl_add[w->stg] + idx * 16);
        if (flvecCalcDistance(a->pos, pos) <= a->r_in) {
            *out = 1;
        } else {
            *out = 0;
        }
    }
}

void GetPlayerMaterialData(PWK *w) {
    GetGroundCameraData(w->gkind, &w->cam, w);
    GetPlayerDiffuseData(w->gkind, w->pos, w);
    GetPlayerShagamiData(w->gkind, w->pos, w);
}

void GetEmMaterialData(PWK *w) {
    if (w->kind == 9 || w->kind == 0x17) {
        GetPlayerDiffuseData(w->gkind, w->pos, w);
    }
}

u8 GetWaterData(PWK *w) {
    return ground_tbl_add[w->stg][w->gkind].water;
}

int fmsInitialize(FMSTK *p, u32 base, u32 size, u32 align) {
    u32 mask = ~(align - 1);
    u32 sz = mask & (size + align - 1);

    p->base = base;
    p->align = align;
    p->lo = ~(p->align - 1) & ((u32)p->base + p->align - 1);
    p->hi = ~(p->align - 1) & (p->align + (p->base + sz) - 1);
    p->cur = p->lo;
    p->top = p->hi;
    return 1;
}

/* Allocates size bytes (rounded up to the alignment) from the low end, or from the high end
 * when fromTop is set. Returns the address or 0 when the two ends would meet. */
u32 fmsAllocMemory(FMSTK *p, u32 size, int fromTop) {
    u32 top;
    u32 al;
    u32 cur;
    u32 sz;

    top = p->top;
    al = p->align;
    sz = ~(al - 1) & (size + al - 1);
    cur = p->cur;

    if (top < cur + sz) {
        return 0;
    }
    if (fromTop) {
        p->top = top - sz;
        return p->top;
    }
    p->cur = cur + sz;
    return cur;
}
