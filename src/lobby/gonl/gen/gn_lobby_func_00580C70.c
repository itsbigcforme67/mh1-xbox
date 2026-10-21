/* G online (PC): lobby_func_00580C70, written from Monster Hunter G's code at lobby 0x580C70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Fade_busy_ck();
s32 Lbc_set_prim();
s32 To_LogOut();
s32 cnLbc_CheckInFloorOrder();
s32 cnWrap_InitWork();
s32 fade_set();
s32 lobby_func_00579B90();
s32 lobby_func_0059A270();
s32 lobby_func_0059E270();
s32 str_stop();
extern u8 cw;
void lobby_func_00580C70(void) {
    u8 temp_v0;
    u8 *temp_a0;

    temp_v0 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C));
    switch (temp_v0) {                              /* irregular */
    case 0:
        cnLbc_CheckInFloorOrder(1);
        temp_a0 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) = 1;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF751)) = 0;
        cnWrap_InitWork();
        Lbc_set_prim(0, 0, 0, 0);
        lobby_func_00579B90();
        lobby_func_0059A270();
        str_stop(0);
        str_stop(1);
        if (Fade_busy_ck() & 0xFF) {
            fade_set(2);
            return;
        }
        return;
    case 1:
        lobby_func_0059E270();
        return;
    default:
        To_LogOut(4);
        break;
    }
}

