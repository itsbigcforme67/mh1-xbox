/* lb_ak02 - browser stock* recorders 0x005E1290-0x005E14A0: stockLinkText, stockSendButton, stockResetButton. Whole file in lb_ak.c. */
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
