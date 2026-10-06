/* lb_gz06 - browser table/tag handlers 0x006000A0-0x00600120: tagAct_310 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void tagAct_310(void) {
    int temp_a0;
    int temp_s0;
    int temp_v0;
    int temp_v1;

    temp_a0 = bsw;
    temp_v0 = temp_a0 + (F(u16, temp_a0, 0xD894) * 0x5C);
    F(s8, temp_a0, 0x18D) = 0;
    temp_s0 = temp_v0 + 0x24E0;
    DispFontSize(F(u8, temp_v0, 0x252F));
    set_TABLE_data_1st(temp_s0);
    temp_v1 = bsw;
    if (F(s8, temp_v1, 0x186) == 0) {
        pushTableImage(temp_v1 + 0xE10);
    }
    set_TABLE_position(temp_s0);
}
