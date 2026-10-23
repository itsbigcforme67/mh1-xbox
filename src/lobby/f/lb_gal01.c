/* lb_gal01 - near-match fixes 0x005E43C0-0x005E441C: SoftKeyboard. Whole file in lb_al.c. */
#include "lobby_f.h"
#ifndef __MWERKS__
signed char SoftKeyboard_move();   /* (declared before its first use) */
#endif
extern BSSYS *bsSys;
extern s8 BsBgImgReq;
extern s8 BsPageObjReq;
extern s8 BsHScrlBarReq;
extern s8 BsVScrlBarReq;
extern s8 BsTtlBarReq;
extern s8 BsToolMenuReq;
BSWK *BsWorkPull();
void BsBgImgTask();
void BsBgImgSprTrans();
void BsPageObjTask();
void BsPageObjSprTrans();
void BsHScrlBarTask();
void BsHScrlBarSprTrans();
void BsVScrlBarTask();
void BsVScrlBarSprTrans();
void BsTitleBarTask();
void BsTitleBarSprTrans();
void BsToolMenuTask();
void BsToolMenuSprTrans();
void BsSoftKbdTask();
void BsSoftKbdSprTrans();
void BsCursorTask();
void BsCursorSprTrans();
void BsDialogTask();
void BsDialogSprTrans();
extern s8 BsSoftKbdReq;
extern s8 BsCursorReq;
extern s8 BsDoEmphasise;
extern s8 BsDialogReq;
extern char inputStrBuf[];
extern char BsPsw[];
extern char lit_880_00666050[];
u32 strlen();
char *strcpy();
int SoftKeyboard_set();
#ifdef __MWERKS__
int SoftKeyboard_move();
#else
signed char SoftKeyboard_move();   /* the real return type (tools/check_kr_returns.py) */
#endif
void flfntSetSize();

char *SoftKeyboard(void) {
    s8 sx;
    flfntSetSize(0x16, 0x16);
    if ((sx = SoftKeyboard_move(inputStrBuf, *(s16 *)BsPsw, *(s16 *)0x3F3714)) != 0) {
        return inputStrBuf;
    }
    return 0;
}
