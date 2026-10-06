#include "lobby_a.h"
extern char ClassInfo[];
extern char PlazaInfo[];
extern char ClassInfo[];
extern char ClassInfo[];
void lbc_top_menu_01(void) {
    int var_s1;
    s32 var_s0;
    u16 temp_a1;

    F(s8, (u8 *)cw, 0x2C08) = 1;
    temp_a1 = F(u16, &ClassInfo, 2);
    var_s0 = 0;
    if (temp_a1 > 0) {
        var_s1 = (int)&PlazaInfo;
loop_2:
        if (F(u8, var_s1, 0x10) != 3) {
            var_s0 += 1;
            var_s1 += 0x15C;
            if (var_s0 == temp_a1) {
                To_LogOut(1);
            }
            if (var_s0 >= F(u16, &ClassInfo, 2)) {

            } else {
                goto loop_2;
            }
        }
    }
    F(s8, &ClassInfo, 0) = (s8) (var_s0 + 1);
    F(s8, (u8 *)cw, 0x2C33) = 2;
    F(s8, (u8 *)cw, 0x2C34) = 0;
}
