/* SLPM_654.95 0x0018F0B0-0x0018F17C: GetFileHeadAAN (file-static in the original: GetModelHeadAAN only matches with it as a static of this file; other files reach it through config/main_aliases.txt), GetModelHeadAAN. See res_nm.c. */
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
