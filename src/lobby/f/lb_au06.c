/* lb_au06 - browser quit/poster/init states, page work 0x005FD2C0-0x005FD3A4: To_ReqCancelWait, SetNextURL. Whole file in lb_au.c. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *bsCsv;
extern u8 *lpSKey;
extern BSWK *bsOW[0x1F4];
extern u8 BsPsw[0x18];
extern u8 bsUrl[4];
extern s8 bsIsOnRequesting;
extern s8 bsMainRetVal;
extern u8 bsGoHidePage;
extern u8 bsRetryCtr;
extern u8 MMBB_LOGIN;
extern s8 BsBgImgReq, BsCursorReq, BsDialogReq, BsHScrlBarReq, BsPageObjReq, BsSoftKbdReq, BsToolMenuReq, BsTtlBarReq, BsVScrlBarReq;
extern s8 wpushCtr;
extern u8 FirstURL[];
extern u8 bssbuf[];
extern u8 *sbfptr;
extern u16 BsOuterTexHdl[0x28];
extern u16 BsOuterTexWidth[0x14];
extern u16 BsOuterTexHeight[0x14];
extern char lit_429_00667370[];
extern char lit_430_00667378[];
void drawBlank();
void BsWorkPush();
BSWK *BsWorkPull();
void BsDummyTask();
void MoveAndTransSet();
void PostLbsInfoGetOrGameEnd();
void To_BodyMain_RcvSrc();
void BsRequestHtmlGetCached();
void BsRequestCancelAll();
int BsUrlSet();
int BsRequestCheck();
int BsRequestPostAdd();
void BsRequestHtmlPost();
void BsCloseCapDlg();
void flfntInit();
int strcmp();
char *strcpy();

void To_ReqCancelWait(int a, int b, int c) {
    int s;
    s = (s8)a;
    switch (s) {
    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF:
        goto go;
    default:
        if (bsSys->x38 == 0) {
go:
            BsRequestCancelAll(s, b);
            bsSys->x02 = a;
        }
    }
}

int SetNextURL(char *url, int b, int c) {
    memset(bsSys->meta, 0, 0x100);
    strcpy(bsSys->meta, url);
    return BsUrlSet(bsUrl, bsSys->meta);
}
