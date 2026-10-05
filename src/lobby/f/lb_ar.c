/* Lobby: tiny wrappers and helpers, hand-written from asm. */
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
void *_png_malloc(int a) {
    return _zlib_calloc(0, 1, a);
}
void BsParseInitialize(void) {
    ParseReq = 0;
    memset(ParseCk_ret, 0, 4);
}
void font_data_clear(void) {
    memset(bsw + 0x2C3, 0, 0x20);
}
int inflateInit_(int z, int version, int size) {
    return inflateInit2_(z, 15, version, size);
}
int ItemboxWindow(int a) {
    return ItemboxWindowX(306.0f, a);
}
int ItemboxWindowCursor(int a, int b, int c) {
    return ItemboxWindowCursorX(306.0f, a, b, c);
}
u8 *bs_pul_wk(void) {
    u8 *n;
    n = Bs_work_free_head;
    if (n != 0) {
        Bs_work_free_head = *(u8 **)(n + 0x20);
    }
    return n;
}
