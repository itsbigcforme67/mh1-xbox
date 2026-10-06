/* lb_ak05 - browser stock* recorders 0x005E1BF0-0x005E1C78: stockBgImage. Whole file in lb_ak.c. */
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
