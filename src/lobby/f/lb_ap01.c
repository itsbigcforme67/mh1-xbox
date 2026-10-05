/* lb_ap01 - browser url/kanji helpers 0x005E8920-0x005E8934: BsUrlBaseClear. Whole file in lb_ap.c. */
#include "lobby_f.h"
extern u8 BsCacheCurrentBaseUrlstr[];
extern u8 lit_928_00666260[];
u32 strlen();
int strncmp();
int BsUrlSchemeGet();

void BsUrlBaseClear(void) {
    memset(BsCacheCurrentBaseUrlstr, 0, 0x100);
}
