#include "lobby_a.h"
extern int tbl;
extern char pl_name_tbl[];
extern char plData[];
extern char plData[];
void lm_member_trans(void) {
    u8 sp88;
    int var_s3;
    s32 var_s0;
    s32 var_s6;
    int var_s0_2;
    int var_s2_2;
    int var_s1;
    int var_s1_2;
    int var_s4;
    u8 temp_a1;
    u8 var_a2;
    int var_s2;
    int temp_a0;
    int temp_v1;

    temp_a1 = F(u8, pNet, 2);
    switch (temp_a1) {                              /* irregular */
    case 0:
        Disp_lb_menu(1);
        var_s0 = 0;
        var_s4 = (int)&player_work;
        var_s3 = (int)&lb_player;
        var_s2 = (int)&pl_name_tbl;
        var_s6 = 0;
        var_s1 = (int)&sp88;
        do {
            if (var_s0 != game_w.master) {
                if ((*(u8 *)var_s4) != 0) {
                    (*(u8 *)var_s2) = var_s3 + 4;
                    if (Online_ck() == 1) {
                        if (*(u8 *)0x39DAD4 != 0) {
                            (*(u8 *)var_s2) = var_s3 + 0x24;
                        }
                        (*(u8 *)var_s1) = F(u8, ((s32)cw + var_s6), 0x1348);
                    }
                } else {
                    (*(u8 *)var_s2) = tbl;
                }
                var_s2 += 4;
            }
            var_s0 += 1;
            var_s4 += 0xA00;
            var_s3 += 0x38;
            var_s6 += 0x2FC;
            var_s1 += 1;
        } while (var_s0 < 8);
        var_a2 = F(u8, pNet, 8);
        if (game_w.master < var_a2) {
            var_a2 -= 1;
        }
        F(int, &plData, 0xC) = (int)&pl_name_tbl;
        DispFrameList(&plData, 0, var_a2);
        var_s2_2 = 0x3C;
        var_s0_2 = 0;
        var_s1_2 = (int)&player_work;
        do {
            if (var_s0_2 != game_w.master) {
                if (((*(u8 *)var_s1_2) != 0) && (Lb_get_pl_stat2((s8)var_s0_2) == 0)) {
                    Lb_put_status(0x1A4, var_s2_2, 0x14, -1);
                }
                var_s2_2 =  ((var_s2_2 + 0x16) << 0x30) >> 0x30;
            }
            var_s0_2 += 1;
            var_s1_2 += 0xA00;
        } while (var_s0_2 < 8);
        return;
    case 2:
    case 1:
        temp_v1 = (int)pNet;
        Lb_PlayerStatus((int)&lb_player + (F(u8, temp_v1, 8) * 0x38), F(u8, temp_v1, 3));
        temp_a0 = (int)pNet;
        if (F(u8, temp_a0, 3) == 0) {
            Disp_FriendListEntry(0x110, (F(u8, temp_a0, 2) - 1) & 0xFF);
        }
        return;
    }
}
