/* lb_ak09 - browser stock* recorders 0x005E2C00-0x005E2CB8: UpdateEndpoint. Whole file in lb_ak.c. */
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
