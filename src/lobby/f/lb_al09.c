/* lb_al09 - browser screen element objects 0x005E54B0-0x005E5720: BsDialogCharSet, BsDialogTask. Whole file in lb_al.c. */
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

void BsDialogCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(6, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsDialogTask;
            w->trans = BsDialogSprTrans;
            w->x34 = 213.0f;
            w->x38 = 128.0f;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 213.0f;
            w->x44 = 128.0f;
            w->x06 = 0;
            w->x07 = 1;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            BsDialogReq = 1;
        }
    }
}

void BsDialogTask(BSWK *w) {
    u8 r;
    r = BsDialogReq;
    switch (r) {
    case 1:
        if (bsSys->x2E == 8) {
            w->x06 = 1;
            return;
        }
        w->x06 = 0;
        return;
    case 2:
        w->x06 = 1;
        w->x34 = 213.0f;
        w->x38 = 128.0f;
        w->x40 = 213.0f;
        w->x44 = 128.0f;
        return;
    case 3:
        w->x06 = 2;
        w->x34 = 0x1F0 - bsSys->x1E;
        w->x38 = 0x16C - bsSys->x1C;
        w->x40 = 128.0f;
        w->x44 = 64.0f;
        return;
    case 0:
    case 4:
    case 5:
    case 6:
    case 9:
    case 10:
    default:
        w->x06 = 3;
        w->x34 = 60.0f;
        w->x38 = 80.0f;
        w->x40 = 640.0f - (2.0f * w->x34);
        w->x44 = 448.0f - (2.0f * w->x38);
        return;
    case 7:
        w->x06 = 4;
        w->x34 = 175.0f;
        w->x38 = 102.0f;
        w->x40 = 290.0f;
        w->x44 = 244.0f;
        return;
    case 8:
        w->x06 = 5;
        w->x34 = 175.0f;
        w->x38 = 102.0f;
        w->x40 = 290.0f;
        w->x44 = 244.0f;
        return;
    case 11:
        w->x06 = 7;
        w->x34 = 130.0f;
        w->x38 = 82.0f;
        w->x40 = 380.0f;
        w->x44 = 284.0f;
        return;
    case 12:
        w->x06 = 0x63;
        return;
    }
}
