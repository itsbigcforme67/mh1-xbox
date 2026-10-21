/* G online (PC): lobby_func_005BFBA0, written from Monster Hunter G's code at lobby 0x5BFBA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005C02B0();
extern u8 gonl_cb_FileDownload;

void lobby_func_005BFBA0(void *arg0) {
    (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
    (*(s8 *)((u8 *)(arg0) + 0x23)) = 0;
    lobby_func_005C02B0((s32) &gonl_cb_FileDownload);
}

