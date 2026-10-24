/* G online (PC): CallBack_Result_Lobby_ReadRoomAllocation, written from Monster Hunter G's code at lobby 0x581970 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_eat_to_bell();
s32 cnLBS_Get_ServerMessage();
extern u8 ClassInfo;
extern u8 cw;
s32 cnLBS_Get_AllocationProgressCount(); /* extern */
s32 cnLBS_Get_LobbyCount(); /* extern */
s32 gnet_get_room_status();         /* extern */
s32 cnLBS_Get_RoomExplain();         /* extern */
s32 cnLBS_Get_RoomJoinInfo(); /* extern */
s32 gnet_get_room_password();    /* extern */
s32 cnLBS_Get_RoomProperty();     /* extern */
s32 gnet_get_room_joinuser(); /* extern */
extern u16 g_lb_6DE120;

void CallBack_Result_Lobby_ReadRoomAllocation(s64 arg0) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s64 *)(g_frame + 0x30))
#define sp3C (*(s32 *)(g_frame + 0x3C))
    u8 *var_s0;   /* (s32 *) */
    s16 temp_v0;
    s32 var_s1;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_v1;

    temp_a2 = (*(void **)&cw);
    sp30 = arg0;
    if (((*(u8 *)((u8 *)(temp_a2) + 0xB99)) != 5) && (temp_a3 = temp_a2 + 0xBA9, ((*(u8 *)((u8 *)(temp_a2) + 0xBA9)) == 0x1E))) {
        if ((s8) sp30 == 2) {
            if ((((s8) ((s32) (((s8 *)&sp30)[1])))) == 0xB) {
                cnLBS_Get_AllocationProgressCount(&sp3C, (((s8 *)&sp30)[1]), temp_a2, temp_a3);
                return;
            }
            return;
        }
        if ((s8) sp30 == 0) {
            (*(u8 *)((u8 *)(temp_a2) + 0xBA9)) = 0U;
            temp_v1 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9D)) + 1);
            cnLBS_Get_LobbyCount(&(*(u16 *)((u8 *)&ClassInfo + 0xA)), (((s8 *)&sp30)[1]), temp_a2, temp_a3);
            var_s1 = 0;
            if ((s32) (*(u16 *)((u8 *)&ClassInfo + 0xA)) > 0) {
                var_s0 = &g_lb_6DE120;
                do {
                    temp_v0 = var_s1 + 1;
                    (*(s16 *)((u8 *)(var_s0) + 4)) = temp_v0;
                    gnet_get_room_status(temp_v0 & 0xFFFF, var_s0 + 0x16);
                    gnet_get_room_joinuser((var_s1 + 1) & 0xFFFF, var_s0 + 6, var_s0 + 0x12);
                    cnLBS_Get_RoomJoinInfo((var_s1 + 1) & 0xFFFF, var_s0 + 8, var_s0 + 0xA, var_s0 + 0xC, var_s0 + 0xE, var_s0 + 0x10);
                    gnet_get_room_password((var_s1 + 1) & 0xFFFF, var_s0 + 0x17);
                    cnLBS_Get_RoomProperty((var_s1 + 1) & 0xFFFF, var_s0);
                    cnLBS_Get_RoomExplain((var_s1 + 1) & 0xFFFF, var_s0 + 0x5B);
                    var_s1 += 1;
                    var_s0 += 0x15C;
                } while (var_s1 < (s32) (*(u16 *)((u8 *)&ClassInfo + 0xA)));
            }
        } else {
            (*(u8 *)((u8 *)(temp_a2) + 0xB9D)) = 3U;
            cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
    }
}

