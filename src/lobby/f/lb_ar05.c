/* lb_ar05 - tiny wrappers 0x005ED930-0x005ED940: _png_malloc. Whole file in lb_ar.c. */
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

void *_png_malloc(int a) {
    return _zlib_calloc(0, 1, a);
}
