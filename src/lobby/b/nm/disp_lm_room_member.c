#include "lobby_a.h"
extern s32 no_pl;
extern int lbmw;
extern char pf_room_member[];
extern char pf_room_member[];
void disp_lm_room_member(void) {
    s32 sp30;
    int var_s0;
    s32 temp_v0_2;
    s32 var_v0;
    s8 temp_v0;
    u32 var_s1;
    u8 temp_a1;
    u8 var_a2;
    int temp_a0;
    int temp_v1;
    int temp_v1_2;

    temp_a1 = F(u8, lbmw, 9);
    switch (temp_a1) {                              /* irregular */
    case 0:
        Disp_lb_menu(1);
        var_s1 = 0;
        var_s0 = (int)&sp30;
        do {
            temp_v0 = F(s8, (lbmw + var_s1), 0xD);
            if (temp_v0 > 0) {
                var_v0 = Lb_room_member(temp_v0 & 0xFF, *(u8 *)0x39DAD4);
            } else {
                var_v0 = no_pl;
            }
            (*(s32 *)var_s0) = var_v0;
            var_s1 += 1;
            var_s0 += 4;
        } while (var_s1 < 3U);
        temp_v1 = lbmw;
        var_a2 = -1U;
        if (F(u8, temp_v1, 8) != 0) {
            var_a2 = F(u8, temp_v1, 0xA);
        }
        F(int, &pf_room_member, 0xC) = (int)&sp30;
        DispFrameList(&pf_room_member, 0, var_a2);
        return;
    case 2:
    case 1:
        temp_v1_2 = lbmw;
        temp_v0_2 = Lb_room_member(F(u8, (F(u8, temp_v1_2, 0xA) + temp_v1_2), 0xD), 1U);
        if (temp_v0_2 != 0) {
            Lb_PlayerStatus((int)&lb_player + ((Lb_get_plID(temp_v0_2) & 0xFF) * 0x38), F(u8, lbmw, 0xB));
            temp_a0 = lbmw;
            if (F(u8, temp_a0, 0xB) == 0) {
                Disp_FriendListEntry(0x110, (F(u8, temp_a0, 9) - 1) & 0xFF);
            }
        }
        /* fallthrough */
    case 3:
        return;
    }
}
