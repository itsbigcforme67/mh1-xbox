/* Model/motion file readers (AAN motion, AHI hierarchy). SLPM_654.95 0x0018F0B0-0x00190400.
 * Files are a header (word 1 = entry count) followed by variable sized entries; every entry has its
 * byte size at +8. Names are Capcom's (pl prefix = platform layer). */
#include "types.h"

extern int tree_model_num;

static void *GetFileHeadAAN(void *f);
static void *GetFileHeadAHI(void *f);
u8 *GetDataHeadAHI(u8 *f, u32 type);
u8 *GetModelDataAHI(u8 *f, u32 idx);
static void GetTreeModelNum(void *f, int model);
int plAHIGetTreeNum(void *f);
int plAHIGetTreeModelNum(void *f, int tree);

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

int plGetInitMotionSetSizeFromAHI(void *f, int tree) {
    int r;

    if (tree < plAHIGetTreeNum(f)) {
        r = plAHIGetTreeModelNum(f, tree) * 64 + 16;
    } else {
        r = 0;
    }
    return r;
}
