/* gmat02 - ground material data 0x0016AA20-0x0016ABAC: GetPlayerShagamiData, GetPlayerMaterialData, GetEmMaterialData, GetWaterData. Whole file in groundmat_nm.c. */
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
