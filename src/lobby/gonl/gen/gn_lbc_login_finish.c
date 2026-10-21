/* G online (PC): lbc_login_finish, written from Monster Hunter G's code at lobby 0x57B9C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 CallBack_SendMiniData();
s32 Check_CallBackWait();
s32 Lb_make_quest_tbl();
s32 Lb_set_mini_data();
s32 To_LogOut();
s32 lobby_func_0057B800();
s32 lobby_func_0058BAA0();
extern u8 g_main_2EF2A8;
extern u8 g_main_2EF390;
extern u8 BsLbsCount;
extern u8 CnetWork;
extern u8 cw;
extern u8 g_main_2EF2A8;
extern u8 g_main_2EF390;
extern u8 game_w;
extern u8 lb_sys;
extern u8 pNet;
extern u8 player_work;
s32 cnLBS_Send_UserMiniData(); /* extern */
s32 cnLbc_Init_NgServerId();                    /* extern */
s32 memset();              /* extern */
extern u8 my_user_mini_data;

void lbc_login_finish(void) {
    u8 *var_a0;   /* (s32 *) */
    s32 temp_v0;
    s32 var_a1_2;
    s8 var_a1;
    u8 temp_v1;
    u8 *temp_a0;
    u8 *temp_a0_2;

    temp_v1 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C));
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_v0 = lobby_func_0057B800();
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
            break;
        case 1:                                     /* switch 2 */
            To_LogOut(4);
            break;
        }
        break;
    case 1:                                         /* switch 1 */
        CallBackWaitInit();
        if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 0) {
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) == 0) {
                var_a1 = 0x4C;
                (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&player_work + 0x736)) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00))) = 0x4C;
            } else {
                var_a1 = 0x4D;
                (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&player_work + 0x736)) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00))) = 0x4D;
            }
            (*(s8 *)((u8 *)&game_w + 0x14)) = var_a1;
            Lb_set_mini_data(&my_user_mini_data);
            lobby_func_0058BAA0(&my_user_mini_data);
        }
        if ((*(u8 *)((u8 *)&CnetWork + 0x5)) != 3) {
            if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 0) {
                goto block_20;
            }
        } else {
block_20:
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA63)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA62)) = 0;
            (*(s8 *)((u8 *)&game_w + 0x1F2)) = 0;
            (*(s8 *)((u8 *)&game_w + 0x1F3)) = 0;
            (*(s8 *)((u8 *)&game_w + 0x1F4)) = 0;
            (*(s16 *)((u8 *)&game_w + 0xF8)) = 0;
            Lb_make_quest_tbl();
        }
        memset((*(s32 *)&g_main_2EF390) + 0x23F4, 0, 0x20);
        (*(u8 *)((u8 *)(&my_user_mini_data) + 2)) = (u8) (*(u8 *)&g_main_2EF2A8);
        (*(s8 *)((u8 *)(&my_user_mini_data) + 0x10)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 2;
        lobby_func_0058BAA0(&my_user_mini_data);
        cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, CallBack_SendMiniData);
        var_a1_2 = 0;
        var_a0 = &(*(u8 *)((u8 *)&lb_sys + 0xA0));
        do {
            (*(s16 *)((u8 *)(var_a0) + 0)) = 0;
            (*(s16 *)((u8 *)(var_a0) + 2)) = 0;
            var_a1_2 += 6;
            (*(s16 *)((u8 *)(var_a0) + 4)) = 0;
            (*(s16 *)((u8 *)(var_a0) + 6)) = 0;
            (*(s16 *)((u8 *)(var_a0) + 8)) = 0;
            (*(s16 *)((u8 *)(var_a0) + 0xA)) = 0;
            var_a0 += 0xC;
        } while (var_a1_2 < 0xC);
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) + 1);
        break;
    case 2:                                         /* switch 1 */
        Check_CallBackWait();
        break;
    case 3:                                         /* switch 1 */
        cnLbc_Init_NgServerId();
        if (((*(u8 *)((u8 *)&CnetWork + 0x5)) == 0) && ((*(u8 *)&BsLbsCount) == 1)) {
            (*(u8 *)((u8 *)&CnetWork + 0x5)) = 1;
        }
        if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 0) {
            (*(u8 *)((u8 *)&CnetWork + 0x5)) = 1;
            To_LogOut(0);
        } else {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 8;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
        }
        break;
    default:                                        /* switch 1 */
        To_LogOut(4);
        break;
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
}

