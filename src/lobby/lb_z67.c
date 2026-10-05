/* lb_z67 - auto-drafted 0x005FDC70-0x005FDDC0: tagAct_001, tagAct_040, tagAct_047, tagAct_002, tagAct_210 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;
extern char lit_457_006677A0[];
extern char lit_458_006677A8[];

s32 tagAct_001(int arg0, s8 *arg1) {
    F(s32, bsw, 4) = 0;
    *arg1 = 0;
    F(s16, bsw, 0xD8C4) = 0;
    F(s16, bsw, 0xD8C6) = 0;
    F(s16, bsw, 0xD8C0) = 0;
    F(s16, bsw, 0xD8BC) = 0;
    F(s16, bsw, 0xD8C2) = 0;
    F(s16, bsw, 0xD8BE) = 0;
    F(s8, bsw, 0xD8CC) = 0;
    F(s32, bsw, 0x1C) = 0;
    strcpy(bsw + 0x2A3, &lit_457_006677A0);
    strcpy(bsw + 0x2B3, &lit_458_006677A8);
    return 0;
}

s32 tagAct_040(int arg0, int arg1) {
    tagoutprintf4(arg1);
    return 0;
}

s32 tagAct_047(int arg0, int arg1) {
    tagoutprintf4(arg1);
    return 0;
}

s32 tagAct_002(int arg0, s8 *arg1) {
    F(s32, bsw, 4) = 0;
    *arg1 = 0;
    return 0;
}

s32 tagAct_210(int arg0, int arg1) {
    Disp_Title(arg1);
    return 0;
}
