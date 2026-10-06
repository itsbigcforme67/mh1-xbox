/* lb_ak07 - browser stock* recorders 0x005E20B0-0x005E24A0: stockTableInline, stockStrike, stockUnderText, stockActionURL, stockHiddenField, stockShadowPageNum. Whole file in lb_ak.c. */
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
