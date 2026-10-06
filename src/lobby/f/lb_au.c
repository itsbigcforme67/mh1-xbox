/* Lobby browser: quit/poster/init states, page work push/pull, hand-written from m2c drafts. */
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
void BsPoster06_RetError(void) {
    if (BsRequestCheck(bsUrl) != 0) {
        bsIsOnRequesting = 0;
        bsMainRetVal = -1;
        bsSys->x01 = 3;
    }
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
void BsBody00_ReqSrc(void) {
    MoveAndTransSet();
    if (bsGoHidePage != 0) {
        switch (MMBB_LOGIN) {
        case 2:
        case 1:
            break;
        default:
            return;
        }
        PostLbsInfoGetOrGameEnd();
        To_BodyMain_RcvSrc();
        return;
    }
    BsRequestHtmlGetCached(bsUrl);
    bsIsOnRequesting = 1;
    To_BodyMain_RcvSrc();
}
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
void BsCsMove06_CapWarn(void) {
    if ((*(u16 *)(BsPsw + 4) & 0x20) || lpSKey[0x658] == 0x28) {
        BsCloseCapDlg(1);
    }
    if ((*(u16 *)(BsPsw + 4) & 0x40) || lpSKey[0x658] == 0x29) {
        BsCloseCapDlg(0);
    }
}
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
void BsInit00_BootInit(void) {
    bsMainRetVal = 0;
    if (strcmp(FirstURL, bsCsv + 0x1C) == 0 || strcmp(FirstURL, bsCsv + 0x11D) == 0) {
        bsGoHidePage = 1;
    } else {
        bsGoHidePage = 0;
    }
    flfntInit();
    sbfptr = bssbuf;
    bsSys->x02 = bsSys->x02 + 1;
    bsSys->x03 = 0;
}
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
