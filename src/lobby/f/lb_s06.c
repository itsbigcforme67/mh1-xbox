/* lb_s06 - browser tag handlers (struct-array indexing) 0x00600850-0x006009AC: tagAct_051, tagAct_324 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_051(s32 arg0, s32 arg1) {
    s32 temp_v0;
    int temp_a0;

    if (chack_TableTagClose(arg0, arg1, 2) < 0) {
        return -1;
    }
    F(s32, bsw, 4) = 0;
    (*(u8 *)arg1) = 0;
    init_tr_data();
    if (SetTableData(2) < 0) {
        return -1;
    }
    temp_a0 = bsw;
    if (F(s8, temp_a0, 0x186) != -0xA) {
        temp_v0 = BSC(s32, temp_a0, F(u16, temp_a0, 0xD894), 0x24F4);
        if (temp_v0 != 0) {
            (*(int *)arg0) = temp_v0;
            F(s32, bsw, 8) = 2;
            tagAct_320(arg0, arg1);
        }
    }
    return 0;
}

s32 tagAct_324(s32 arg0, s32 arg1) {
    if (chack_TableTagClose(arg0, arg1, 2) < 0) {
        return -1;
    }
    F(s32, bsw, 4) = 0;
    (*(u8 *)arg1) = 0;
    init_tr_data();
    if (SetTableData(2) < 0) {
        return -1;
    }
    tagAct_320(arg0, arg1);
    return 0;
}
