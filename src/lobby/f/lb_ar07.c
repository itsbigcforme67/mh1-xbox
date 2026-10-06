/* lb_ar07 - tiny wrappers 0x0060BEF0-0x0060BF04: ItemboxWindow. Whole file in lb_ar.c. */
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

int ItemboxWindow(int a) {
    return ItemboxWindowX(306.0f, a);
}
