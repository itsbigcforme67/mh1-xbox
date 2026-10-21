/* G online (PC): lobby_func_005E3120, written from Monster Hunter G's code at lobby 0x5E3120 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E3340();
s32 lobby_func_005E3480();
s32 lobby_func_005E35E0();
s32 lobby_func_005E3740();
s32 lobby_func_005E3880();
extern u8 CnetSys_w;
s32 __cnetSub_Get_RestBgWork();                     /* extern */
s32 cnLBS_Init_LobbyBgProcess();                /* extern */

void lobby_func_005E3120(void *arg0) {
    s32 temp_s0;
    u32 temp_a0;

    if ((*(u8 *)((u8 *)(arg0) + 0x23)) == 2) {
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
        (*(u8 *)((u8 *)(arg0) + 0x22)) = 0U;
        cnLBS_Init_LobbyBgProcess();
        return;
    }
    temp_s0 = (*(s32 *)((u8 *)(arg0) + 0xC));
    if (__cnetSub_Get_RestBgWork() >= (s32) (*(u8 *)((u8 *)(arg0) + 0x24))) {
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 1) {
            lobby_func_005E35E0(temp_s0 & 0xFFFF, (s32) lobby_func_005E3340);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 2) {
            lobby_func_005E3740(temp_s0 & 0xFFFF, (s32) lobby_func_005E3340);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 4) {
            lobby_func_005E3480(temp_s0 & 0xFFFF, (s32) lobby_func_005E3340);
        }
        if ((*(s32 *)((u8 *)(arg0) + 0x18)) & 8) {
            lobby_func_005E3880(temp_s0 & 0xFFFF, (s32) lobby_func_005E3340);
        }
        temp_a0 = temp_s0 + 1;
        (*(s32 *)((u8 *)(arg0) + 0xC)) = (s32) ((*(s32 *)((u8 *)(arg0) + 0xC)) + 1);
        if ((u16) (*(u16 *)((u8 *)&CnetSys_w + 0x43B4)) >= temp_a0) {
            if (temp_a0 >= 0x1BU) {
                goto block_15;
            }
        } else {
block_15:
            (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        }
    }
}

