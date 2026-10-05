/* lb_ak06 - browser stock* recorders 0x005E1E70-0x005E1F14: stockAnchorName. Whole file in lb_ak.c. */
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

int stockAnchorName(int x, int y, char *name) {
    char buf[0x100];
    memset(buf, 0, 0x100);
    sprintf(buf, lit_548_00665FC8, name);
    return AppendWork(0xE, 1, x, y, 0, 0, 0xFF000001, 0, 0, 0, buf, lit_270_00665FB0, lit_270_00665FB0);
}
