/* G online (PC): Lbc_ReserveRoom, written from Monster Hunter G's code at lobby 0x581C70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 Lb_eat_to_bell();
s32 SetDialogData();
s32 cnLbc_CheckInFloorOrder();
extern u8 ClassInfo;
extern u8 cw;
extern u8 lb_sys;
s32 cnLBS_RoomCreate();           /* extern */
extern u8 CallBack_Result_Lobby_RoomCreate;
extern u8 g_lb_6DE120;
extern u16 RoomInfo;

s32 Lbc_ReserveRoom(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    u8 *var_a0;   /* (s32 *) */
    s8 var_a2;
    u8 temp_a0;
    u8 *temp_a1;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    temp_a1 = temp_v1 + 0xB9D;
    switch (temp_a0) {                              /* irregular */
    case 0:
        var_a2 = 0;
        var_a0 = &g_lb_6DE120;
loop_6:
        if ((*(u8 *)((u8 *)(var_a0) + 0x16)) == 1) {
            (*(s8 *)((u8 *)&ClassInfo + 0x8)) = (s8) (*(u16 *)((u8 *)&RoomInfo + (var_a2 * 0x15C)));
            (*(s8 *)((u8 *)&lb_sys + 0x7E)) = var_a2;
        } else {
            var_a2 += 1;
            var_a0 += 0x15C;
            if (var_a2 < 5) {
                goto loop_6;
            }
        }
        if (var_a2 == 5) {
            SetDialogData(0x57, (s32) temp_a1, (s32) var_a2, arg3);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
            return 1;
        }
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9D)) + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x1F;
        cnLBS_RoomCreate(cnLbc_CheckInFloorOrder(2, (s32) temp_a1, (s32) var_a2, arg3) & 0xFFFF, &CallBack_Result_Lobby_RoomCreate);
block_19:
    default:
        return 2;
    case 1:
        Check_CallBackWait((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        goto block_19;
    case 2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 3:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), (s32) temp_a1, arg2, arg3);
        return 1;
    }
}

