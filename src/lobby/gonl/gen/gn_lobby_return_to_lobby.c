/* G online (PC): lobby_return_to_lobby, written from Monster Hunter G's code at lobby 0x57CCF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_ReadCurrentPlace();
s32 CallBack_Result_Plaza_LobbyEntry();
s32 CallBack_Result_Plaza_ReadLobbyAllocation2();
s32 Check_CallBackWait();
s32 Lbc_SendMiniData();
s32 MH_lobbyClear();
s32 Set_userdata();
s32 To_EnterLobby();
s32 To_LogOut();
s32 cnLBS_Read_PlazaAllocation();
s32 lobby_func_0057ECE0();
s32 lobby_func_00580FA0();
s32 lobby_func_00581210();
s32 lobby_func_005AB8B0();
void * memset();
extern u8 cw;
extern u8 game_w;
extern u8 jtbl_1035_0065E950;
extern u8 pNet;
s32 cnLBS_Read_CurrentPlace(); /* extern */
extern u8 player_work;

void lobby_return_to_lobby(s32 unused_a0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    u8 *var_s0;   /* (s32 *) */
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 var_s1;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a1;
    u8 *temp_a0_3;
    u8 *temp_a1_2;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_a3_2;

    temp_a2 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0xB9A));
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_a2) + 0xB9A)) = (u8) (temp_a1 + 1);
        var_s1 = 0;
        var_s0 = &player_work;
        do {
            memset(var_s0, 0, 0xA00);
            var_s1 = (var_s1 + 1) & 0xFFFF;
            var_s0 += 0xA00;
        } while (var_s1 < 0xA);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 1;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        cnLBS_Read_CurrentPlace(CallBack_ReadCurrentPlace);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
    default:                                        /* switch 1 */
block_43:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        return;
    case 1:                                         /* switch 1 */
        Check_CallBackWait((s32) &jtbl_1035_0065E950, (s32) temp_a1, (s32) temp_a2, arg3);
        goto block_43;
    case 2:                                         /* switch 1 */
        temp_a0 = (*(u8 *)((u8 *)(temp_a2) + 0xB9D));
        temp_a3 = temp_a2 + 0xB9D;
        switch (temp_a0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(u8 *)((u8 *)(temp_a2) + 0xB9D)) = (u8) (temp_a0 + 1);
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
            cnLBS_Read_PlazaAllocation(0, 0xF, (s32) lobby_func_0057ECE0, (s32) temp_a3);
            Set_userdata(&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00));
            Lbc_SendMiniData(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        case 1:                                     /* switch 2 */
            Check_CallBackWait((s32) temp_a0, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
            break;
        case 2:                                     /* switch 2 */
            (*(u8 *)((u8 *)(temp_a2) + 0xB9A)) = (u8) (temp_a1 + 1);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
            break;
        default:                                    /* switch 2 */
            To_LogOut(4, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
            break;
        }
        goto block_43;
    case 3:                                         /* switch 1 */
        temp_a0_2 = (*(u8 *)((u8 *)(temp_a2) + 0xB9D));
        temp_a3_2 = temp_a2 + 0xB9D;
        switch (temp_a0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            (*(u8 *)((u8 *)(temp_a2) + 0xB9D)) = (u8) (temp_a0_2 + 1);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) = 1;
            MH_lobbyClear((s32) temp_a0_2, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3_2);
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
            lobby_func_005AB8B0(0, 0x87, (s32) CallBack_Result_Plaza_ReadLobbyAllocation2, M2C_ERROR(/* Read from unset register $a3 */));
            break;
        case 1:                                     /* switch 3 */
            Check_CallBackWait((s32) temp_a0_2, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3_2);
            break;
        case 2:                                     /* switch 3 */
            (*(u8 *)((u8 *)(temp_a2) + 0xB9A)) = (u8) (temp_a1 + 1);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
            break;
        default:                                    /* switch 3 */
            To_LogOut(4, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3_2);
            break;
        }
        goto block_43;
    case 4:                                         /* switch 1 */
        temp_v0 = CallBack_Result_Plaza_LobbyEntry((s32) &jtbl_1035_0065E950, (s32) temp_a1, (s32) temp_a2, arg3);
        switch (temp_v0) {                          /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            temp_a0_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_3) + 0xB9A)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0xB9A)) + 1);
            break;
        case 1:                                     /* switch 4 */
            To_LogOut(4, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        goto block_43;
    case 5:                                         /* switch 1 */
        temp_v0_2 = lobby_func_00581210((s32) &jtbl_1035_0065E950, (s32) temp_a1, (s32) temp_a2, arg3);
        if ((temp_v0_2 != 0) && (temp_v0_2 != 1)) {
            goto block_43;
        }
        temp_a1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a1_2) + 0xB9A)) = (u8) ((*(u8 *)((u8 *)(temp_a1_2) + 0xB9A)) + 1);
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        return;
    case 6:                                         /* switch 1 */
        temp_v0_3 = lobby_func_00580FA0((s32) &jtbl_1035_0065E950, (s32) temp_a1, (s32) temp_a2, arg3);
        if ((temp_v0_3 != 0) && (temp_v0_3 != 1)) {
            goto block_43;
        }
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
        To_EnterLobby(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        return;
    }
}

