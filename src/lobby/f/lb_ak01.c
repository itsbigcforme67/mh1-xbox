/* lb_ak01 - browser stock* recorders 0x005E1100-0x005E118C: stockTitle. Whole file in lb_ak.c. */
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
