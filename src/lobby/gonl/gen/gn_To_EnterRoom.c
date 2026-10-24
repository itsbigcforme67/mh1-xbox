/* G online (PC): To_EnterRoom, written from Monster Hunter G's code at lobby 0x5833D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_InRoom00_EntryJoinUser();
s32 CallBack_Result_InRoom00_JoinUser();
s32 CallBack_Result_InRoom00_Member();
extern u8 ClassInfo;
extern u8 cw;
s32 Init_InterruptFlag();                       /* extern */
s32 gnet_read_6403(); /* extern */
s32 cnLBS_Read_RoomMemberList(); /* extern */
s32 lobby_func_005BF190(); /* extern */

void To_EnterRoom(void) {
    u8 temp_s0;

    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    Init_InterruptFlag();
    (*(s16 *)((u8 *)((*(void **)&cw)) + 0xF754)) = 0;
    (*(s16 *)((u8 *)((*(void **)&cw)) + 0xF752)) = 0;
    (*(s16 *)((u8 *)((*(void **)&cw)) + 0xF756)) = 0;
    temp_s0 = (*(u8 *)((u8 *)&ClassInfo + 0x8));
    cnLBS_Read_RoomMemberList(temp_s0, CallBack_Result_InRoom00_Member);
    gnet_read_6403(temp_s0 & 0xFFFF, CallBack_Result_InRoom00_JoinUser);
    lobby_func_005BF190(temp_s0 & 0xFFFF, CallBack_Result_InRoom00_EntryJoinUser);
}

