/* lb_au07 - browser body state 0x005F3020-0x005F309C: BsBody00_ReqSrc (request the page source). Whole file in lb_au.c. */
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

void BsBody00_ReqSrc(void) {
    MoveAndTransSet();
    if (bsGoHidePage != 0) {
        switch (MMBB_LOGIN) {
        case 2:
        case 1:
            PostLbsInfoGetOrGameEnd();
            To_BodyMain_RcvSrc();
            break;
        default:
            break;
        }
    } else {
        BsRequestHtmlGetCached(bsUrl);
        bsIsOnRequesting = 1;
        To_BodyMain_RcvSrc();
    }
}
