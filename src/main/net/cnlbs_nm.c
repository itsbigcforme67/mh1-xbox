/* cnlbs01 - f_cnlbs (0x0027CF10-0x0027D4A0 part): file download requests to the lobby server (online). Guesses:
 * state bytes at 0x677344/45 and 0x67736C, entry table at D_6AFF80 (12-byte entries), request slots D_6763AE (28 bytes). */
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

int cnLBS_Get_FileDownloadInfo(u8 *a, void **b)
{
    *a = A_U8(0x6AFF7C);
    *b = D_6AFF80;
    return 0;
}

u16 __cnet_SendReq_FileDownloadHeader(void)
{
    u16 r = func_5ADEE0(D_6EA740, 0xCF);

    func_5AE090(D_6EA740);
    func_5AE300(D_6EA740);
    return r;
}

int cnLBS_Read_FileDownloadHeader(void *cb)
{
    int h = func_5AD280(1, 0, cb);

    if (h != -1) {
        *(u16 *)(D_6763AE + h * 28) = __cnet_SendReq_FileDownloadHeader();
        return h;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerFileDownloadHeader(void)
{
    u8 n;
    int i;
    int cur;
    DLE *p;
    int off;
    int sz;

    p = D_6AFF80;
    if (*(s8 *)0x67736C == 0) {
        cur = func_5ADB60(&n, D_6EA130);
        *(s8 *)0x6AFF7C = n;
        for (i = 0; i < n; i++) {
            cur = func_5ADB90(p, cur);
            p++;
        }
        off = A_S32(0x6AFF74);
        p = D_6AFF80;
        sz = 0;
        for (i = 0; i < n; i++) {
            p->ofs = off + sz;
            off = p->ofs;
            sz = p->size;
            p++;
        }
    }
    func_5AD1B0(0);
}

u16 __cnet_SendReq_FileDownloadData(int a, int b, int c)
{
    u16 r = func_5ADEE0(D_6EA740, 0xD1);

    func_5AE0B0(D_6EA740, a);
    func_5AE120(D_6EA740, b);
    func_5AE120(D_6EA740, c);
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

typedef struct CBRES { s8 val; s8 id; u8 pad[6]; } CBRES;

void __cnet_CallBack_Result_Plaza_NumOfPlaza_0027D420(CBRES res)
{
    if (res.val == 0) {
        A_U8(0x677346) = 1;
        return;
    }
    A_U8(0x677346) = 2;
}

void _cnet_CallBack_Result_Plaza_PlazaStatus_0027D460(CBRES res)
{
    if (res.val == 0) {
        A_U8(0x677346) = A_U8(0x677346) + 1;
        return;
    }
    A_U8(0x677346) = 100;
}
