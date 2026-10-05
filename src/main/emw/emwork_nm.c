/* View helpers and monster work slots. SLPM_654.95 0x00169DA0-0x0016A848 (g_RollView):
 * lpView accessors, em_work[0x14] allocation (pull_enemy_work) and release (push_em_work),
 * and the small pointer stacks (smell, smoke, senko, ear, em_yobi: 32 entries of work
 * pointers each, with a count). Field names guessed from use; offsets are exact. */
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

void em_work_set(EWK *w) {
    GWL *g = &game_w;
    int idx;
    MDLW *m;
    int h;
    int acc;
    int i;

    idx = get_start_mdlw(1);
    if (idx >= 0) {
        w->mdl_start = idx;
        w->mdl_num = 1;
        w->mdl = get_mdlw_ptr(idx);
        set_used_mdlw(idx, 1);
        m = get_mdlw_ptr(idx);
        memcpy(m, g->mdl_tmpl[w->x34F], 0x80);
        m->flag = 1;
        m->x74 = 2;
        m->x75 = 1;
        h = get_start_hierarchy(m->hier_n);
        m->hier_no = h;
        m->hier0 = get_hierarchy_ptr(h);
        m->hier1 = get_hierarchy_ptr(h + m->hier_n / 2);
        set_used_hierarchy(h, m->hier_n);
        acc = 0;
        for (i = 0; i < m->num; i++) {
            int ofs = acc * 400;
            flGetHierarchySI(m->hier0 + ofs, m->si[i]);
            flGetHierarchySI(m->hier1 + ofs, m->si[i]);
            m->a[i] = m->hier0 + ofs;
            m->b[i] = m->hier1 + ofs;
            acc += S16(m->hier0, 0xC2);
        }
    }
}

EWK *pull_enemy_work(void) {
    u32 i;
    EWK *w = (EWK *)em_work;

    for (i = 0; i < 0x14; i++) {
        if (w->be_flag == 0) {
            memset(w, 0, EMW_SIZE);
            w->slot = i;
            w->be_flag = 1;
            w->x10 = 1;
            w->x88D = -1;
            w->id = i;
            w->mdl = 0;
            w->x8C3 = game_w.master;
            w->x798 = 1.0f;
            w->x3F8[i] = 0;
            w->x410 = 0;
            return w;
        }
        w++;
    }
    return 0;
}

void smell_init(void) {
    int i;
    void **p = smell_stack;

    i = 0;
    do {
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
        p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0;
        i += 8;
        p += 8;
    } while (i < 0x20);
    smell_cnt = 0;
}

int push_smell(void *p) {
    int i;

    if (smell_cnt >= 0x20) {
        return 0;
    }
    for (i = 0; i < 0x20; i++) {
        if (smell_stack[i] == 0) {
            smell_stack[i] = p;
            smell_cnt++;
            return 1;
        }
    }
    return 0;
}

void pull_smell(void *p) {
    int i;
    void **q = smell_stack;

    for (i = 0; i < 0x20; i++) {
        if (*q == p) {
            *q = 0;
            smell_cnt--;
        }
        q++;
    }
}

void smoke_init(void) {
    int i;
    void **p = smoke_stack;

    i = 0;
    do {
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
        p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0;
        i += 8;
        p += 8;
    } while (i < 0x20);
    smoke_cnt = 0;
}

int push_smoke(void *p) {
    int i;

    if (smoke_cnt >= 0x20) {
        return 0;
    }
    for (i = 0; i < 0x20; i++) {
        if (smoke_stack[i] == 0) {
            smoke_stack[i] = p;
            smoke_cnt++;
            return 1;
        }
    }
    return 0;
}

void pull_smoke(void *p) {
    int i;
    void **q = smoke_stack;

    for (i = 0; i < 0x20; i++) {
        if (*q == p) {
            *q = 0;
            smoke_cnt--;
        }
        q++;
    }
}

void move_smoke(void) {
    int i;
    void **p = smoke_stack;

    for (i = 0; i < 0x20; i++) {
        u8 *q = *p;

        if (q != 0) {
            s8 v = (s8)q[0x15] - 1;
            q[0x15] = v;
            if (v <= 0) {
                pull_smoke(q);
            }
        }
        p++;
    }
}

void senko_init(void) {
    int i;
    void **p = senko_stack;

    i = 0;
    do {
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
        p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0;
        i += 8;
        p += 8;
    } while (i < 0x20);
    senko_cnt = 0;
}

int push_senko(void *p) {
    int i;

    if (senko_cnt >= 0x20) {
        return 0;
    }
    for (i = 0; i < 0x20; i++) {
        if (senko_stack[i] == 0) {
            func_539340(p);
            senko_stack[i] = p;
            senko_cnt++;
            return 1;
        }
    }
    return 0;
}

void pull_senko(void *p) {
    int i;
    void **q = senko_stack;

    for (i = 0; i < 0x20; i++) {
        if (*q == p) {
            *q = 0;
            senko_cnt--;
        }
        q++;
    }
}

void move_senko(void) {
    int i;
    void **p = senko_stack;

    for (i = 0; i < 0x20; i++) {
        u8 *q = *p;

        if (q != 0) {
            s8 v = (s8)q[0x15] - 1;
            q[0x15] = v;
            if (v <= 0) {
                pull_senko(q);
            }
        }
        p++;
    }
}

void ear_init(void) {
    int i;
    void **p = ear_stack;

    i = 0;
    do {
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
        p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0;
        i += 8;
        p += 8;
    } while (i < 0x20);
    ear_cnt = 0;
}

void em_yobi_init(void) {
    int i;
    void **p = em_yobi_stack;

    i = 0;
    do {
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
        p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0;
        i += 8;
        p += 8;
    } while (i < 0x20);
    em_yobi_cnt = 0;
}

int push_em_yobi(void *p) {
    int i;

    if (em_yobi_cnt >= 0x20) {
        return 0;
    }
    for (i = 0; i < 0x20; i++) {
        if (em_yobi_stack[i] == 0) {
            em_yobi_stack[i] = p;
            em_yobi_cnt++;
            return 1;
        }
    }
    return 0;
}

void pull_em_yobi(void *p) {
    int i;
    void **q = em_yobi_stack;

    for (i = 0; i < 0x20; i++) {
        if (*q == p) {
            *q = 0;
            em_yobi_cnt--;
        }
        q++;
    }
}

