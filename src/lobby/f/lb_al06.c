/* lb_al06 - browser screen element objects 0x005E4050-0x005E4138: BsToolMenuCharSet, BsToolMenuTask. Whole file in lb_al.c. */
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

void BsToolMenuCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(3, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsToolMenuTask;
            w->trans = BsToolMenuSprTrans;
            w->x34 = 200.0f;
            w->x38 = 150.0f;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 200.0f;
            w->x44 = 150.0f;
            w->x06 = 0;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            BsToolMenuReq = 0;
        }
    }
}

void BsToolMenuTask(BSWK *w) {
    u8 r;
    r = BsToolMenuReq;
    switch (r) {
    case 0:
        w->x06 = 0;
        break;
    case 1:
        w->x06 = 1;
        break;
    case 2:
        w->x06 = 0x63;
        break;
    }
}
