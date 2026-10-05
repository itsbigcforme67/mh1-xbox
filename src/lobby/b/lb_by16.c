/* lb_by16 - agent B promoted near-match 0x005BACB0-0x005BAD4C: CallBack_Result_Plaza_LobbyMember2 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char tl_member_buff[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x13]; u8 x2C45; } CWS_LobbyMember2;

void CallBack_Result_Plaza_LobbyMember2(CNET_RES res) {
    s32 var_s1;
    int var_s0;
    int temp_a0;

    if ((((CWS_LobbyMember2 *)cw)->x2C31 != 5) && (((CWS_LobbyMember2 *)cw)->x2C45 == 0xA)) {
        ((CWS_LobbyMember2 *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            var_s1 = 0;
            var_s0 = (int)&tl_member_buff;
            do {
                cnLBS_Get_LobbyMemberList(var_s1 & 0xFFFF, var_s0 + 0x280, var_s0 + 0x288, var_s0 + 0x29A);
                var_s1 += 1;
                var_s0 += 0x2FC;
            } while (var_s1 < 8);
        }
        temp_a0 = (int)pNet;
        F(u8, temp_a0, 0x12) = (u8) (F(u8, temp_a0, 0x12) + 1);
    }
}
