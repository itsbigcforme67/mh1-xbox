/* lb_gau01 - near-match fixes 0x005F29C0-0x005F2A74: BsPoster05_RcvData. Whole file in lb_au.c. */
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

void BsPoster05_RcvData(void) {
    u8 *r;
    u8 v;
    r = (u8 *)BsRequestCheck(bsUrl);
    if (r != 0) {
        bsIsOnRequesting = 0;
        switch (*(s8 *)(r + 4)) {
        case 0:
            if (*(s8 *)(r + 5) == 9) {
                v = bsRetryCtr + 1;
                bsRetryCtr = v;
                if ((v & 0xFF) != 2) {
                    bsMainRetVal = 0;
                    bsSys->x01 = 0;
                    bsSys->x02 = 0;
                    return;
                }
                bsSys->x01 = 2;
                return;
            }
            bsSys->x01 = 2;
            break;
        default:
            bsSys->x01 = 2;
        }
    }
}
