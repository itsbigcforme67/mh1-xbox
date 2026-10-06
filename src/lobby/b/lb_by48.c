/* lb_by48 - agent B promoted near-match 0x00539E40-0x00539F7C: lb_armor2_sel2Prog (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern char shop_process2_help[];

s32 lb_armor2_sel2Prog(void) {
    if (lbShop.key & 0x20) {
        if (lbShop.x78 == 0) {
            cnWrap_SoundRequest(0);
            lbShop.f38 = 0;
            lbShop.help = F(s32, &shop_process2_help, 0x10);
            return 0;
        }
        cnWrap_SoundRequest(3);
        lbShop.help = F(s32, &shop_process2_help, 8);
        return 3;
    }
    if (lbShop.key & 0x40) {
        if (lbShop.x78 != 1) {
            cnWrap_SoundRequest(3);
            lbShop.x78 = 1;
            goto block_16;
        }
        cnWrap_SoundRequest(3);
        lbShop.help = F(s32, &shop_process2_help, 8);
        return 3;
    }
    if (lbShop.key & 0x400) {
        if (lbShop.x78 != 1) {
            cnWrap_SoundRequest(1);
            lbShop.x78 = 1;
        }
    } else if ((lbShop.key & 0x800) && (lbShop.x78 != 0)) {
        cnWrap_SoundRequest(1);
        lbShop.x78 = 0;
    }
block_16:
    return 2;
}
