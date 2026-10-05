/* lb_ar02 - tiny wrappers 0x005DB4A0-0x005DB4B0: http_test_12. Whole file in lb_ar.c. */
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

void http_test_12(int a, int b) {
    *(s8 *)(a + 0x68) = 2;
    CpInetInterfaceProblemEnable(1);
}
