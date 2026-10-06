/* lb_gau03 - mutation fixes 0x005F7110-0x005F7188: BsQuit00_Init. Whole file in lb_au.c. */
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

void BsQuit00_Init(void) {
    BsBgImgReq = 4;
    wpushCtr = 0;
    BsPageObjReq = 5;
    BsToolMenuReq = 2;
    BsDialogReq = 0xC;
    BsTtlBarReq = 5;
    BsVScrlBarReq = 5;
    BsHScrlBarReq = 5;
    BsSoftKbdReq = 3;
    BsCursorReq = 3;
    if ((s8)bsSys->x36 == 0) {
        drawBlank(3, 5);
    }
    bsSys->x02 = 1;
}
