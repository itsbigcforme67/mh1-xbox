/* lb_au03 - browser quit/poster/init states, page work 0x005F2C90-0x005F2DD4: BsPullPageWork, BsPushPageWork. Whole file in lb_au.c. */
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

BSWK *BsPullPageWork(void) {
    int i;
    BSWK **p;
    BSWK *w;
    memset(BsOuterTexHdl, 0, 0x50);
    memset(BsOuterTexWidth, 0, 0x28);
    memset(BsOuterTexHeight, 0, 0x28);
    i = 0;
    p = bsOW;
    for (;;) {
        w = BsWorkPull(1, 0);
        *p = w;
        if (w != 0) {
            i = (i + 1) & 0xFFFF;
            (*p)->x00 = 0;
            (*p)->x5F = 0;
            (*p)->task = BsDummyTask;
            (*p)->trans = BsDummyTask;
            (*p)->x01 = 0;
            (*p)->x5F = 0;
            (*p)->x05 = 0;
            p += 1;
            if (i < 0x1F4) {
                continue;
            }
        }
        break;
    }
    return w;
}

void BsPushPageWork(void) {
    int i;
    BSWK **p;
    BSWK *w;
    i = 0;
    p = bsOW;
    for (;;) {
        w = *p;
        if (w != 0) {
            BsWorkPush(w);
            *p = 0;
            i = (i + 1) & 0xFFFF;
            p += 1;
            if (i >= 0x1F4) {
            } else {
                continue;
            }
        }
        break;
    }
}
