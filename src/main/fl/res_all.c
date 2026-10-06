/* AAN/AHI file readers and motion set creators (plAAN*, plAHI*, plCreate*Motion*). SLPM_654.95 0x0018F0B0-0x001905F8: one source file in the
 * original (helpers are file-static), so it is one translation unit here. Files are a header (word 1 = entry count) followed by variable
 * sized entries; every entry has its byte size at +8. Names are Capcom's (pl prefix = platform layer). */
#include "types.h"

typedef struct F4 { f32 a, b, c, d; } F4;

extern int tree_model_num;
extern int base_addr_0038A300;
extern u8 fms[];

u32 fmsAllocMemory(void *p, u32 size, int from_top);
int fmsInitialize(void *p, u32 base, u32 size, u32 align);
void plFCVSetBaseAddress(u32 base);
f32 plGetFcurveTime(u8 *fc);
f32 plGetFcurveStartTime(u8 *fc);

static void *GetFileHeadAAN(void *f);
static void *GetFileHeadAHI(void *f);
u8 *GetDataHeadAHI(u8 *f, u32 type);
u8 *GetModelDataAHI(u8 *f, u32 idx);
static void GetTreeModelNum(void *f, int model);
int plAHIGetTreeNum(void *f);
int plAHIGetTreeModelNum(void *f, int tree);
int plAHIGetTreeModelNum2(void *f, int root);
int plAHIGetTreeRootModel(void *f, u32 tree);
int plAANGetModelNum(void *f);
u8 *GetModelHeadAAN(u8 *f, int idx);
static int plGetMotionSizeFromAAN(u8 *m);
void plCreateMotionFromAAN(u8 *out, u8 *aan, int idx);
static void plCreateMotionFromAANSub_SRT(u8 *aan, u8 *out, u8 *m, int unused);
static void plCreateFcurveDataAAN(u8 *src, u8 *dst);
static void plCreateFcurveDataAAN_Linear(u8 *src, u8 *dst);
static void plCreateFcurveDataAAN_Hermite(u8 *src, u8 *dst);
static void plCreateFcurveDataAAN_Complex(u8 *src, u8 *dst);
static void plCreateFcurveDataAAN_LinearShort(u8 *src, u8 *dst);
static void plCreateFcurveDataAAN_HermiteShort(u8 *src, u8 *dst);
static void plCreateFcurveDataAAN_ComplexShort(u8 *src, u8 *dst);
static f32 plAANGetMotionEndTime(u8 *m);
static f32 plAANGetMotionStartTime(u8 *m);
int plGetLoopInfoAan(void *f, f32 *out);
int plGetInitMotionSetSizeFromAHI(void *f, int tree);

static void *GetFileHeadAAN(void *f) {
    return f;
}

u8 *GetModelHeadAAN(u8 *f, int idx) {
    u8 *p;
    int i;
    u8 *q;

    p = (u8 *)GetFileHeadAAN(f);
    q = p + 0x14;
    p = q;
    for (i = 0; i < idx; i++) {
        q += *(int *)(p + 8);
        p = q;
    }
    return p;
}

int plGetLoopInfoAan(void *f, f32 *out) {
    int *h = (int *)GetFileHeadAAN(f);
    int r;

    if (h[3] != 0) {
        r = 1;
        *out = ((f32 *)h)[4];
    } else {
        *(int *)out = 0;
        r = 0;
    }
    return r;
}

int plGetMotionSetSizeFromAAN(void *f) {
    int i;
    int n;
    int size;
    u8 *m;

    n = plAANGetModelNum(f);
    i = 0;
    size = n * 4 + 0x20;
    for (; i < n; i++) {
        m = GetModelHeadAAN(f, i);
        size += 8;
        size += *(int *)(m + 4) * 8;
        size += plGetMotionSizeFromAAN(m);
    }
    return size;
}

static int plGetMotionSizeFromAAN(u8 *m) {
    u8 *p;
    int n;
    int size;
    int i;

    n = *(int *)(m + 4);
    p = m + 0xC;
    size = 0;

    for (i = 0; i < n; i++) {
        switch ((*(u32 *)p & 0xFF0000) >> 16) {
        case 0x21:
            size += *(int *)(p + 4) * 8;
            break;
        case 0x22:
            size += *(int *)(p + 4) * 16;
            break;
        case 0x23:
            size += *(int *)(p + 4) * 20;
            break;
        case 0x11:
            size += *(int *)(p + 4) * 4;
            break;
        case 0x12:
            size += *(int *)(p + 4) * 8;
            break;
        case 0x13:
            size += *(int *)(p + 4) * 12;
            break;
        }
        p += *(int *)(p + 8);
    }
    return size;
}

int plCreateMotionSetFromAAN(u8 *out, u32 base, u8 *aan) {
    int size;
    int i;
    int n;
    u8 *set;
    int *arr;
    f32 start;
    f32 end;
    f32 t;
    f32 loop;

    size = plGetMotionSetSizeFromAAN(aan);
    base_addr_0038A300 = base;
    plFCVSetBaseAddress(base);
    fmsInitialize(fms, base, size, 1);
    set = (u8 *)fmsAllocMemory(fms, 0x20, 0);
    n = plAANGetModelNum(aan);
    *(s16 *)set = 0;
    *(s16 *)(set + 2) = n;
    *(int *)(set + 0x10) = fmsAllocMemory(fms, n * 4, 0);
    *(int *)(set + 0x14) = 0;
    *(int *)(set + 0x18) = size;
    arr = *(int **)(set + 0x10);
    end = 0;
    start = 9.9999999e9f;
    for (i = 0; i < n; i++) {
        *arr = fmsAllocMemory(fms, 8, 0);
        plCreateMotionFromAAN((u8 *)*arr, aan, i);
        t = plAANGetMotionEndTime((u8 *)*arr);
        if (!(t <= end)) end = t;
        t = plAANGetMotionStartTime((u8 *)*arr);
        if (t < start) start = t;
        arr++;
    }
    *(f32 *)(set + 4) = start;
    *(f32 *)(set + 8) = end;
    if (plGetLoopInfoAan(aan, &loop)) {
        *(u16 *)set |= 0x8000;
        *(f32 *)(set + 0xC) = loop;
    } else {
        *(u16 *)set &= 0x7FFF;
        *(int *)(set + 0xC) = 0;
    }
    for (i = 0; i < n; i++) {
        (*(int **)(set + 0x10))[i] -= base_addr_0038A300;
    }
    *(int *)(set + 0x10) -= base_addr_0038A300;
    *(F4 *)out = *(F4 *)set;
    ((F4 *)out)[1] = ((F4 *)set)[1];
    *(u32 *)(out + 0x14) = base;
    return 1;
}

void plCreateMotionFromAAN(u8 *out, u8 *aan, int idx) {
    u8 *m;
    u8 *h;
    u32 k;
    u8 *t;

    if (plAANGetModelNum(aan) < idx) return;
    m = t = GetModelHeadAAN(aan, idx);
    if (t == 0) return;
    h = (u8 *)GetFileHeadAAN(aan);
    if (h == 0) return;
    switch (*h) {
    case 1:
        *(u16 *)out = 0x1000;
        break;
    case 2:
        *(u16 *)out = 0x2000;
        break;
    }
    k = *(u16 *)out & 0xF000;
    switch (k) {
    case 0x1000:
    case 0x2000:
        plCreateMotionFromAANSub_SRT(aan, out, m, 0);
        break;
    }
}

static void plCreateMotionFromAANSub_SRT(u8 *aan, u8 *out, u8 *m, int unused) {
    u8 *dst;
    u8 *p;
    int n;
    int i;
    u32 flags;

    n = *(int *)(m + 4);

    *(s16 *)(out + 2) = n;
    dst = (u8 *)fmsAllocMemory(fms, n * 8, 0);
    *(u8 **)(out + 4) = dst;
    for (i = 0; i < n; i++) {
        dst[i * 8] = 0x20;
        *(s16 *)(dst + i * 8 + 2) = 0;
    }
    p = m + 0xC;
    for (i = 0; i < n; i++) {
        flags = *(u32 *)p;
        if (flags & 0x80000000) {
            plCreateFcurveDataAAN(p, dst);
            switch (flags & 0x1FF) {
            case 1:
                dst[1] = 0;
                break;
            case 2:
                dst[1] = 1;
                break;
            case 4:
                dst[1] = 2;
                break;
            case 8:
                dst[1] = 3;
                break;
            case 0x10:
                dst[1] = 4;
                break;
            case 0x20:
                dst[1] = 5;
                break;
            case 0x40:
                dst[1] = 6;
                break;
            case 0x80:
                dst[1] = 7;
                break;
            case 0x100:
                dst[1] = 8;
                break;
            }
        }
        dst += 8;
        p += *(int *)(p + 8);
    }
    *(int *)(out + 4) -= base_addr_0038A300;
}

static void plCreateFcurveDataAAN(u8 *src, u8 *dst) {
    switch ((*(u32 *)src & 0xFF0000) >> 16) {
    case 0x21:
        plCreateFcurveDataAAN_Linear(src, dst);
        break;
    case 0x22:
        plCreateFcurveDataAAN_Hermite(src, dst);
        break;
    case 0x23:
        plCreateFcurveDataAAN_Complex(src, dst);
        break;
    case 0x11:
        plCreateFcurveDataAAN_LinearShort(src, dst);
        break;
    case 0x12:
        plCreateFcurveDataAAN_HermiteShort(src, dst);
        break;
    case 0x13:
        plCreateFcurveDataAAN_ComplexShort(src, dst);
        break;
    }
}

static void plCreateFcurveDataAAN_Linear(u8 *src, u8 *dst) {
    f32 *s = (f32 *)(src + 0xC);
    f32 *d;
    int i;

    dst[0] = 0x21;
    dst[1] = 0;
    *(u16 *)(dst + 2) = *(int *)(src + 4);
    d = (f32 *)fmsAllocMemory(fms, *(u16 *)(dst + 2) * 8, 0);
    *(f32 **)(dst + 4) = d;
    for (i = 0; i < *(u16 *)(dst + 2); i++) {
        d[0] = s[0];
        d[1] = s[1];
        s += 2;
        d += 2;
    }
    *(int *)(dst + 4) -= base_addr_0038A300;
}

static void plCreateFcurveDataAAN_Hermite(u8 *src, u8 *dst) {
    f32 *s = (f32 *)(src + 0xC);
    f32 *d;
    int i;

    dst[0] = 0x22;
    dst[1] = 0;
    *(u16 *)(dst + 2) = *(int *)(src + 4);
    d = (f32 *)fmsAllocMemory(fms, *(u16 *)(dst + 2) * 16, 0);
    *(f32 **)(dst + 4) = d;
    for (i = 0; i < *(u16 *)(dst + 2); i++) {
        d[0] = s[0];
        d[1] = s[1];
        d[2] = s[2];
        d[3] = s[3];
        s += 4;
        d += 4;
    }
    *(int *)(dst + 4) -= base_addr_0038A300;
}

static void plCreateFcurveDataAAN_Complex(u8 *src, u8 *dst) {
    u32 *s = (u32 *)(src + 0xC);
    u32 *d;
    int i;

    dst[0] = 0x23;
    dst[1] = 0;
    *(u16 *)(dst + 2) = *(int *)(src + 4);
    d = (u32 *)fmsAllocMemory(fms, *(u16 *)(dst + 2) * 20, 0);
    *(u32 **)(dst + 4) = d;
    for (i = 0; i < *(u16 *)(dst + 2); i++) {
        d[0] = s[0];
        ((f32 *)d)[1] = ((f32 *)s)[1];
        ((f32 *)d)[2] = ((f32 *)s)[2];
        ((f32 *)d)[3] = ((f32 *)s)[3];
        ((f32 *)d)[4] = ((f32 *)s)[4];
        s += 5;
        d += 5;
    }
    *(int *)(dst + 4) -= base_addr_0038A300;
}

static void plCreateFcurveDataAAN_LinearShort(u8 *src, u8 *dst) {
    u16 *s = (u16 *)(src + 0xC);
    u16 *d;
    int i;

    dst[0] = 0x11;
    dst[1] = 0;
    *(u16 *)(dst + 2) = *(int *)(src + 4);
    d = (u16 *)fmsAllocMemory(fms, *(u16 *)(dst + 2) * 4, 0);
    *(u16 **)(dst + 4) = d;
    for (i = 0; i < *(u16 *)(dst + 2); i++) {
        d[0] = s[0];
        d[1] = s[1];
        s += 2;
        d += 2;
    }
    *(int *)(dst + 4) -= base_addr_0038A300;
}

static void plCreateFcurveDataAAN_HermiteShort(u8 *src, u8 *dst) {
    u16 *s = (u16 *)(src + 0xC);
    u16 *d;
    int i;

    dst[0] = 0x12;
    dst[1] = 0;
    *(u16 *)(dst + 2) = *(int *)(src + 4);
    d = (u16 *)fmsAllocMemory(fms, *(u16 *)(dst + 2) * 8, 0);
    *(u16 **)(dst + 4) = d;
    for (i = 0; i < *(u16 *)(dst + 2); i++) {
        d[0] = s[0];
        d[1] = s[1];
        d[2] = s[2];
        d[3] = s[3];
        s += 4;
        d += 4;
    }
    *(int *)(dst + 4) -= base_addr_0038A300;
}

static void plCreateFcurveDataAAN_ComplexShort(u8 *src, u8 *dst) {
    u16 *s = (u16 *)(src + 0xC);
    u16 *d;
    int i;

    dst[0] = 0x13;
    dst[1] = 0;
    *(u16 *)(dst + 2) = *(int *)(src + 4);
    d = (u16 *)fmsAllocMemory(fms, *(u16 *)(dst + 2) * 12, 0);
    *(u16 **)(dst + 4) = d;
    for (i = 0; i < *(u16 *)(dst + 2); i++) {
        *(u32 *)d = *(u32 *)s;
        d[2] = s[2];
        d[3] = s[3];
        d[4] = s[4];
        d[5] = s[5];
        s += 6;
        d += 6;
    }
    *(int *)(dst + 4) -= base_addr_0038A300;
}

static f32 plAANGetMotionEndTime(u8 *m) {
    f32 end = 0;
    int i;
    f32 t;

    for (i = 0; i < *(s16 *)(m + 2); i++) {
        t = plGetFcurveTime((u8 *)(*(int *)(m + 4) + base_addr_0038A300 + i * 8));
        if (!(t <= end)) end = t;
    }
    return end;
}

static f32 plAANGetMotionStartTime(u8 *m) {
    f32 start = 9.9999999e9f;
    int i;
    f32 t;

    for (i = 0; i < *(s16 *)(m + 2); i++) {
        t = plGetFcurveStartTime((u8 *)(*(int *)(m + 4) + base_addr_0038A300 + i * 8));
        if (t < start) start = t;
    }
    return start;
}

int plAANGetModelNum(void *f) {
    return ((int *)GetFileHeadAAN(f))[1];
}

int plAHIGetModelNum(void *f) {
    int *h = GetFileHeadAHI(f);
    int n = h[1];

    if (h[0] & 0x80000000) {
        n--;
    }
    return n;
}

int plAHIGetTreeNum(void *f) {
    int *d = (int *)GetDataHeadAHI(f, 0);

    if (d != 0) {
        return d[1];
    }
    return 0;
}

int plAHIGetTreeRootModel(void *f, u32 tree) {
    int *d = (int *)GetDataHeadAHI(f, 0);

    if (d != 0 && tree < (u32)d[1]) {
        return d[3 + tree];
    }
    return -1;
}

int plAHIGetTreeModelNum(void *f, int tree) {
    return plAHIGetTreeModelNum2(f, plAHIGetTreeRootModel(f, tree));
}

int plAHIGetTreeModelNum2(void *f, int root) {
    tree_model_num = 0;
    GetTreeModelNum(f, root);
    return tree_model_num;
}

static void GetTreeModelNum(void *f, int model) {
    int *m;

    tree_model_num++;
    m = (int *)GetModelDataAHI(f, model);
    if (m[2] != -1) {
        GetTreeModelNum(f, m[2]);
    }
    if (m[3] != -1) {
        GetTreeModelNum(f, m[3]);
    }
}

static void *GetFileHeadAHI(void *f) {
    return f;
}

u8 *GetDataHeadAHI(u8 *f, u32 type) {
    u32 i;
    u32 n = ((int *)GetFileHeadAHI(f))[1];

    f += 0xC;
    for (i = 0; i < n; i++) {
        if (*f == type) {
            return f;
        }
        f += *(int *)(f + 8);
    }
    return 0;
}

u8 *GetModelDataHeadAHI(u8 *f, u32 idx) {
    int k = 0;
    u32 i;
    u32 n = ((int *)GetFileHeadAHI(f))[1];

    if (idx < n) {
        f += 0xC;
        for (i = 0; i < n; i++) {
            if (*f != 0) {
                if (k == idx) {
                    return f;
                }
                k++;
            }
            f += *(int *)(f + 8);
        }
    }
    return 0;
}

u8 *GetModelDataAHI(u8 *f, u32 idx) {
    u8 *p = GetModelDataHeadAHI(f, idx);

    if (p != 0) {
        return p + 0xC;
    }
    return 0;
}

/* original bytes: build/raw/plGetInitMotionSetSizeFromAHI.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int plGetInitMotionSetSizeFromAHI(void *f, int tree)
{
#include "plGetInitMotionSetSizeFromAHI.inc"
}
#else
int plGetInitMotionSetSizeFromAHI(void *f, int tree) {
    int r;

    if (tree >= plAHIGetTreeNum(f)) {
        r = 0;
    } else {
        r = plAHIGetTreeModelNum(f, tree) * 64 + 16;
    }
    return r;
}
#endif

/* original bytes: build/raw/plCreateInitMotionSetFromAHI.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int plCreateInitMotionSetFromAHI(u8 *out, u8 *tmp, u8 *ahi, int tree)
{
#include "plCreateInitMotionSetFromAHI.inc"
}
#else
int plCreateInitMotionSetFromAHI(u8 *out, u8 *tmp, u8 *ahi, int tree) {
    int n;
    int root;
    int i;
    int j;
    int k;
    u8 *node;
    u8 *q;
    u8 *d;
    f32 (*nm)[4];
    f32 (*dm)[4];

    if (!(tree < plAHIGetTreeNum(ahi))) {
        return 0;
    }
    *(int *)(tmp + 0xC) = plGetInitMotionSetSizeFromAHI(ahi, tree);
    n = plAHIGetTreeModelNum(ahi, tree);
    *(s16 *)(tmp + 2) = n;
    *(int *)(tmp + 8) = 0;
    *(int *)(tmp + 4) = 0x10;
    node = tmp + 0x10;
    for (i = 0, q = node; i < n; i++) {
        nm = (f32 (*)[4])(q + 0x10);
        for (j = 0; j < 4; j++) {
            nm[0][j] = 0;
            nm[1][j] = 0;
            nm[2][j] = 0;
        }
        q += 0x40;
    }
    root = plAHIGetTreeRootModel(ahi, tree);
    for (i = 0; i < n; i++) {
        d = GetModelDataAHI(ahi, i + root);
        *(s16 *)node = *(int *)d;
        *(s16 *)(node + 2) = *(int *)(d + 0x44);
        *(s16 *)(node + 6) = *(int *)(d + 8);
        *(s16 *)(node + 8) = *(int *)(d + 0xC);
        if (*(s16 *)(node + 6) != -1) {
            *(s16 *)(node + 6) -= (s16)root;
        }
        if (*(s16 *)(node + 8) != -1) {
            *(s16 *)(node + 8) -= (s16)root;
        }
        *(s16 *)(node + 0xA) = *(int *)(d + 0x40);
        nm = (f32 (*)[4])(node + 0x10);
        dm = (f32 (*)[4])(d + 0x10);
        for (j = 0; j < 4; j++) {
            nm[0][j] = dm[0][j];
            nm[1][j] = dm[1][j];
            nm[2][j] = dm[2][j];
        }
        node += 0x40;
    }
    *(F4 *)out = *(F4 *)tmp;
    *(u8 **)(out + 8) = tmp;
    return 1;
}
#endif
