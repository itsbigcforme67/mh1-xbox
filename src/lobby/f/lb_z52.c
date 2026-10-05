/* lb_z52 - auto-drafted 0x005F7190-0x005F7254: BsQuit01_Push1 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsSys;
extern char BsBgImgTrans[];
extern char BsPageObjTrans[];
extern char BsHScrlBarTrans[];
extern char BsVScrlBarTrans[];
extern char BsTitleBarTrans[];
extern char BsToolMenuTrans[];
extern char BsSoftKbdTrans[];
extern char BsDialogTrans[];
extern char BsCursorTrans[];

void BsQuit01_Push1(void) {
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
    BsWorkMove(6);
    TransSet(&BsDialogTrans);
    BsWorkMove(8);
    TransSet(&BsCursorTrans);
    F(s8, bsSys, 2) = 2;
}
