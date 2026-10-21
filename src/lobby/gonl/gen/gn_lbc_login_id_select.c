/* G online (PC): lbc_login_id_select, written from Monster Hunter G's code at lobby 0x57AEF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 Fade_busy_ck();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 Set_userdata();
s32 To_LogOut();
s32 fade_set();
s32 lobby_func_0058BAA0();
void * memcpy();
void * memset();
s32 net_SetMenu_SelectHandleName();
extern u8 g_main_2EF2A8;
extern u8 CnetWork;
extern u8 User_data;
extern u8 cw;
extern u8 g_main_2EF2A8;
extern u8 game_w;
extern u8 lb_gbyte_2EF2A4;
extern u8 pNet;
s32 cnLBS_Send_LoginUserAccount(); /* extern */
s32 cnWrap_IsBBConnect();                           /* extern */
s32 cnetGet_Login_DecideUserHandle();     /* extern */
s32 cnetGet_Login_DecideUserID();         /* extern */
u8 cnetGet_Login_NoOfUserAccount();                 /* extern */
s32 cnetGet_Login_UserHandle();      /* extern */
s32 cnetGet_Login_UserID();             /* extern */
s32 cnetGet_Login_UserMiniData();    /* extern */
extern s32 network_work;
extern s32 player_work;

void lbc_login_id_select(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s1_2;
    s32 temp_s2;
    s32 temp_v0_2;
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    u8 temp_a0;
    u8 temp_s1;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a0_4;
    u8 *temp_a1;
    u8 *temp_v0;
    u8 *temp_v0_3;
    u8 *temp_v0_4;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_v0 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_v0) + 0xB9C));
    temp_a1 = temp_v0 + 0xB9C;
    switch (temp_a0) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v0) + 0xB9C)) = (u8) (temp_a0 + 1);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 2)) = 0U;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 1)) = 0U;
        memset((*(void **)&cw) + 0xB, 0, 0x20);
        memset((*(void **)&cw) + 0x2B, 0, 0x44);
        memset((*(void **)&cw) + 0x6F, 0, 0x100);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0)) = cnetGet_Login_NoOfUserAccount();
        var_s3 = 0;
        if ((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0)) > 0) {
            var_s2 = 0;
            var_s1 = 0;
            var_s0 = 0;
            do {
                cnetGet_Login_UserID(var_s3 & 0xFF, (*(void **)&cw) + var_s2 + 0xB);
                cnetGet_Login_UserHandle(var_s3 & 0xFF, (*(void **)&cw) + var_s1 + 0x2B);
                cnetGet_Login_UserMiniData(var_s3 & 0xFF, (*(void **)&cw) + var_s0 + 0x6F);
                var_s3 += 1;
                var_s2 += 8;
                var_s1 += 0x11;
                var_s0 += 0x40;
            } while (var_s3 < (s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0)));
        }
        if (cnWrap_IsBBConnect() != 0) {
            temp_a0_2 = (*(void **)&cw);
            if ((*(u8 *)((u8 *)(temp_a0_2) + 0)) == 0) {
                (*(s8 *)((u8 *)(temp_a0_2) + 0xB9B)) = 5;
                (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
                return;
            }
        }
    case -1:                                        /* switch 2 */
        return;
    case 1:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v0) + 0xB9C)) = (u8) (temp_a0 + 1);
        Lbc_init_network_work((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = (u8) (*(u8 *)((u8 *)((*(void **)&cw)) + 2));
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) = (u8) (*(u8 *)((u8 *)((*(void **)&cw)) + 1));
        return;
    case 2:                                         /* switch 1 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            temp_a0_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0xB9C)) + 1);
            if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 0) {
                fade_set(2);
                return;
            }
        }
        break;
    case 3:                                         /* switch 1 */
        temp_v0_2 = net_SetMenu_SelectHandleName(&network_work, (s32) temp_a1, arg2, arg3);
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a0_4 = (*(void **)&cw);
            (*(s8 *)&lb_gbyte_2EF2A4) = 0;
            (*(s8 *)&g_main_2EF2A8) = 0;
            (*(u8 *)((u8 *)(temp_a0_4) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_4) + 0xB9C)) + 1);
            return;
        case -2:                                    /* switch 2 */
            To_LogOut(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        break;
    case 4:                                         /* switch 1 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            temp_v1 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9C)) + 1);
            Lbc_set_prim(0, 0, 0, 0);
        }
        /* fallthrough */
    case 5:                                         /* switch 1 */
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) + 1);
        temp_s1 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29));
        (*(u8 *)((u8 *)((*(void **)&cw)) + 1)) = temp_s1;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        Set_userdata((u8 *)&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00));
        temp_s2 = temp_s1 * 0x11;
        memcpy((*(void **)&cw) + temp_s2 + 0x2B, &(*(s32 *)((u8 *)&User_data + 0x8)), 0x10);
        temp_v1_3 = (*(void **)&cw);
        temp_s0 = temp_s1 * 8;
        if ((*(s8 *)((u8 *)((temp_s0 + temp_v1_3)) + 0xB)) == 0) {
            temp_s0_2 = temp_s1 << 6;
            lobby_func_0058BAA0(temp_v1_3 + temp_s0_2 + 0x6F, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            temp_v0_3 = (*(void **)&cw);
            cnLBS_Send_LoginUserAccount(NULL, temp_v0_3 + temp_s2 + 0x2B, temp_v0_3 + temp_s0_2 + 0x6F);
        } else {
            temp_s1_2 = temp_s1 << 6;
            lobby_func_0058BAA0(temp_v1_3 + temp_s1_2 + 0x6F, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            temp_v0_4 = (*(void **)&cw);
            cnLBS_Send_LoginUserAccount(temp_v0_4 + temp_s0 + 0xB, temp_v0_4 + temp_s2 + 0x2B, temp_v0_4 + temp_s1_2 + 0x6F);
        }
        cnetGet_Login_DecideUserID((*(void **)&cw) + 0x1C0);
        cnetGet_Login_DecideUserHandle((*(void **)&cw) + 0x1C8);
        return;
    case 6:                                         /* switch 1 */
        Check_CallBackWait((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        return;
    default:                                        /* switch 1 */
        To_LogOut(4, (s32) temp_a1, arg2, arg3);
        break;
    }
}

