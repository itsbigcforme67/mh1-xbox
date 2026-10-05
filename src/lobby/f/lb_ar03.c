/* lb_ar03 - tiny wrappers 0x005E9A20-0x005E9A40: bs_pul_wk. Whole file in lb_ar.c. */
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

u8 *bs_pul_wk(void) {
    u8 *n;
    n = Bs_work_free_head;
    if (n != 0) {
        Bs_work_free_head = *(u8 **)(n + 0x20);
    }
    return n;
}
