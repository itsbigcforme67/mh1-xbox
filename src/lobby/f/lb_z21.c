/* lb_z21 - auto-drafted 0x005E9350-0x005E937C: BsUrlSchemeGet (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char UrlSchemeList[];

s32 BsUrlSchemeGet(s32 arg0) {
    return (bs_url_cmp_list(&UrlSchemeList, arg0) + 1) & 0xFF;
}
