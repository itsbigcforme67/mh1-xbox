/* G online (PC): lobby_func_005AE650, written from Monster Hunter G's code at lobby 0x5AE650 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005AE760();
s32 lobby_func_005AE790();
s32 lobby_func_005AE8E0();
s32 lobby_func_005AE930();
s32 lobby_func_005AE980();
void lobby_func_005AE650(void *arg0) {
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_a1) {                              /* irregular */
    case 0:
        lobby_func_005AE760(arg0);
        return;
    case 1:
        lobby_func_005AE790(arg0);
        return;
    case 2:
        lobby_func_005AE8E0(arg0);
        return;
    case 3:
        lobby_func_005AE930(arg0);
        return;
    case 4:
        lobby_func_005AE980(arg0);
        return;
    }
}

