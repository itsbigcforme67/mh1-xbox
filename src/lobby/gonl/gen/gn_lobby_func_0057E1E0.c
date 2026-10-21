/* G online (PC): lobby_func_0057E1E0, written from Monster Hunter G's code at lobby 0x57E1E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_InterruptFlag();
s32 To_EnterLobby();
s32 To_EnterPlaza();
s32 To_LogOut();
s32 fade_set();
s32 getHandleFromID();
s32 lbc_in_plaza_00();
s32 lobby_func_0057E990();
s32 str_stop();
extern u8 cw;
extern u8 pNet;
void lobby_func_0057E1E0(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    s32 temp_v0;
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_v1) + 0xB9B));
    temp_a1 = temp_v1 + 0xB9B;
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9B)) = (u8) (temp_a0 + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        /* fallthrough */
    case 1:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (lbc_in_plaza_00((s32) temp_a0, (s32) temp_a1, arg2, arg3) != 0) {
            return;
        }
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9B)) + 1);
        return;
    case 2:                                         /* switch 1 */
        if (Check_InterruptFlag((s32) temp_a0, (s32) temp_a1, arg2, arg3) == 0) {
            temp_v0 = getHandleFromID(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            switch (temp_v0) {                      /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                To_EnterLobby(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                fade_set(0xA);
                str_stop(0);
                str_stop(1);
                return;
            case 3:                                 /* switch 2 */
                To_EnterPlaza(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                return;
            case 1:                                 /* switch 2 */
                To_LogOut(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
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
        lobby_func_0057E990((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        return;
    default:                                        /* switch 1 */
        To_LogOut(4, (s32) temp_a1, arg2, arg3);
        break;
    }
}

