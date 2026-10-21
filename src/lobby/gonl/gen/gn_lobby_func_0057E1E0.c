/* G online (PC): lobby_func_0057E1E0, written from Monster Hunter G's code at lobby 0x57E1E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_InterruptFlag();
s32 To_EnterLobby();
s32 To_EnterPlaza();
s32 To_LogOut();
s32 getHandleFromID();
s32 lbc_in_plaza_00();
s32 lobby_func_0057E990();
extern u8 cw;
extern u8 pNet;
s32 fade_set();                          /* extern */
s32 str_stop();                          /* extern */

void lobby_func_0057E1E0(void) {
    s32 temp_v0;
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_v1) + 0xB9B));
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9B)) = (u8) (temp_a0 + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        /* fallthrough */
    case 1:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (lbc_in_plaza_00() != 0) {
            return;
        }
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9B)) + 1);
        return;
    case 2:                                         /* switch 1 */
        if (Check_InterruptFlag() == 0) {
            temp_v0 = getHandleFromID();
            switch (temp_v0) {                      /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                To_EnterLobby();
                fade_set(0xA);
                str_stop(0);
                str_stop(1);
                return;
            case 3:                                 /* switch 2 */
                To_EnterPlaza();
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                return;
            case 1:                                 /* switch 2 */
                To_LogOut(0);
                fade_set(0xA);
                str_stop(0);
                str_stop(1);
                return;
            }
        } else {
            return;
        }
        break;
    case 3:                                         /* switch 1 */
        lobby_func_0057E990();
        return;
    default:                                        /* switch 1 */
        To_LogOut(4);
        break;
    }
}

