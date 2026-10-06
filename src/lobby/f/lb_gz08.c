/* lb_gz08 - browser table/tag handlers 0x00600CE0-0x00600F1C: tagAct_052, tagAct_339, tagAct_053, tagAct_349 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_052(int arg0, int arg1) {
    int temp_a1;

    if (chack_TableTagClose(arg0, arg1, 3) < 0) {
        return -1;
    }
    F(s32, bsw, 4) = 0;
    (*(s8 *)arg1) = 0;
    init_td_data();
    if (SetTableData(3) < 0) {
        return -1;
    }
    temp_a1 = bsw;
    {
        int k = F(u16, temp_a1, 0xD894) * 0x5C;
        F(s8, (k + temp_a1), 0x252F) = 1;
    }
    DispFontSize(1, temp_a1);
    return 0;
}

s32 tagAct_339(s32 arg0, int arg1) {
    int temp_a1;

    if (chack_TableTagClose(arg0, arg1, 3) < 0) {
        return -1;
    }
    F(s32, bsw, 4) = 0;
    (*(s8 *)arg1) = 0;
    init_td_data();
    if (SetTableData(3) < 0) {
        return -1;
    }
    temp_a1 = bsw;
    {
        int k = F(u16, temp_a1, 0xD894) * 0x5C;
        F(s8, (k + temp_a1), 0x252F) = 1;
    }
    DispFontSize(1, temp_a1);
    tagAct_340(arg0, arg1);
    return 0;
}

s32 tagAct_053(int arg0, int arg1) {
    if (chack_TableTagClose(arg0, arg1, 4) < 0) {
        return -1;
    }
    F(s32, bsw, 4) = 0;
    (*(s8 *)arg1) = 0;
    init_td_data();
    return -(SetTableData(4) < 0);
}

s32 tagAct_349(s32 arg0, int arg1) {
    if (chack_TableTagClose(arg0, arg1, 4) < 0) {
        return -1;
    }
    F(s32, bsw, 4) = 0;
    (*(s8 *)arg1) = 0;
    init_td_data();
    if (SetTableData(4) < 0) {
        return -1;
    }
    tagAct_340(arg0, arg1);
    return 0;
}
