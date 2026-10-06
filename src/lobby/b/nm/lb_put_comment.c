#include "lobby_a.h"

void lb_put_comment(int arg0, int arg1, int arg2, int arg3) {
    int temp_s0;
    int temp_s1;

    if (((s8)arg3) != 0) {
        temp_s1 =  (arg1 << 0x30) >> 0x30;
        temp_s0 =  (arg0 << 0x30) >> 0x30;
        Draw_square( ((temp_s0 - 6) << 0x30) >> 0x30,  ((temp_s1 - 2) << 0x30) >> 0x30, 0x12C, 0x42);
        Draw_square( ((temp_s0 - 7) << 0x30) >> 0x30,  ((temp_s1 - 3) << 0x30) >> 0x30, 0x12E, 0x44);
    }
    KinshiYogo_chk(arg2);
    Put_comment(arg0,  ((( (arg1 << 0x30) >> 0x30) - 0x16) << 0x30) >> 0x30, 0x16, arg2);
}
