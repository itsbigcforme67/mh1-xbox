#include "lobby_a.h"
extern char lit_3397[];
extern char lit_3399[];
void put_main_cursor2(int arg0, int arg1, s32 arg2) {
    s16 sp26;
    s16 sp24;
    s16 sp22;
    s16 sp20;
    s16 sp16;
    s16 sp14;
    s16 sp12;
    s16 sp10;
    int temp_v1;

    F(long long, &sp20, 0) =  F(long long, &lit_3397, 0);
    temp_v1 =  (arg0 << 0x30) >> 0x30;
    F(f32, &sp20, 8) = (f32) F(f32, &lit_3397, 8);
    sp20 = temp_v1 + 4;
    sp24 = temp_v1 + 0x190;
    F(long long, &sp10, 0) =  F(long long, &lit_3399, 0);
    F(f32, &sp10, 8) = (f32) F(f32, &lit_3399, 8);
    sp14 = temp_v1 + 0x193;
    sp10 = temp_v1 + 1;
    if (F(u8, pNet, 0xC) == 0) {
        sp22 = ( (arg1 << 0x30) >> 0x30) + 0x3C + (arg2 * 0x16);
        sp26 = sp22 + 0x16;
        sp12 = sp22 - 2;
        sp16 = sp26 + 2;
        Put_F(&sp10, &sp26, &sp22);
        Put_F(&sp20);
    }
}
