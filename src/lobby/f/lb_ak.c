/* Lobby browser: stock* page-object recorders (AppendWork wrappers), endpoint tracking, hand-written from asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern s8 BsBgImgReq;
extern char lit_270_00665FB0[];
extern char lit_288_00665FB8[];
extern char lit_289_00665FC0[];
extern char lit_548_00665FC8[];
u32 strlen();
int strcmp();
char *strcpy();
int sprintf(char *, const char *, ...);
int skipShadowImage();
void setUpDnLtRtBlank();
void UpdateEndpoint(int x, int y);
int AppendWork(int, int, int, int, int, int, int, int, int, int, void *, void *, void *);
void UpdateEndpoint(int x, int y) {
    if (bsSys->x10 < y) {
        bsSys->x10 = y;
        bsSys->x22 = 0x20490 / bsSys->x10;
        if (bsSys->x10 > 0x17C) {
            bsSys->x2A = 1;
        }
    }
    if (x >= bsSys->x0C) {
        bsSys->x0C = x;
        bsSys->x26 = 0x4EB40 / bsSys->x0C;
        if (bsSys->x0C > 0x248) {
            bsSys->x2B = 1;
        }
    }
}
void stockDrawEndpoint(int x, int y) {
    UpdateEndpoint(x & 0xFFFF, y & 0xFFFF);
}
int stockTitle(char *s, int b, int c) {
    if (strlen(s) > 0x3C) {
        s[0x38] = 0x20;
        s[0x39] = 0x2E;
        s[0x3A] = 0x2E;
        s[0x3B] = 0x2E;
        s[0x3C] = 0;
    }
    return AppendWork(0, 1, 0x10, 0x1AC, 0, 0, 0xFF000001, 0x14, 0, 0, s, lit_270_00665FB0, lit_270_00665FB0);
}
int stockPlainText(int x, int y, int a2, int a3, int x2, int y2, char *text, int t3) {
    int v;
    if ((v = strcmp(text, lit_288_00665FB8)) == 0 || (v = strcmp(text, lit_289_00665FC0)) == 0) {
        return v;
    }
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(1, 1, x, y, x2, y2, a2, a3, t3, 0, text, lit_270_00665FB0, lit_270_00665FB0);
}
int stockLinkText(int x, int y, int c1, int c2, int x2, int y2, char *s6, char *s7, u8 a8) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(2, 1, x, y, x2, y2, c1, c2, a8, 0, s7, s6, lit_270_00665FB0);
}
int stockSendButton(int x, int y, int x2, int y2, char *s) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(3, 1, x, y, x2, y2, 0xFF000001, 0x10, 0, 0, s, lit_270_00665FB0, lit_270_00665FB0);
}
int stockResetButton(int x, int y, int x2, int y2, char *s) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(4, 1, x, y, x2, y2, 0xFF000001, 0x10, 0, 0, s, lit_270_00665FB0, lit_270_00665FB0);
}
void stockButtonImage(u16 x, u16 y, u16 x2, u16 y2, void *t0, void *t1, void *t2) {
    if (x2 - x <= 0) {
        return;
    }
    if (y2 - y <= 0) {
        return;
    }
    if (skipShadowImage() != 0) {
        return;
    }
    UpdateEndpoint(x2, y2);
    AppendWork(0xD, 5, x, y, x2, y2, 0xFF000001, 0x10, 0, 0, t0, t1, t2);
}
int stockSpButton(int x, int y, int x2, int y2, char *s) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(5, 1, x, y, x2, y2, 0xFF000001, 0x10, 0, 0, s, lit_270_00665FB0, lit_270_00665FB0);
}
int stockTextField(int x, int y, int x2, int y2, void *t0, char *str, int len, int kind) {
    u16 n;
    u16 ln;
    ln = len;
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    if ((kind & 0xFF) > 0 && (kind & 0xFF) < 4 && ln > 0x10) {
        ln = 0x10;
    }
    n = ln;
    if (n < strlen(str)) {
        str = lit_270_00665FB0;
    }
    if (n > 0xFF) {
        ln = 0xFF;
    }
    len = ln;
    return AppendWork(6, 1, x, y, x2, y2, 0xFF000001, 0x10, kind, len & 0xFF, t0, str, lit_270_00665FB0);
}
int stockPassField(int x, int y, int x2, int y2, void *t0, char *str, int len, int kind) {
    u16 n;
    u16 ln;
    ln = len;
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    if ((kind & 0xFF) > 0 && (kind & 0xFF) < 4 && ln > 0x10) {
        ln = 0x10;
    }
    n = ln;
    if (n < strlen(str)) {
        str = lit_270_00665FB0;
    }
    if (n > 0xFF) {
        ln = 0xFF;
    }
    len = ln;
    return AppendWork(7, 1, x, y, x2, y2, 0xFF000001, 0x10, kind, len & 0xFF, t0, str, lit_270_00665FB0);
}
int stockStartPulldown(void *a) {
    return AppendWork(8, 0xA, 0, 0, 0, 0, 0xFF000001, 0x10, 0, 0, lit_270_00665FB0, a, lit_270_00665FB0);
}
int stockMiddlePulldown(void *a, void *b, int c) {
    return AppendWork(8, 0x14, 0, 0, 0, 0, 0xFF000001, 0x10, 0, c, a, b, lit_270_00665FB0);
}
int stockEndPulldown(int x, int y, int x2, int y2) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(8, 0x1E, x, y, x2, y2, 0xFF000001, 0x10, 0, 0, lit_270_00665FB0, lit_270_00665FB0, lit_270_00665FB0);
}
int stockHorizon(int x, int y, int x2, int y2) {
    return AppendWork(0xA, 1, x, y, x2, y2, 0xFF000001, 0, 0, 0, lit_270_00665FB0, lit_270_00665FB0, lit_270_00665FB0);
}
int stockRadioButton(int x, int y, int a2, int a3, void *t0, int t1) {
    int x2;
    int y2;
    y2 = (y & 0xFFFF) + 0x14;
    x2 = (x & 0xFFFF) + 0x14;
    UpdateEndpoint(x2, y2);
    return AppendWork(0xB, 1, x, y, x2 & 0xFFFF, y2 & 0xFFFF, 0xFF000001, 0, t1, a2, (void *)a3, t0, lit_270_00665FB0);
}
int stockCheckBox(int x, int y, int a2, int a3, void *t0, int t1) {
    int x2;
    int y2;
    y2 = (y & 0xFFFF) + 0x14;
    x2 = (x & 0xFFFF) + 0x14;
    UpdateEndpoint(x2, y2);
    return AppendWork(0xC, 1, x, y, x2 & 0xFFFF, y2 & 0xFFFF, 0xFF000001, 0, t1, a2, (void *)a3, t0, lit_270_00665FB0);
}
int stockBgImage(u8 *p) {
    int r;
    r = skipShadowImage();
    if (r == 0) {
        if (*p == 0) {
            BsBgImgReq = 3;
        } else {
            BsBgImgReq = 1;
            r = AppendWork(0xD, 1, 0, 0, 0, 0, 0xFF000001, 0x10, 0, 0, lit_270_00665FB0, p, lit_270_00665FB0);
        }
    }
    return r;
}
void stockImage(int x, int y, int x2, int y2, void *t0, void *t1) {
    if (((x2 & 0xFFFF) - (x & 0xFFFF)) > 0) {
        if (((y2 & 0xFFFF) - (y & 0xFFFF)) <= 0) {
        } else if (skipShadowImage() == 0) {
            UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
            AppendWork(0xD, 2, x, y, x2, y2, 0xFF000001, 0x10, 0, 0, lit_270_00665FB0, t0, t1);
        }
    }
}
void stockLinkImage(int x, int y, int x2, int y2, void *t0, void *t1, void *t2, int t3) {
    if (((x2 & 0xFFFF) - (x & 0xFFFF)) > 0) {
        if (((y2 & 0xFFFF) - (y & 0xFFFF)) <= 0) {
        } else if (skipShadowImage() == 0) {
            UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
            AppendWork(0xD, 3, x, y, x2, y2, 0xFF000001, 0x10, t3, 0, t1, t0, t2);
        }
    }
}
int stockAnchorName(int x, int y, char *name) {
    char buf[0x100];
    memset(buf, 0, 0x100);
    sprintf(buf, lit_548_00665FC8, name);
    return AppendWork(0xE, 1, x, y, 0, 0, 0xFF000001, 0, 0, 0, buf, lit_270_00665FB0, lit_270_00665FB0);
}
void stockTableOutline(int x, int y, int x2, int y2, int t0, int t1, int t2, int t3, u8 *p) {
    int u;
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    if (*p != 0) {
        AppendWork(0xD, 4, x, y, x2, y2, 0xFF000001, t0, 0, 0, lit_270_00665FB0, p, lit_270_00665FB0);
    }
    u = t2 & 0xFF;
    AppendWork(0x10, 1, ((x & 0xFFFF) + u) & 0xFFFF, ((y & 0xFFFF) + u) & 0xFFFF, ((x2 & 0xFFFF) - u) & 0xFFFF, ((y2 & 0xFFFF) - u) & 0xFFFF, t1, t0, t2, 2, lit_270_00665FB0, lit_270_00665FB0, lit_270_00665FB0);
    AppendWork(0x10, 1, x, y, x2, y2, t3, t0, t2, 1, lit_270_00665FB0, lit_270_00665FB0, lit_270_00665FB0);
}
void stockTableInline(int x, int y, int x2, int y2, int t0, int t1, int t2, u8 *p) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    if (*p != 0) {
        AppendWork(0xD, 4, x, y, x2, y2, 0xFF000001, t0, 0, 0, lit_270_00665FB0, p, lit_270_00665FB0);
    }
    AppendWork(0xF, 1, x, y, x2, y2, t1, t0, t2, 0, lit_270_00665FB0, lit_270_00665FB0, lit_270_00665FB0);
}
int stockStrike(int x, int y, int c1, int c2, int x2, int y2, void *t2, char *t3, u8 a8) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(0x11, 1, x, y, x2, y2, c1, c2, a8, 0, t3, t2, lit_270_00665FB0);
}
int stockUnderText(int x, int y, int c1, int c2, int x2, int y2, void *t2, char *t3, u8 a8) {
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(0x12, 1, x, y, x2, y2, c1, c2, a8, 0, t3, t2, lit_270_00665FB0);
}
int stockActionURL(void *a, int b) {
    return AppendWork(0x13, 1, 0, 0, 0, 0, 0xFF000001, 0x10, 0, b, a, lit_270_00665FB0, lit_270_00665FB0);
}
int stockHiddenField(void *a, void *b) {
    return AppendWork(0x14, 1, 0, 0, 0, 0, 0xFF000001, 0x10, 0, 0, a, b, lit_270_00665FB0);
}
void stockShadowPageNum(int n) {
    switch (n & 0xFF) {
    case 1:
    case 4:
    case 5:
        bsSys->x34 = 1;
        break;
    case 6:
    case 7:
    default:
        bsSys->x34 = 0;
        break;
    }
    bsSys->x2F = bsSys->x30;
    bsSys->x30 = n;
}
void stockSystemStyle(int a, char *s, int c) {
    bsSys->x18 = a;
    if (bsSys->x39 != 0) {
        if (((bsSys->x18 & 8) >> 3) == 0) {
            bsSys->x18 = bsSys->x18 + 8;
        }
    }
    strcpy(bsSys->style, s);
    bsSys->x5C3 = c;
    bsSys->x38 = 0x1E;
    bsSys->x37 = 1;
    setUpDnLtRtBlank(c, 0x1E);
}
