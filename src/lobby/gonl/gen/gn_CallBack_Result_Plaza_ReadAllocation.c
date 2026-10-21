/* G online (PC): CallBack_Result_Plaza_ReadAllocation, written from Monster Hunter G's code at lobby 0x57DC80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_ServerMessage();
s32 lobby_func_005E3420();
s32 lobby_func_005E36A0();
s32 lobby_func_005E36D0();
s32 lobby_func_005E3900();
extern u8 ClassInfo;
extern u8 cw;
s32 cnLBS_Get_LobbyName();           /* extern */
s32 cnLBS_Get_RoomStatus();          /* extern */
s32 cnLBS_Get_mhRoomJoinUser(); /* extern */
extern u8 PlazaInfo;

void CallBack_Result_Plaza_ReadAllocation(s64 arg0) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp48 (*(s64 *)(g_frame + 0x48))
    u8 *var_s1;   /* (s32 *) */
    s16 temp_s0;
    s32 var_s2;
    u8 *temp_a1;
    u8 *temp_v1;

    temp_a1 = (*(void **)&cw);
    sp48 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 9) && ((s8) sp48 != 2)) {
        if ((s8) sp48 == 0) {
            (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
            temp_v1 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9C)) + 1);
            lobby_func_005E3420(&(*(u16 *)((u8 *)&ClassInfo + 0x2)));
            var_s2 = 0;
            if ((s32) (*(u16 *)((u8 *)&ClassInfo + 0x2)) > 0) {
                var_s1 = &PlazaInfo;
                do {
                    temp_s0 = (var_s2 & 0xFFFF) + 1;
                    (*(s16 *)((u8 *)(var_s1) + 4)) = temp_s0;
                    cnLBS_Get_RoomStatus(temp_s0 & 0xFFFF, var_s1 + 0x16);
                    cnLBS_Get_mhRoomJoinUser(temp_s0 & 0xFFFF, var_s1 + 6, var_s1 + 0x12);
                    cnLBS_Get_LobbyName(temp_s0 & 0xFFFF, var_s1 + 0x1A);
                    lobby_func_005E3900(temp_s0 & 0xFFFF, (s32) (var_s1 + 0x5B));
                    lobby_func_005E36A0(temp_s0 & 0xFFFF, var_s1 + 8);
                    lobby_func_005E36D0(temp_s0 & 0xFFFF, var_s1 + 0x14);
                    var_s2 = (var_s2 + 1) & 0xFFFF;
                    var_s1 += 0x15C;
                } while (var_s2 < (s32) (*(u16 *)((u8 *)&ClassInfo + 0x2)));
            }
        } else {
            (*(u8 *)((u8 *)(temp_a1) + 0xB9C)) = 0x63U;
            cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
        }
    }
}

