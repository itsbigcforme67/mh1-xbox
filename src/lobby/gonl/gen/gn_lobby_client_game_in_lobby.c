/* G online (PC): lobby_client_game_in_lobby, written from Monster Hunter G's code at lobby 0x580BB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_InterruptFlag();
s32 Lbc_set_prim();
s32 To_LogOut();
s32 lobby_func_00580C70();
s32 lobby_func_00582A20();
extern u8 cw;
extern u8 pNet;
extern s32 put_back;

void lobby_client_game_in_lobby(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 temp_v1;
    u8 *temp_a0;

    if (Check_InterruptFlag(arg0, arg1, arg2, arg3) != 0) {
        temp_a0 = (*(void **)&cw);
        if (((*(u8 *)((u8 *)(temp_a0) + 0xB9B)) == 0) && ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) == 1)) {
            Lbc_set_prim(0, 0, (s32) &put_back, 0);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 0;
        }
        return;
    }
    temp_v1 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9B));
    switch (temp_v1) {                              /* irregular */
    case 0:
        lobby_func_00580C70(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */), M2C_ERROR(/* Read from unset register $a4 */), M2C_ERROR(/* Read from unset register $a5 */), M2C_ERROR(/* Read from unset register $a6 */), M2C_ERROR(/* Read from unset register $a7 */));
        return;
    case 1:
        lobby_func_00582A20(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    default:
        To_LogOut(4, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    }
}

