#include "lobby_a.h"
extern char lit_3380[];
extern char lit_3382[];
void put_main_cursor(s32 arg0) {
    s16 sp26;
    s16 sp22;
    int sp20;
    s16 sp16;
    s16 sp12;
    int sp10;

    F(long long, &sp20, 0) =  F(long long, &lit_3380, 0);
    F(f32, &sp20, 8) = (f32) F(f32, &lit_3380, 8);
    F(long long, &sp10, 0) =  F(long long, &lit_3382, 0);
    F(f32, &sp10, 8) = (f32) F(f32, &lit_3382, 8);
    if (F(u8, pNet, 0xC) == 0) {
        sp22 = (arg0 * 0x16) + 0x8C;
        sp26 = sp22 + 0x16;
        sp12 = sp22 - 2;
        sp16 = sp26 + 2;
        Put_F(&sp10, &sp10, &sp22, &sp20);
        Put_F(&sp20);
    }
}
