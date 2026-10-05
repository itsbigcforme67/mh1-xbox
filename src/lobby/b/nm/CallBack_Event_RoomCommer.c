#include "lobby_a.h"
extern char lit_4548[];
void CallBack_Event_RoomCommer(void) {
    char sp16C[0x40];
    s8 sp158;
    s8 sp150;
    s8 sp14F;
    s8 sp14E;
    s8 sp14D;
    int sp4C;
    char sp30[0x120];
    s32 temp_s0;
    s32 var_a2;
    int temp_a0;
    int var_a1;

    cnLBS_Get_RoomLeaveUser(&sp150);
    temp_a0 = (int)cw;
    var_a2 = 0;
    var_a1 = temp_a0;
loop_1:
    if (F(s8, var_a1, 0x73C) == 0) {
        temp_s0 = var_a2 * 0x2FC;
        strcpy(temp_a0 + temp_s0 + 0x73C, &sp150, var_a2);
        strcpy((u8 *)cw + temp_s0 + 0x744, &sp158);
        memcpy((u8 *)cw + temp_s0 + 0x756, sp16C, 0x40);
    } else {
        var_a2 += 1;
        var_a1 += 0x2FC;
        if (var_a2 < 4) {
            goto loop_1;
        }
    }
    if ((F(u8, (u8 *)cw, 0x35D5) != 0) && (sp158 != 0)) {
        memset(sp30, 0, 0x120);
        sp14F = 6;
        sp14E = 6;
        sp14D = 6;
        sprintf(&sp4C, &lit_4548, &sp158);
        Chat_log_add(0, sp30);
    }
}
