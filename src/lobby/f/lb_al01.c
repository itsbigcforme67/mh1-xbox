/* lb_al01 - browser screen element objects 0x005E2DE0-0x005E2E74: BsBgImgCharSet. Whole file in lb_al.c. */
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

void BsBgImgCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(0, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsBgImgTask;
            w->trans = BsBgImgSprTrans;
            w->x34 = 0;
            w->x38 = 0;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 0;
            w->x44 = 0;
            w->x06 = 0;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            w->x5D = 0;
            BsBgImgReq = 1;
        }
    }
}
