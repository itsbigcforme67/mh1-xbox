/* cnlbs02 - cnLBS file download requests 0x0027D1C0-0x0027D2A4: __cnet_SendReq_FileDownloadHeader, cnLBS_Read_FileDownloadData. Whole file in cnlbs_nm.c. */
#include "types.h"

typedef struct DLE { s32 size; s32 x4; s32 ofs; } DLE;
extern DLE D_6AFF80[];
extern u8 D_6EA130[];
extern u8 D_6EA740[];
extern u8 D_6763AE[];
void *memset(void *, int, int);
int func_5AD280();
int func_5AD1B0();
int func_5ADB60();
int func_5ADB90();
int func_5ADEE0();
int func_5AE090();
int func_5AE0B0();
int func_5AE120();
int func_5AE300();
int __cnet_bgProg_ReadFileDownloadAllocation();

#define A_U8(a) (*(u8 *)(a))
#define A_S32(a) (*(s32 *)(a))








typedef struct CBRES { s8 val; s8 id; u8 pad[6]; } CBRES;



u16 __cnet_SendReq_FileDownloadHeader(void)
{
    u16 r = func_5ADEE0(D_6EA740, 0xCF);

    func_5AE090(D_6EA740);
    func_5AE300(D_6EA740);
    return r;
}

int cnLBS_Read_FileDownloadData(int a, int b, int c, void *cb)
{
    int h = func_5AD280(1, 0, cb);

    if (h != -1) {
        *(u16 *)(D_6763AE + h * 28) = __cnet_SendReq_FileDownloadData(a, b, c);
        return h;
    }
    return -1;
}
