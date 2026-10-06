/* lb_gz03 - browser table/tag handlers 0x005F6B30-0x005F6C50: RequestAllImages (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsSys;
extern char bsOW[];

void RequestAllImages(void) {
    int sp4C;
    s32 var_s2;
    s32 var_s1;
    int temp_a1;
    int var_s0;
    int temp_a0;
    int temp_a2;

    var_s2 = 0;
    var_s1 = 0;
    var_s0 = (int)&bsOW;
loop_1:
    temp_a2 = (*(int *)var_s0);
    if ((temp_a2 != 0) && (F(u8, temp_a2, 0) != 0)) {
        if (F(u8, temp_a2, 2) == 0xD) {
            if (F(u8, bsSys, 0x2D) == 0x14) {
                F(s8, temp_a2, 5) = 3;
            } else {
                temp_a1 = F(int, temp_a2, 0x64);
                if ((*(u8 *)temp_a1) == 0) {
                    F(s8, temp_a2, 5) = 2;
                } else {
                    var_s1 = (var_s1 + 1) & 0xFF;
                    BsUrlSet(&sp4C, temp_a1, temp_a2);
                    BsRequestImage(&sp4C);
                    F(s8, (*(int *)var_s0), 5) = 1;
                    temp_a0 = bsSys;
                    F(u8, temp_a0, 0x2D) = (u8) (F(u8, temp_a0, 0x2D) + 1);
                }
            }
        }
        var_s2 = (var_s2 + 1) & 0xFFFF;
        var_s0 += 4;
        if (var_s2 >= 0x1F4) {

        } else {
            goto loop_1;
        }
    }
    if ((var_s1 & 0xFF) > 0) {
        F(s8, bsSys, 0x2E) = 8;
        return;
    }
    F(s8, bsSys, 0x2E) = 0xA;
}
