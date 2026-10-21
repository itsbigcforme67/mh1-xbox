/* G online (PC): lobby_client_game_in_lobby, written from Monster Hunter G's code at lobby 0x580BB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 pNet;
s32 Check_InterruptFlag();                          /* extern */
s32 Lbc_set_prim(); /* extern */
s32 To_LogOut();                         /* extern */
s32 lobby_func_00580C70();                      /* extern */
s32 lobby_func_00582A20();                      /* extern */
extern u8 put_back;

void lobby_client_game_in_lobby(void) {
    u8 temp_v1;
    u8 *temp_a0;

    if (Check_InterruptFlag() != 0) {
        temp_a0 = (*(void **)&cw);
        if (((*(u8 *)((u8 *)(temp_a0) + 0xB9B)) == 0) && ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) == 1)) {
            Lbc_set_prim(0, 0, &put_back, 0);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 0;
        }
        return;
    }
    temp_v1 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9B));
    switch (temp_v1) {                              /* irregular */
    case 0:
        lobby_func_00580C70();
        return;
    case 1:
        lobby_func_00582A20();
        return;
    default:
        To_LogOut(4);
        return;
    }
}

