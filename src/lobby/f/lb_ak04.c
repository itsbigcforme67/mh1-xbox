/* lb_ak04 - browser stock* recorders 0x005E1890-0x005E1A48: stockStartPulldown, stockMiddlePulldown, stockEndPulldown, stockHorizon. Whole file in lb_ak.c. */
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
