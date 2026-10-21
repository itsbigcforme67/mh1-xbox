/* G online (PC): net_ToNetworkLobby, written from Monster Hunter G's code at lobby 0x586350 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_clearChatList();
s32 MH_lobbyClear();
extern u8 g_main_2EF390;
extern u8 g_main_2EFBAC;
extern u8 BsLbsErrNum;
extern u8 cw;
extern u8 g_main_2EF390;
extern u8 g_main_2EFBAC;
extern u8 lb_sys;
extern u8 pNet;
extern u8 quest_price;
extern u8 tl_member_buff;
s32 cnLBS_Init_LoginLobbyServer();              /* extern */
s32 cnLbc_SetIspRestTime();               /* extern */
s32 lobby_func_0059A1F0();                      /* extern */
s32 lobby_func_0059F410();                      /* extern */
s32 memset();        /* extern */
extern s32 g_lb_6B42D0;
extern u8 client_work;
extern u8 RecvMailInfo;
extern u8 network_work;

void net_ToNetworkLobby(void) {
    u8 *temp_v1;   /* (s32 *) */
    u8 *var_a0;   /* (s32 *) */
    u8 *var_a4;   /* (s16 *) */
    s32 temp_a2;
    s32 temp_v0;
    s32 var_a1;
    s32 var_a1_2;
    s32 var_a3;
    s32 var_a5;

    memset((*(u8 **)&g_main_2EF390), 0, 0x23F4);
    memset((*(u8 **)&g_main_2EF390) + 0x23F4, 0, 0x20);
    memset((*(u8 **)&g_main_2EF390) + 0x23F4 + 0x20, 0, 0x9C8);
    memset((*(u8 **)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8, 0, 0xF00);
    (*(u8 **)&cw) = &client_work;
    (*(u8 **)&pNet) = &network_work;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB99)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB9B)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB9D)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB9E)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB9F)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xBA0)) = 0;
    cnLbc_SetIspRestTime((*(u8 **)&cw) + 0xFA84);
    MH_lobbyClear();
    lobby_func_0059F410();
    (*(s32 *)((u8 *)((*(u8 **)&cw)) + 0xFA80)) = 0xE10;
    (*(s32 *)&quest_price) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xB76)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xBA8)) = 0;
    (*(s8 *)((u8 *)((*(u8 **)&cw)) + 0xFA61)) = 0;
    Lb_clearChatList();
    (*(s8 *)&BsLbsErrNum) = 0;
    cnLBS_Init_LoginLobbyServer();
    (*(u8 **)&g_main_2EFBAC) = &g_lb_6B42D0;
    memset((*(u8 **)&g_main_2EFBAC), 0, 0x240);
    var_a1 = 0;
    var_a0 = &RecvMailInfo;
    (*(u8 *)((u8 *)((*(u8 **)&g_main_2EFBAC)) + 0x234)) = 0U;
    do {
        if ((*(s8 *)((u8 *)(var_a0) + 1)) != 0) {
            temp_v1 = (*(u8 **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_v1) + 0x234)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x234)) + 1);
        }
        var_a1 += 1;
        var_a0 += 0x9A;
    } while (var_a1 < 8);
    memset((*(u8 **)&cw) + 0xF80, 0, 0x9A);
    memset((*(u8 **)&cw) + 0xEE6, 0, 0x9A);
    memset(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)), 0, 0x618);
    var_a1_2 = 0;
    var_a4 = &(*(s16 *)((u8 *)&lb_sys + 0xA0));
    var_a3 = 0;
    do {
        (*(s16 *)var_a4) = 0;
        var_a5 = 0;
loop_6:
        temp_v0 = var_a5 + var_a3;
        temp_a2 = temp_v0 * 0x24A;
        if (temp_v0 < 0x65) {
            (*(s8 *)((u8 *)(((*(u8 **)&cw) + temp_a2)) + 0x1062)) = 0;
            (*(s8 *)((u8 *)(((*(u8 **)&cw) + temp_a2)) + 0x1265)) = 2;
        }
        var_a5 += 1;
        if (var_a5 < 7) {
            goto loop_6;
        }
        var_a1_2 += 1;
        var_a4 += 2;
        var_a3 += 7;
    } while (var_a1_2 < 0xC);
    Lb_clearChatList();
    lobby_func_0059A1F0();
}

