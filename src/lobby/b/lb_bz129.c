/* lb_bz129 - lobby UI/client 0x005BB5C0-0x005BB6E8: lobby_client_game_in_lobby, lbc_in_lobby_00 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char put_back[];
extern char lbc_in_lobby_jmp_1826[];
extern char lbc_in_lobby_00_jmp_1842[];

void lobby_client_game_in_lobby(void) {
    int temp_a0;

    if (Check_InterruptFlag() != 0) {
        temp_a0 = (int)cw;
        if ((F(u8, temp_a0, 0x2C33) == 0) && (F(u8, temp_a0, 0x2C34) == 1)) {
            Lbc_set_prim(0, 0, &put_back);
            F(s8, pNet, 0x11) = 0;
        }
        return;
    }
    if (((int *)&lbc_in_lobby_jmp_1826)[F(u8, (u8 *)cw, 0x2C33)] == 0) {
        To_LogOut(1);
    }
    ((int (**)())&lbc_in_lobby_jmp_1826)[F(u8, (u8 *)cw, 0x2C33)]();
}

void lbc_in_lobby_00(void) {
    if (((int *)&lbc_in_lobby_00_jmp_1842)[F(u8, (u8 *)cw, 0x2C34)] == 0) {
        To_LogOut(1);
    }
    ((int (**)())&lbc_in_lobby_00_jmp_1842)[F(u8, (u8 *)cw, 0x2C34)]();
}
