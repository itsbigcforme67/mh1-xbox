/* lb_z60 - auto-drafted 0x005FD1F0-0x005FD2B8: To_BodyMain_ActDsp, To_QuitMain_Init, To_NetErrorDialog (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s8 BsDialogReq;
extern u8 * bsSys;
extern u8 * bsCur;
extern s8 BsCursorReq;

void To_BodyMain_ActDsp(void) {
    F(s8, bsSys, 0x2E) = 0xA;
    BsDialogReq = 1;
    F(s8, bsSys, 1) = 1;
    F(s8, bsSys, 2) = 5;
}

void To_QuitMain_Init(s8 arg0) {
    F(s8, bsSys, 0x2E) = arg0;
    BsSetRenderState(0x14, 0xFF000001);
    F(s8, bsSys, 1) = 2;
    F(s8, bsSys, 2) = 0;
    *bsCur = 2;
}

void To_NetErrorDialog(void) {
    F(s8, bsCur, 1) = 7;
    F(s8, bsSys, 0) = 0;
    F(s8, bsSys, 1) = 1;
    F(s8, bsSys, 2) = 5;
    F(s8, bsSys, 3) = 0;
    BsDialogReq = 0xB;
    BsCursorReq = 2;
}
