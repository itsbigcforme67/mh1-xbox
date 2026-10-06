/* fl clay: flPS2CreateClay (SLPM_654.95 0x0016B060): builds the clay handle after the shader parameter lookup (flPS2SetShaderParam). */
#include "types.h"

typedef struct CLAYS {
    u8 x00[0x14];
    s32 x14;            /* 0x014 */
    u8 x18[4];
    s32 mem2;           /* 0x01C */
    s32 buf;            /* 0x020 */
    s32 x24;            /* 0x024 */
    u8 x28[0x34 - 0x28];
    s32 count;          /* 0x034 */
    u8 x38[0x48 - 0x38];
    s32 shader;         /* 0x048 */
    s32 *param;         /* 0x04C */
    u8 x50[0x60 - 0x50];
    u8 list[1];         /* 0x060 */
} CLAYS;

extern s32 flClayNum;

void flPS2SetShaderParam();
void flPS2GetClaySize(void *, void *, int);
void flPS2ConvClayData(void *, void *);
void flPS2ClayMakeTextureList(void *, int);
void flPS2MakeMaterialDmaData(void *, int, int, int, void *);
int flPS2GetSystemBuffAdrs(int);

int flPS2CreateClay(CLAYS *c, void *src) {
    flPS2SetShaderParam();
    if (c->shader != -1) {
        flPS2GetClaySize(src, c, 0);
        flPS2ConvClayData(src, c);
        flPS2ClayMakeTextureList(c, 0);
        flPS2MakeMaterialDmaData(c, c->count, c->param[5], c->x24 + flPS2GetSystemBuffAdrs(c->mem2), c->list);
    } else {
        return 0;
    }
    flClayNum++;
    return 1;
}
