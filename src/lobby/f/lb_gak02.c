/* lb_gak02 - near-match fixes 0x005E1630-0x005E1888: stockTextField, stockPassField. Whole file in lb_ak.c. */
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

int stockTextField(int x, int y, int x2, int y2, void *t0, char *str, u16 len, int kind) {
    u16 n;
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    if ((kind & 0xFF) > 0 && (kind & 0xFF) < 4 && len > 0x10) {
        len = 0x10;
    }
    n = len;
    if (n < strlen(str)) {
        str = lit_270_00665FB0;
    }
    if (n > 0xFF) {
        len = 0xFF;
    }
    return AppendWork(6, 1, x, y, x2, y2, 0xFF000001, 0x10, kind, len & 0xFF, t0, str, lit_270_00665FB0);
}

int stockPassField(int x, int y, int x2, int y2, void *t0, char *str, u16 len, int kind) {
    u16 n;
    UpdateEndpoint(x2 & 0xFFFF, y2 & 0xFFFF);
    if ((kind & 0xFF) > 0 && (kind & 0xFF) < 4 && len > 0x10) {
        len = 0x10;
    }
    n = len;
    if (n < strlen(str)) {
        str = lit_270_00665FB0;
    }
    if (n > 0xFF) {
        len = 0xFF;
    }
    return AppendWork(7, 1, x, y, x2, y2, 0xFF000001, 0x10, kind, len & 0xFF, t0, str, lit_270_00665FB0);
}
