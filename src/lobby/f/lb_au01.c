/* lb_au01 - browser quit/poster/init states, page work 0x005F2890-0x005F293C: BsPoster00_PostData0. Whole file in lb_au.c. */
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

void BsPoster00_PostData0(void) {
    u8 *c;
    u8 *p;
    c = bsCsv;
    if (c[0] != 0 && BsRequestPostAdd(lit_429_00667370, c) >= 0) {
        c = bsCsv;
        p = c + 0x11;
        if (c[0x11] != 0 && BsRequestPostAdd(lit_430_00667378, p) >= 0) {
            c = bsCsv;
            p = c + 0x11D;
            if (c[0x11D] != 0) {
                BsUrlSet(bsUrl, p);
                BsRequestHtmlPost(bsUrl);
                bsMainRetVal = 0;
                bsIsOnRequesting = 1;
                bsSys->x02 = 5;
                return;
            }
        }
    }
    bsSys->x01 = 2;
}
