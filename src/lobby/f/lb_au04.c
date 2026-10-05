/* lb_au04 - browser quit/poster/init states, page work 0x005F7260-0x005F72C8: BsQuit02_Push2. Whole file in lb_au.c. */
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

void BsQuit02_Push2(void) {
    int i;
    BSWK **p;
    BSWK *w;
    i = 0;
    p = bsOW;
    for (;;) {
        w = *p;
        if (w != 0) {
            BsWorkPush(w);
            i = (i + 1) & 0xFFFF;
            p += 1;
            if (i < 0x1F4) {
                continue;
            }
        }
        break;
    }
    bsSys->x02 = 3;
}
