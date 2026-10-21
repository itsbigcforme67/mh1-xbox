/* G online (PC): lobby_func_005BFC30, written from Monster Hunter G's code at lobby 0x5BFC30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnetSub_Get_RestBgWork();
s32 lobby_func_005C0400();
extern u8 g_lb_731162;
extern s32 gonl_cb_FileDownload;

void lobby_func_005BFC30(void *arg0) {
    u8 temp_s1;

    temp_s1 = (*(u8 *)((u8 *)(arg0) + 0xC));
    if (__cnetSub_Get_RestBgWork() > 0) {
        lobby_func_005C0400((s32) temp_s1, (s32) &gonl_cb_FileDownload, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(u8 *)((u8 *)(arg0) + 0xC)) = (s32) ((s32) (*(u8 *)((u8 *)(arg0) + 0xC)) + 1);
        if ((((temp_s1 & 0xFF) + 1) & 0xFF) >= (s32) g_lb_731162) {
            (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        }
    }
}

