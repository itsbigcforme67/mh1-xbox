/* lb_ar01 - tiny wrappers 0x005D93A0-0x005D93B4: BsParseInitialize. Whole file in lb_ar.c. */
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

void BsParseInitialize(void) {
    ParseReq = 0;
    memset(ParseCk_ret, 0, 4);
}
