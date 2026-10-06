/* lb_dd15 - browser: BsInitAllObj 0x005F2B00-0x005F2C6C (clears the browser work areas, sets the render state). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern char inputStrBuf[];
extern u8 bsUrl[4];
extern char BsOuterTexHdl[];
extern u8 bssbuf[];
extern u8 pal[];
extern u8 *sbfptr;
extern s16 bsNowFocus;
extern s8 bsImgNo;
extern char lit_499_006673A8[];
void *memset();
void BsPushPageWork();
void BsPullPageWork();
void stockTitle();
void BsSetRenderState();
void SetFilterMode();
void BsInitAllObj() {
    u16 s1;
    u8 *s0;
    memset(inputStrBuf, 0, 0x100);
    memset(bsUrl, 0, 4);
    memset(BsOuterTexHdl, 0, 0x50);
    BsPushPageWork();
    BsPullPageWork();
    memset(bssbuf, 0, 0x8000);
    sbfptr = bssbuf;
    s1 = 2;
    s0 = pal + 0x10;
    bssbuf[0] = 0;
    sbfptr = sbfptr + 1;
    do {
        memset(s0, 0, 8);
        s1++;
        s0 += 8;
    } while (s1 < 16);
    stockTitle(lit_499_006673A8);
    bsSys->x38 = 0x1E;
    bsSys->x2D = 0;
    bsSys->x2A = 0;
    bsSys->x2B = 0;
    F(s32, bsSys, 4) = 0;
    F(s32, bsSys, 8) = 0;
    bsSys->x0C = 0x248;
    bsSys->x10 = 0x17C;
    F(s8, bsSys, 0x35) = -1;
    bsNowFocus = 0;
    bsImgNo = 0;
    BsSetRenderState(0x60, 0xFF000001, 0x17C, 0x248);
    BsSetRenderState(0x5F, 5);
    BsSetRenderState(0x5E, 1);
    SetFilterMode(0);
}
