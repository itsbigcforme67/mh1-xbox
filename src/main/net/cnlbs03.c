/* cnlbs03 - cnLBS file download requests 0x0027D380-0x0027D4A0: __cnet_SendReq_FileDownloadData, __cnet_CallBack_Result_Plaza_NumOfPlaza_0027D420, _cnet_CallBack_Result_Plaza_PlazaStatus_0027D460. Whole file in cnlbs_nm.c. */
#if defined(VERSION_MHG) && defined(MHG_ONLINE)
#define MHG_NET 1       /* G's command indices in the G PC online build (g_alias.h) */
#endif
#include "g_alias.h"
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



#if !defined(G_PART) || G_PART == 1
u16 __cnet_SendReq_FileDownloadData(int a, int b, int c)
{
    u16 r = func_5ADEE0(D_6EA740, GV(0xD1, 0xE8));

    func_5AE0B0(D_6EA740, a);
    func_5AE120(D_6EA740, b);
    func_5AE120(D_6EA740, c);
    func_5AE090(D_6EA740);
    func_5AE300(D_6EA740);
    return r;
}
#endif

#if !defined(G_PART) || G_PART == 2
void __cnet_CallBack_Result_Plaza_NumOfPlaza_0027D420(CBRES res)
{
    if (res.val == 0) {
        A_U8(GV(0x677346, 0x6FB753)) = 1;
        return;
    }
    A_U8(GV(0x677346, 0x6FB753)) = 2;
}
#endif

#if !defined(G_PART)
void _cnet_CallBack_Result_Plaza_PlazaStatus_0027D460(CBRES res)
{
    if (res.val == 0) {
        A_U8(0x677346) = A_U8(0x677346) + 1;
        return;
    }
    A_U8(0x677346) = 100;
}
#endif
