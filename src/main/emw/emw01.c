/* emw01 - view and monster work 0x00169DA0-0x0016A004: RollView, SetAngleOfView, Get_dist_to_view, Get_view_dir, clr_em_work, push_em_work, push_em_work_all. Whole file in emwork_nm.c. */
#include "types.h"

#define B8(p, o)   (*(u8 *)((u8 *)(p) + (o)))
#define S16(p, o)  (*(s16 *)((u8 *)(p) + (o)))
#define S32(p, o)  (*(s32 *)((u8 *)(p) + (o)))
#define PTR(p, o)  (*(u8 **)((u8 *)(p) + (o)))
#define EMW_SIZE   0xA10

typedef struct LPV {            /* camera view (lpView): eye and target, field of view, roll */
    f32 eye[3];                 /* 0x00 */
    f32 at[3];                  /* 0x0C */
    f32 x18[2];
    f32 fov;                    /* 0x2C is set by SetAngleOfView, see below */
} LPV;

/* Monster work slot, only the fields used here (EMW in em.h has the rest). */
typedef struct EWK {
    u8 be_flag;                 /* 0x000 */
    u8 _pad001[0x0C - 1];
    s16 id;                     /* 0x00C slot number */
    u8 _pad00E[2];
    u8 x10;                     /* 0x010 */
    u8 _pad011[2];
    u8 slot;                    /* 0x013 */
    u8 _pad014[0x34F - 0x14];
    u8 x34F;                    /* 0x34F which model template (game_w+0x88 table) */
    u8 _pad350[0x3F8 - 0x350];
    u8 x3F8[0x410 - 0x3F8];     /* 0x3F8 per slot flags, indexed by slot */
    u8 x410;                    /* 0x410 */
    u8 _pad411[0x508 - 0x411];
    s16 mdl_num;                /* 0x508 model work slots used */
    s16 mdl_start;              /* 0x50A first model work slot */
    struct MDLW *mdl;           /* 0x50C model work */
    u8 _pad510[0x564 - 0x510];
    s32 prim;                   /* 0x564 */
    s16 prim_no;                /* 0x568 */
    u8 _pad56A[0x798 - 0x56A];
    f32 x798;                   /* 0x798 */
    u8 _pad79C[0x88D - 0x79C];
    s8 x88D;                    /* 0x88D */
    u8 _pad88E[0x8C3 - 0x88E];
    u8 x8C3;                    /* 0x8C3 player number of the master */
    u8 _pad8C4[EMW_SIZE - 0x8C4];
} EWK;

extern u8 *lpView;
extern u8 em_work[];
/* Model work (copy of a template from game_w+0x88, 0x80 bytes). */
typedef struct MDLW {
    u8 flag;                    /* 0x00 */
    u8 _pad01[0x20 - 1];
    s16 hier_n;                 /* 0x20 hierarchy entries */
    s16 hier_no;                /* 0x22 first hierarchy slot */
    u8 *hier0;                  /* 0x24 hierarchy set A */
    u8 *hier1;                  /* 0x28 hierarchy set B (second half of the slots) */
    u8 _pad2C[8];
    s32 num;                    /* 0x34 parts */
    u8 _pad38[0x44 - 0x38];
    u8 *a[4];                   /* 0x44 */
    u8 *b[4];                   /* 0x54 */
    s32 si[4];                  /* 0x64 */
    u8 x74;
    u8 x75;
    u8 _pad76[0x80 - 0x76];
} MDLW;

typedef struct GWL {
    u8 _pad00[0x88];
    MDLW *mdl_tmpl[8];          /* 0x88 */
    u8 _pad_a8[0xD1 - 0xA8];
    u8 master;                  /* 0xD1 */
} GWL;
extern GWL game_w;
extern u8 mdlw_heap[];
extern void *smell_stack[0x20], *smoke_stack[0x20], *senko_stack[0x20], *ear_stack[0x20], *em_yobi_stack[0x20];
extern s8 smell_cnt, smoke_cnt, senko_cnt, ear_cnt, em_yobi_cnt;

void *memset(void *, int, unsigned);
void *memcpy(void *, const void *, unsigned);
f32 flvecCalcDistance(f32 *, f32 *);
f32 flArcTan2(f32, f32);
void flGetHierarchySI(void *, int);
void release_prim(int);
void model_work_free2(int);
void clr_used_mdlw(int, int);
int get_start_mdlw(int);
MDLW *get_mdlw_ptr(int);
void set_used_mdlw(int, int);
int get_start_hierarchy(int);
void *get_hierarchy_ptr(int);
void set_used_hierarchy(int, int);
void func_539340(void *);


























void RollView(f32 r) {
    *(f32 *)(lpView + 0x34) = r;
}

void SetAngleOfView(f32 a) {
    *(f32 *)(lpView + 0x2C) = a;
}

f32 Get_dist_to_view(f32 *p) {
    return flvecCalcDistance(p, (f32 *)lpView);
}

/* Heading from the camera target to the eye, as a 16 bit angle. */
u16 Get_view_dir(void) {
    f32 *v = (f32 *)lpView;
    f32 dz = v[5] - v[2];
    f32 dx = v[3] - v[0];

    return (s32)(0.5f + 65536.0f * flArcTan2(-dz, dx) / 6.2831855f);
}

void clr_em_work(void) {
    s16 i;
    u8 *w = em_work;

    for (i = 0; i < 0x14; i++) {
        memset(w, 0, 0xA00);
        w += EMW_SIZE;
    }
}

void push_em_work(EWK *w) {
    s16 i;

    if (w->prim != 0) {
        release_prim(w->prim_no);
    }
    if (w->mdl != 0) {
        for (i = w->mdl_start; i < w->mdl_start + w->mdl_num; i++) {
            model_work_free2(i);
            if (mdlw_heap[i] != 0) {
                clr_used_mdlw(i, 1);
            }
            w->mdl = 0;
            w->mdl_start = 0;
            w->mdl_num = 0;
        }
    }
    memset(w, 0, 0x12);
}

void push_em_work_all(void) {
    s16 i;
    EWK *w = (EWK *)em_work;

    for (i = 0; i < 0x14; i++) {
        if (w->be_flag != 0) {
            push_em_work(w);
        }
        w++;
    }
}
