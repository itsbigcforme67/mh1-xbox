/* cnlbs01 - cnLBS file download requests 0x0027CF10-0x0027D028: cnLBS_Read_FileDownload, cnLBS_Get_FileDownloadInfo, cnLBS_Read_FileDownloadHeader. Whole file in cnlbs_nm.c. */
#include "g_alias.h"
#ifndef __MWERKS__
unsigned short __cnet_SendReq_FileDownloadHeader();   /* its real return type: was implicitly int (tools/check_kr_returns.py) */
#endif
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



#if !defined(G_PART)
int cnLBS_Read_FileDownload(int a, void *cb)
{
    if (A_U8(0x677344) == 0) {
        A_S32(0x6AFF74) = a;
        A_U8(0x6AFF7C) = 0;
        memset(D_6AFF80, 0, 0x180);
        A_S32(0x677328) = (s32)cb;
        A_U8(0x677344) = 1;
        A_S32(0x677324) = (s32)__cnet_bgProg_ReadFileDownloadAllocation;
        A_U8(0x677345) = 0;
        return 0;
    }
    return -1;
}
#endif

#if !defined(G_PART) || G_PART == 1
int cnLBS_Get_FileDownloadInfo(u8 *a, void **b)
{
    *a = A_U8(GV(0x6AFF7C, 0x731162));
    *b = D_6AFF80;
    return 0;
}
#endif

#if !defined(G_PART)
int cnLBS_Read_FileDownloadHeader(void *cb)
{
    int h = func_5AD280(1, 0, cb);

    if (h != -1) {
        *(u16 *)(D_6763AE + h * 28) = __cnet_SendReq_FileDownloadHeader();
        return h;
    }
    return -1;
}
#endif
