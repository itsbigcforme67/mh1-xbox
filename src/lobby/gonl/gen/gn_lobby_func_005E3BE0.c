/* G online (PC): lobby_func_005E3BE0, written from Monster Hunter G's code at lobby 0x5E3BE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 GetRecvData16();                       /* extern */
s32 GetRecvDataString();            /* extern */
extern u8 recv_work;

void lobby_func_005E3BE0(s32 arg1) {
    GetRecvDataString(arg1, GetRecvData16(&recv_work));
}

