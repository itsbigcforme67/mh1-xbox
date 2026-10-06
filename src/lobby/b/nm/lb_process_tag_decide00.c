#include "lobby_s.h"
extern s32 armorIndex;
extern char shop_process01_tag[];
extern char shop_process01_tag[];
void lb_process_tag_decide00(void) {
    if (lbShop.mode == 0) {
        lbShop.tag = (void *)shop_process01_tag;
        lbShop.x17 = 2;
    } else {
        lbShop.tag = (int *) ((int)&shop_process01_tag + 8);
        lbShop.x17 = 5;
    }
    lbShop.x70 = 0;
    armorIndex = 0;
    lbShop.f40 = (void (*)())0;
}
