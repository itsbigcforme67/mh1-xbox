/* lb_al03 - browser screen element objects 0x005E3540-0x005E36D0: BsHScrlBarCharSet, BsHScrlBarTask. Whole file in lb_al.c. */
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

void BsHScrlBarCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(2, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsHScrlBarTask;
            w->trans = BsHScrlBarSprTrans;
            w->x34 = 0x270 - bsSys->x20;
            w->x38 = bsSys->x1A;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 16.0f;
            w->x44 = 380.0f;
            w->x06 = 0;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            w->x30 = 0;
            BsHScrlBarReq = 2;
        }
    }
}

void BsHScrlBarTask(BSWK *w) {
    u8 r;
    u8 c;
    r = BsHScrlBarReq;
    switch (r) {
    case 1:
        w->x06 = 0;
        break;
    case 2:
        w->x06 = 1;
        break;
    case 3:
        w->x06 = 2;
        c = w->x30;
        w->x30 = c + 1;
        if (c > 3) {
            w->x30 = 0;
            BsHScrlBarReq = 2;
        }
        break;
    case 4:
        w->x06 = 3;
        c = w->x30;
        w->x30 = c + 1;
        if (c > 3) {
            w->x30 = 0;
            BsHScrlBarReq = 2;
        }
        break;
    case 5:
        w->x06 = 0x63;
        break;
    }
}
