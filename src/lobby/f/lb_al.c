/* Lobby browser: screen element objects (background, page object, scroll bars, title bar, tool menu), hand-written from m2c drafts. */
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
void BsPageObjCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(1, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsPageObjTask;
            w->trans = BsPageObjSprTrans;
            w->x34 = 0;
            w->x38 = 0;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 0;
            w->x44 = 0;
            w->x06 = 1;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            BsPageObjReq = 3;
        }
    }
}
void BsPageObjTask(BSWK *w) {
    u8 r;
    r = BsPageObjReq;
    switch (r) {
    case 3:
        w->x06 = 1;
        break;
    case 5:
        w->x06 = 0x63;
        break;
    }
}
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
void BsVScrlBarCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(2, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsVScrlBarTask;
            w->trans = BsVScrlBarSprTrans;
            w->x34 = bsSys->x1E;
            w->x38 = 0x19C - bsSys->x1C;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 584.0f;
            w->x44 = 16.0f;
            w->x06 = 0;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            w->x30 = 0;
            BsVScrlBarReq = 2;
        }
    }
}
void BsVScrlBarTask(BSWK *w) {
    u8 r;
    u8 c;
    r = BsVScrlBarReq;
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
            BsVScrlBarReq = 2;
        }
        break;
    case 4:
        w->x06 = 3;
        c = w->x30;
        w->x30 = c + 1;
        if (c > 3) {
            w->x30 = 0;
            BsVScrlBarReq = 2;
        }
        break;
    case 5:
        w->x06 = 0x63;
        break;
    }
}
void BsTitleBarCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(2, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsTitleBarTask;
            w->trans = BsTitleBarSprTrans;
            w->x34 = bsSys->x1E;
            w->x38 = 0x1AC - bsSys->x1C;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = (0x280 - bsSys->x1E) - bsSys->x20;
            w->x44 = 20.0f;
            w->x06 = 0;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            BsTtlBarReq = 1;
        }
    }
}
void BsTitleBarTask(BSWK *w) {
    u8 r;
    r = BsTtlBarReq;
    switch (r) {
    case 4:
        w->x06 = 0;
        break;
    case 1:
        w->x06 = 1;
        break;
    case 2:
        w->x06 = 2;
        break;
    case 3:
        w->x06 = 3;
        break;
    case 5:
        w->x06 = 0x63;
        break;
    }
}
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
void BsSoftKbdCharSet(int a) {
    BSWK *w;
    switch (a) {
    case 1:
        w = BsWorkPull(4, 0);
        if (w != 0) {
            w->x01 = 1;
            w->x00 = 1;
            w->task = BsSoftKbdTask;
            w->trans = BsSoftKbdSprTrans;
            w->x34 = 0;
            w->x38 = 0;
            w->x4C = 0;
            w->x50 = 0;
            w->x40 = 0;
            w->x44 = 0;
            w->x06 = 1;
            w->x07 = 0;
            w->x0A = 0;
            w->x0C = 0;
            w->x0E = 0;
            w->x10 = 0;
            BsSoftKbdReq = 2;
        }
    }
}
void BsSoftKbdTask(BSWK *w) {
    u8 r;
    r = BsSoftKbdReq;
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
int SoftKeyboardInitialize(char *s, int n) {
    int v;
    v = n;
    if ((u32)(s16)n >= 0x100U) {
        v = 0xFF;
    }
    if (strlen(s) >= 0x100U) {
        strcpy(inputStrBuf, lit_880_00666050);
    } else {
        strcpy(inputStrBuf, s);
    }
    return SoftKeyboard_set(0 & 0xFF, v & 0xFFFF, s);
}
char *SoftKeyboard(void) {
    s8 sx;
    flfntSetSize(0x16, 0x16);
    if ((sx = SoftKeyboard_move(inputStrBuf, BsPsw, *(s16 *)0x3F3714)) != 0) {
        return inputStrBuf;
    }
    return 0;
}
