/* lb_z107 - auto-drafted 0x005F3DE0-0x005F3FCC: MoveAndTransSet (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s8 bsNetErrOccur;
extern int bsSys;
extern u8 BsDialogReq;
extern u8 BsSoftKbdReq;
extern u8 bsFirstOpen;
extern char BsDialogTrans[];
extern char BsCursorTrans[];
extern char BsBgImgTrans[];
extern char BsPageObjTrans[];
extern char BsHScrlBarTrans[];
extern char BsVScrlBarTrans[];
extern char BsTitleBarTrans[];
extern char BsToolMenuTrans[];
extern char BsSoftKbdTrans[];

void MoveAndTransSet(void) {
    s8 temp_a1;
    int temp_a0;

    temp_a0 = bsSys;
    temp_a1 = F(s8, temp_a0, 0x36);
    if ((temp_a1 != 0) && (F(u8, temp_a0, 0x2E) != 0xA) && (F(s8, temp_a0, 0x3A) >= 0)) {
        BsSetRenderState(0x14, 0xFF000001);
        BsWorkMove(6);
        TransSet(&BsDialogTrans);
        return;
    }
    if (temp_a1 == 0) {
        BsSetRenderState(0x14, F(s32, temp_a0, 0x14));
    } else if ((BsDialogReq == 1) && (F(s8, temp_a0, 0x3A) == 0)) {
        BsSetRenderState(0x14, F(s32, temp_a0, 0x14));
    } else {
        BsSetRenderState(0x14, 0xFF000001);
    }
    if (bsNetErrOccur > 0) {
        BsWorkMove(6);
        TransSet(&BsDialogTrans);
        BsWorkMove(8);
        TransSet(&BsCursorTrans);
        return;
    }
    if (bsFirstOpen != 0) {
        BsWorkMove(6);
        TransSet(&BsDialogTrans);
        return;
    }
    BsWorkMove(0);
    TransSet(&BsBgImgTrans);
    BsWorkMove(1);
    TransSet(&BsPageObjTrans);
    BsWorkMove(2);
    TransSet(&BsHScrlBarTrans);
    TransSet(&BsVScrlBarTrans);
    TransSet(&BsTitleBarTrans);
    BsWorkMove(3);
    TransSet(&BsToolMenuTrans);
    BsWorkMove(4);
    TransSet(&BsSoftKbdTrans);
    if (BsSoftKbdReq != 1) {
        BsWorkMove(6);
        TransSet(&BsDialogTrans);
        BsWorkMove(8);
        TransSet(&BsCursorTrans);
    }
}
