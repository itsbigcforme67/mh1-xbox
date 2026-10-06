/* lb_bz89 - lobby UI/client 0x005BCA80-0x005BCAC0: CallBack_Result_Lobby_GuestRoomMember (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_GuestRoomMember;

void CallBack_Result_Lobby_GuestRoomMember(void) {
    if ((((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C45 == 0x14)) {
        ((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C45 = 0U;
        ((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C35 + 1);
    }
}
