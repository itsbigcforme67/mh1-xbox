#include "lobby_b.h"
extern char room_member_id[];
extern char room_member_handle[];
extern char room_member_mini_data[];
void CallBack_Event_MatchStart(CNET_RES res) {
    int var_s0;
    int var_s1;
    int var_s2;
    s32 var_s3;
    int var_s4;

    F(s8, (u8 *)cw, 0x2C0C) = 1;
    Lbc_set_prim(0, 0, 0);
    *(s8 *)0x3F3404 = 0;
    Lb_send_stage();
    fade_set(0xA);
    var_s4 = 0;
    var_s3 = 0;
    var_s2 = (int)&room_member_id;
    var_s1 = (int)&room_member_handle;
    var_s0 = (int)&room_member_mini_data;
    do {
        memcpy(var_s2, (u8 *)cw + var_s3 + 0x73C, 8);
        memcpy(var_s1, (u8 *)cw + var_s3 + 0x744, 0x11);
        memcpy(var_s0, (u8 *)cw + var_s3 + 0x756, 0x40);
        var_s3 += 0x2FC;
        var_s2 += 8;
        var_s4 =  ((var_s4 + 1) << 0x38) >> 0x38;
        var_s1 += 0x11;
        var_s0 += 0x40;
    } while (var_s4 < 4);
}
