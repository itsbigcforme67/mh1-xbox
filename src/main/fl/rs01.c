/* rs01 - AAN/AHI readers 0x0018F0B0-0x0018F0B8: GetFileHeadAAN. Whole file in res_nm.c. */
#include "types.h"

extern int tree_model_num;

void *GetFileHeadAAN(void *f);
static void *GetFileHeadAHI(void *f);
u8 *GetDataHeadAHI(u8 *f, u32 type);
u8 *GetModelDataAHI(u8 *f, u32 idx);
static void GetTreeModelNum(void *f, int model);
int plAHIGetTreeNum(void *f);
int plAHIGetTreeModelNum(void *f, int tree);















void *GetFileHeadAAN(void *f) {
    return f;
}
