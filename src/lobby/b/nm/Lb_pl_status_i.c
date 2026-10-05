#include "lobby_f.h"
extern u8 * pNet;
extern char Lb_pl_status_t[];
void Lb_pl_status_i(void) {
    u8 temp_a0;
    void *temp_s0;

    temp_a0 = game_w.master;
    Lbc_init_network_work(temp_a0);
    Lbc_set_prim(0, &Lb_pl_status_t, 0);
    temp_s0 = F(void *, ((u8 *)&player_work + (temp_a0 * 0xA00)), 0x3B0);
    Lb_get_comment((u8 *)&lb_player + (F(u16, temp_s0, 0xC) * 0x38) + 0x24);
    if ((temp_s0 == 0) || (F(u8, temp_s0, 0) == 0)) {
        F(s32, &lb_sys, 0x68) = 0;
        F(s32, &lb_sys, 0x6C) = 0;
        return;
    }
    *(s8 *)0x39DAD0 = 0;
    F(u8, pNet, 8) = (u8) F(u16, temp_s0, 0xC);
    Lb_PlStatusSet(F(u8, pNet, 8));
    cnWrap_SoundRequest(6);
}
