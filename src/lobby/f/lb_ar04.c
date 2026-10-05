/* lb_ar04 - tiny wrappers 0x005EC3C0-0x005EC3D4: inflateInit_. Whole file in lb_ar.c. */
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

int inflateInit_(int z, int version, int size) {
    return inflateInit2_(z, 15, version, size);
}
