/* lb_ar06 - tiny wrappers 0x006049E0-0x006049F4: font_data_clear. Whole file in lb_ar.c. */
#include "lobby_f.h"
extern u8 *bsw;
extern u8 ParseCk_ret[4];
extern s8 ParseReq;
extern u8 *Bs_work_free_head;
void CpInetInterfaceProblemEnable();
void *_zlib_calloc();
int inflateInit2_();
int ItemboxWindowX(f32, int);
int ItemboxWindowCursorX(f32, int, int, int);

void font_data_clear(void) {
    memset(bsw + 0x2C3, 0, 0x20);
}
