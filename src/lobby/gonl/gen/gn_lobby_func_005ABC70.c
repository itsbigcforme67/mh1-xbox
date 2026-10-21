/* G online (PC): lobby_func_005ABC70, written from Monster Hunter G's code at lobby 0x5ABC70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005ABED0();
s32 lobby_func_005AC6B0();
extern u8 CnetSys_w;
s32 __cnetSub_Get_RestBgWork();                     /* extern */
s32 cnLBS_Init_LobbyBgProcess();                /* extern */
s32 cnLBS_Read_PlazaExplain(); /* extern */
s32 cnLBS_Read_PlazaJoinUser(); /* extern */
s32 cnLBS_Read_PlazaName();    /* extern */
s32 cnLBS_Read_PlazaStatus();  /* extern */
s32 cnLBS_Read_RoomProperty(); /* extern */

void lobby_func_005ABC70(void *arg0) {
    s32 temp_s0;
    u32 temp_a0;

    if ((*(u8 *)((u8 *)(arg0) + 0x23)) & 0x80) {
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
        cnLBS_Init_LobbyBgProcess();
        return;
    }
    temp_s0 = (*(s32 *)((u8 *)(arg0) + 0xC));
    if (__cnetSub_Get_RestBgWork() >= (s32) (*(u8 *)((u8 *)(arg0) + 0x24))) {
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 1) {
            cnLBS_Read_PlazaJoinUser(temp_s0 & 0xFFFF, lobby_func_005ABED0);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 2) {
            cnLBS_Read_PlazaStatus(temp_s0 & 0xFFFF, lobby_func_005ABED0);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 4) {
            cnLBS_Read_PlazaName(temp_s0 & 0xFFFF, lobby_func_005ABED0);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 8) {
            cnLBS_Read_PlazaExplain(temp_s0 & 0xFFFF, lobby_func_005ABED0);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 0x10) {
            lobby_func_005AC6B0(temp_s0 & 0xFFFF, (s32) lobby_func_005ABED0);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 0x80) {
            cnLBS_Read_RoomProperty(temp_s0 & 0xFFFF, lobby_func_005ABED0);
        }
        temp_a0 = temp_s0 + 1;
        (*(s32 *)((u8 *)(arg0) + 0xC)) = (s32) ((*(s32 *)((u8 *)(arg0) + 0xC)) + 1);
        if ((u16) (*(u16 *)((u8 *)&CnetSys_w + 0x43B6)) >= temp_a0) {
            if (temp_a0 >= 0x24U) {
                goto block_19;
            }
        } else {
block_19:
            (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        }
    }
}

