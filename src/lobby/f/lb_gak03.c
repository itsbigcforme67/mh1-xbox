/* lb_gak03 - near-match fixes 0x005E1A50-0x005E1BEC: stockRadioButton, stockCheckBox. Whole file in lb_ak.c. */
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

int stockRadioButton(int x, int y, int a2, int a3, void *t0, int t1) {
    int y2;
    int x2;
    y2 = (y & 0xFFFF) + 0x14;
    x2 = (x & 0xFFFF) + 0x14;
    UpdateEndpoint(x2, y2);
    return AppendWork(0xB, 1, x, y, x2 & 0xFFFF, y2 & 0xFFFF, 0xFF000001, 0, t1, a2, (void *)a3, t0, lit_270_00665FB0);
}

int stockCheckBox(int x, int y, int a2, int a3, void *t0, int t1) {
    int y2;
    int x2;
    y2 = (y & 0xFFFF) + 0x14;
    x2 = (x & 0xFFFF) + 0x14;
    UpdateEndpoint(x2, y2);
    return AppendWork(0xC, 1, x, y, x2 & 0xFFFF, y2 & 0xFFFF, 0xFF000001, 0, t1, a2, (void *)a3, t0, lit_270_00665FB0);
}
