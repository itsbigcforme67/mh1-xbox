#include "lobby_a.h"
extern char CnetWork[];
extern char CnetWork[];
extern char lit_291_0065EC10[];
void net_time_str(void) {
    char sp30[0x40000000];
    s32 var_a3;
    s32 var_s0;
    s32 var_s1;

    var_s1 = (F(u32, &CnetWork, 8) / 216000) + ((u32) F(u32, &CnetWork, 8) >> 0x1F);
    var_a3 = 0x3B;
    if (var_s1 >= 0x64) {
        var_s1 = 0x63;
    } else {
        var_a3 = ((F(u32, &CnetWork, 8) % 216000) / 3600) + ((u32) (F(u32, &CnetWork, 8) % 216000) >> 0x1F);
    }
    sprintf(sp30, &lit_291_0065EC10, (u32) var_s1, var_a3);
    var_s0 = 1;
    if (((var_s1 / 10) + ((u32) var_s1 >> 0x1F)) > 0) {
        var_s0 = 2;
    }
    cnWrap_SetFontSize(0x41A00000);
    cnWrap_SetFontColor(0);
    cnWrap_FontDisp(510.0f - ((11.0f * (f32) (8 - (var_s0 + 6))) / 2.0f), 0x3F800000, sp30, 0x43FF0000, 0x40000000);
}
