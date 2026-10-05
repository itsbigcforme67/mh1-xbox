/* lb_z120 - auto-drafted 0x0060E2B0-0x0060E32C: Eft25_set (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char eft25_move[];

void Eft25_set(int arg0, s8 arg1) {
    int temp_v0;

    if (Pl_stg_ck() & 0xFF) {
        temp_v0 = pull_eft_work(1);
        if (temp_v0 != 0) {
            F(s8, temp_v0, 2) = 0x19;
            F(int, temp_v0, 0x20) = (int)&eft25_move;
            F(s32, temp_v0, 0x14) = 0;
            F(int, temp_v0, 0x34) = arg0;
            F(s8, temp_v0, 3) = arg1;
            F(s16, temp_v0, 0xA) = (s16) F(s32, arg0, 0xA4);
            F(s32, temp_v0, 0x38) = 0;
            F(s32, temp_v0, 0x30) = 0x3F800000;
        }
    }
}
