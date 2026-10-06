/* lb_al08 - browser screen element objects 0x005E4C90-0x005E4D90: BsCursorCharSet, BsCursorTask. Whole file in lb_al.c. */
#include "lobby_f.h"
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
int SoftKeyboard_move();
void flfntSetSize();

void BsCursorCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(8, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsCursorTask;
            w->trans = BsCursorSprTrans;
            w->x34 = 150.0f;
            w->x38 = 100.0f;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 20.0f;
            w->x44 = 20.0f;
            w->x06 = 0;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 5;
            w->x0E = 0;
            w->x10 = 0;
            BsCursorReq = 1;
            BsDoEmphasise = 0;
        }
    }
}

void BsCursorTask(BSWK *w) {
    u8 r;
    r = BsCursorReq;
    switch (r) {
    case 2:
        w->x06 = 0;
        break;
    case 1:
        w->x06 = 1;
        break;
    case 3:
        w->x06 = 0x63;
        break;
    }
}
