/* lb_z86 - auto-drafted 0x005FF510-0x005FF580: tagAct_193, tagAct_032 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 bsw;
extern char lit_1389_006677C0[];

s32 tagAct_193(void) {
    font_data_clear();
    strcpy(bsw + 0x2D3, &lit_1389_006677C0);
    font_data_set();
    return 0;
}

s32 tagAct_032(int arg0, int arg1) {
    font_data_clear();
    tagoutprintf3(arg1);
    return 0;
}
