/* lb_gak01 - near-match fixes 0x005E1190-0x005E128C: stockPlainText. Whole file in lb_ak.c. */
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

int stockPlainText(int x, int y, int a2, int a3, int x2, int y2, char *text, int t3) {
    int v;
    if ((v = strcmp(text, lit_288_00665FB8)) == 0 || (v = strcmp(text, lit_289_00665FC0)) == 0) {
        return v;
    }
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    return AppendWork(1, 1, x, y, x2, y2, a2, a3, t3, 0, text, lit_270_00665FB0, lit_270_00665FB0);
}
