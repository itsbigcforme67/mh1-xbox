/* G online (PC): lobby_func_0057FC80, written from Monster Hunter G's code at lobby 0x57FC80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 CallBack_Result_Plaza_LobbyEntry();          /* extern */
s32 MH_lobbyClear();                         /* extern */

s32 lobby_func_0057FC80(void) {
    s32 temp_a0;
    s32 temp_v0;
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9E;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9E));
    switch (temp_v1_2) {                            /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9E)) = (u8) (temp_v1_2 + 1);
        MH_lobbyClear(temp_a0);
    default:                                        /* switch 1 */
        return 2;
    case 1:                                         /* switch 1 */
        temp_v0 = CallBack_Result_Plaza_LobbyEntry(temp_a0);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            return 0;
        case 1:                                     /* switch 2 */
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            return 1;
        }
        break;
    }
}

