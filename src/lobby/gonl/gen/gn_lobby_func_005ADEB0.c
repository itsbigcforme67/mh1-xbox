/* G online (PC): lobby_func_005ADEB0, written from Monster Hunter G's code at lobby 0x5ADEB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005ADF90();                    /* extern */
s32 lobby_func_005ADFF0();                    /* extern */
s32 lobby_func_005AE060();                    /* extern */

void lobby_func_005ADEB0(void *arg0) {
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_a1) {                              /* irregular */
    case 0:
        lobby_func_005ADF90(temp_a1);
        return;
    case 1:
        lobby_func_005ADFF0(temp_a1);
        return;
    case 2:
        lobby_func_005AE060(temp_a1);
        return;
    }
}

