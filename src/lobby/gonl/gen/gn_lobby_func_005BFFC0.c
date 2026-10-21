/* G online (PC): lobby_func_005BFFC0, written from Monster Hunter G's code at lobby 0x5BFFC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnetSub_Get_RestBgWork();
s32 cnLBS_Read_FileDownloadData();
s32 lobby_func_005C00A0();
extern u8 g_lb_731163;
extern u32 g_lb_731168;

void lobby_func_005BFFC0(void *arg0) {
    s32 var_a2;
    u32 temp_a1;
    u32 temp_v0;

    if (__cnetSub_Get_RestBgWork() > 0) {
        temp_a1 = (*(u32 *)((u8 *)(arg0) + 0xC));
        temp_v0 = temp_a1 + 0x200;
        if (temp_v0 < (u32) g_lb_731168) {
            (*(u32 *)((u8 *)(arg0) + 0xC)) = temp_v0;
            var_a2 = 0x200;
        } else {
            var_a2 = g_lb_731168 - temp_a1;
            (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        }
        cnLBS_Read_FileDownloadData((s32) g_lb_731163, (s32) temp_a1, var_a2, lobby_func_005C00A0);
    }
}

