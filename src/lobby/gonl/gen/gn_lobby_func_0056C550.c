/* G online (PC): lobby_func_0056C550, written from Monster Hunter G's code at lobby 0x56C550 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Plaza_LobbyEntry();
s32 Eft25_set();
s32 Get_sw_on2();
s32 Lb_eat_to_bell();
s32 Lb_select();
s32 Lbs_SeekId();
s32 Lbs_request_enter_lobby2();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
s32 func_00235C80();
s32 getFriendNow();
s64 lobby_func_00569390();
s32 lobby_func_0056AF20();
s32 lobby_func_0056C350();
s32 lobby_func_0056C3F0();
s32 lobby_func_0056D6E0();
s32 lobby_func_00571AF0();
s32 lobby_func_00572110();
s32 lobby_func_0057FEA0();
s32 lobby_func_00580A40();
s32 lobby_func_00581210();
void * memcpy();
void * memset();
void * strcpy();
extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBC4;
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBC4;
extern u8 lb_sys;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Lbc_RequestNetComment();              /* extern */
s32 SaveNetFile_ForLobby();                       /* extern */
s32 SoftKeyboard_alive_check();                     /* extern */
s16 net_Check_FriendSuu();        /* extern */
s32 net_Delete_FriendData(); /* extern */
extern u8 g_main_302CB0;
extern u8 g_main_3064AF;

s32 lobby_func_0056C550(s32 arg0) {
    u8 *var_a1;   /* (s16 *) */
    u8 *var_a2_9;   /* (s16 *) */
    u8 *var_a3;   /* (s16 *) */
    u8 *var_a3_2;   /* (s16 *) */
    u8 *var_a3_3;   /* (s16 *) */
    u8 *var_a3_4;   /* (s16 *) */
    u8 *var_a3_5;   /* (s16 *) */
    u8 *var_a3_6;   /* (s16 *) */
    u8 *var_a3_7;   /* (s16 *) */
    u8 *var_a3_8;   /* (s16 *) */
    u8 *var_a4;   /* (s16 *) */
    u8 *var_a4_2;   /* (s16 *) */
    u8 *var_a4_3;   /* (s16 *) */
    u8 *var_a4_4;   /* (s16 *) */
    u8 *var_a4_5;   /* (s16 *) */
    u8 *var_a4_6;   /* (s16 *) */
    u8 *var_a4_7;   /* (s16 *) */
    u8 *var_a4_8;   /* (s16 *) */
    s16 temp_s1;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a2;
    s32 temp_v0;
    s32 temp_v0_10;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v0_8;
    s32 temp_v0_9;
    s32 temp_v1_3;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a2_3;
    s32 var_a2_4;
    s32 var_a2_5;
    s32 var_a2_6;
    s32 var_a2_7;
    s32 var_a2_8;
    s32 var_s1;
    s32 var_v1_2;
    u16 temp_a0_5;
    u8 *temp_v1;
    u8 var_a0;
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a1_3;
    u8 *temp_a2_2;
    u8 *temp_a2_3;
    u8 *temp_a2_4;
    u8 *temp_s0;
    u8 *temp_s0_2;
    u8 *temp_v0_4;
    u8 *temp_v1_2;
    u8 *temp_v1_4;
    u8 *var_a3_9;
    u8 *var_a4_9;
    u8 *var_v0;
    u8 *var_v1;

    var_s1 = arg0;
    if (SoftKeyboard_alive_check() != 0) {
        var_s1 = 0;
    }
    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) = (s16) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 7));
    (*(s8 *)&D_0035EFEB) = 0;
    switch ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C))) {  /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
        if (((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) || ((*(u16 *)((u8 *)&(*(u16 *)((u8 *)&lb_sys + 0xA0)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 2))) != 0)) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 2;
        } else {
        case 1:                                     /* switch 1 */
            if (getFriendNow(0, 7, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) != 0) {

            } else {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
            }
        }
block_192:
        return 2;
    case 2:                                         /* switch 1 */
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        (*(u8 *)&D_0035EFEB) = 1;
        temp_a0 = var_s1 & 0xFFFF;
        if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
            if (temp_a0 & 0x40) {
                if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) == 1) {
                    return 3;
                }
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xF;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) = lobby_func_0056C350(temp_a0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = -1;
                func_00235C80(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                goto block_192;
            }
            if (temp_a0 & 0x2A0) {
                cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
            }
            goto block_192;
        }
        temp_a0_2 = var_s1 & 0xFFFF;
        if (temp_a0_2 & 0x40) {
            if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) == 1) {
                return 3;
            }
            cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xF;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) = lobby_func_0056C350(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = -1;
            func_00235C80(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            goto block_192;
        }
        if (temp_a0_2 & 0x20) {
            if ((*(u8 *)((u8 *)((((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + (*(void **)&cw))) + 0x1265)) != 2) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x614)) = 0;
                (*(u8 *)&D_0035EFEB) = 0;
                memset((*(void **)&cw) + 0xAF8, 0, 0x62);
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) = 0;
                SetDialogData(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                cnWrap_SoundRequest(7, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), M2C_ERROR(/* Read from unset register $a2 */), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
            }
        } else if (temp_a0_2 & 0x200) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xE;
            SetDialogData(0x1C, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
            SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        } else if (temp_a0_2 & 0x80) {
            temp_a0_3 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A;
            if ((*(u8 *)((u8 *)((temp_a0_3 + (*(void **)&cw))) + 0x1265)) != 2) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = (s8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C));
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xC;
                lobby_func_00571AF0(temp_a0_3, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), M2C_ERROR(/* Read from unset register $a2 */), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 2;
                temp_s0 = &g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30);
                memset((*(void **)&g_main_2EFBAC) + 0x9A, 0, 0x114);
                strcpy((*(void **)&g_main_2EFBAC) + 0x9A, temp_s0);
                strcpy((*(void **)&g_main_2EFBAC) + 0xA2, temp_s0 + 8);
                memset((*(void **)&g_main_2EFBAC) + 0x23C, 1, 3);
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x23C)) = 0;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) = 0;
                cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                cnWrap_SoundRequest(7, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), M2C_ERROR(/* Read from unset register $a2 */), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
            }
        } else {
            temp_s1 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604));
            lobby_func_00569390(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)) + 0x60F, &(*(u8 *)((u8 *)&tl_member_buff + 0x1398)) + 0x604, 7, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), var_s1);
            if (((((s16) ((s32) temp_s1))) != (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604))) && ((*(u16 *)((u8 *)&(*(u16 *)((u8 *)&lb_sys + 0xA0)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 2))) == 0)) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 1;
            }
        }
        goto block_192;
    case 3:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0 = lobby_func_0057FEA0(&g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        if ((temp_v0 != 1) && (temp_v0 != 0)) {

        } else {
            temp_v1 = (*(u8 **)&g_main_2EFBC4);
            if (*temp_v1 != 0) {
                memcpy((*(void **)&cw) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + 0x101C, temp_v1 + 0x20, 0x40);
                memset((*(void **)&cw) + 0xAF8, 0, 0x62);
                Lbc_RequestNetComment(&g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30));
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
            } else {
                Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(u8 *)&D_0035EFEB) = 0;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xD;
            }
        }
        goto block_192;
    case 4:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_2 = Lbs_SeekId((s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), (s32) (&g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30)), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x33)) = 3;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x614)) = 0;
            break;
        case 1:                                     /* switch 2 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xD;
            break;
        }
        goto block_192;
    case 5:                                         /* switch 1 */
        temp_a1 = (*(void **)&pNet);
        temp_a2 = var_s1 & 0xFFFF;
        (*(s16 *)((u8 *)(temp_a1) + 0x20)) = Get_sw_on2(0);
        (*(u8 *)&D_0035EFEB) = 1;
        if (temp_a2 & 0x40) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 2;
            cnWrap_SoundRequest(3, (s32) temp_a1, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        } else if (temp_a2 & 0x100) {
            temp_a2_2 = (*(void **)&cw);
            if ((*(u8 *)((u8 *)(temp_a2_2) + 0xFA61)) == 0) {
                temp_a1_2 = (*(void **)&pNet);
                if ((*(u8 *)((u8 *)(temp_a1_2) + 0x2E)) == 1) {
                    temp_v1_2 = ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + temp_a2_2;
                    if (((*(u16 *)((u8 *)(temp_v1_2) + 0x105C)) == 0) || ((*(u8 *)((u8 *)(temp_v1_2) + 0x1063)) != 3)) {
                        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = 2;
                        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0x13;
                        if ((*(u16 *)((u8 *)((((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + (*(void **)&cw))) + 0x105E)) == 0) {
                            SetDialogData(0x4F, (s32) temp_a1_2, (s32) temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
                        } else {
                            SetDialogData(0x4E, (s32) temp_a1_2, (s32) temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
                        }
                        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                        cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    } else {
                        (*(s8 *)((u8 *)(temp_a1_2) + 0x2D)) = 1;
                        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 6;
                        SetDialogData(0x3E, (s32) temp_a1_2, (s32) temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
                        SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                        cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    }
                } else {
                    cnWrap_SoundRequest(6, (s32) temp_a1_2, (s32) temp_a2_2, M2C_ERROR(/* Read from unset register $a3 */));
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xF;
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) = lobby_func_0056C350(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = 6;
                    func_00235C80(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                }
            }
        } else if (temp_a2 & 0x200) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xE;
            SetDialogData(0x1C, (s32) temp_a1, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
            SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        } else if (temp_a2 & 0x80) {
            temp_a0_4 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A;
            if ((*(u8 *)((u8 *)((temp_a0_4 + (*(void **)&cw))) + 0x1265)) != 2) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = 2;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xC;
                lobby_func_00571AF0(temp_a0_4, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 2;
                temp_s0_2 = &g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30);
                memset((*(void **)&g_main_2EFBAC) + 0x9A, 0, 0x114);
                strcpy((*(void **)&g_main_2EFBAC) + 0x9A, temp_s0_2);
                strcpy((*(void **)&g_main_2EFBAC) + 0xA2, temp_s0_2 + 8);
                memset((*(void **)&g_main_2EFBAC) + 0x23C, 1, 3);
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x23C)) = 0;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) = 0;
                cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                cnWrap_SoundRequest(7, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
            }
        } else {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x614)) = Eft25_set((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x614)), 3, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_192;
    case 6:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_3 = Lb_select((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_3) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            temp_a2_3 = (*(void **)&cw);
            temp_v1_3 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A;
            temp_a0_5 = *(temp_a2_3 + 0x105C + temp_v1_3);
            if (temp_a0_5 != 0) {
                temp_v1_4 = temp_v1_3 + temp_a2_3;
                if ((*(u8 *)((u8 *)(temp_v1_4) + 0x1063)) != 3) {
                    goto block_87;
                }
                if (lobby_func_0056AF20((s32) temp_a0_5, (s32) (*(u16 *)((u8 *)(temp_v1_4) + 0x105E)), (s32) *(&g_main_3064AF + (temp_a0_5 * 0x15C)), M2C_ERROR(/* Read from unset register $a3 */)) == 0) {
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xB;
                } else {
                    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0;
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 7;
                    SetDialogData(0x1B, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                }
            } else {
block_87:
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = 2;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0x13;
                if ((*(u16 *)((u8 *)((((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + (*(void **)&cw))) + 0x105E)) == 0) {
                    SetDialogData(0x4F, 3, (s32) temp_a2_3, M2C_ERROR(/* Read from unset register $a3 */));
                } else {
                    SetDialogData(0x4E, 3, (s32) temp_a2_3, M2C_ERROR(/* Read from unset register $a3 */));
                }
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            }
            break;
        case 3:                                     /* switch 3 */
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 5;
            break;
        }
        goto block_192;
    case 7:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_4 = ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + (*(void **)&cw);
        temp_v0_5 = lobby_func_00580A40((s32) (*(u16 *)((u8 *)(temp_v0_4) + 0x105C)), (s32) (*(u16 *)((u8 *)(temp_v0_4) + 0x105E)), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_5) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            if ((*(u16 *)((u8 *)((((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + (*(void **)&cw))) + 0x105E)) == 0) {
                (*(s32 *)((u8 *)((*(void **)&pNet)) + 0x14)) = 3;
                (*(u8 *)&D_0035EFEB) = 1;
            } else {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
            }
            break;
        case 1:                                     /* switch 4 */
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xB;
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), 1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        goto block_192;
    case 8:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_6 = Lbs_request_enter_lobby2((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_6) {                        /* switch 5; irregular */
        case 0:                                     /* switch 5 */
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
            break;
        case 1:                                     /* switch 5 */
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xB;
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        goto block_192;
    case 9:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_7 = CallBack_Result_Plaza_LobbyEntry((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_7) {                        /* switch 6; irregular */
        case 0:                                     /* switch 6 */
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
            break;
        case 1:                                     /* switch 6 */
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0xB;
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        goto block_192;
    case 10:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_8 = lobby_func_00581210((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        if ((temp_v0_8 != 1) && (temp_v0_8 != 0)) {

        } else {
            (*(s32 *)((u8 *)((*(void **)&pNet)) + 0x14)) = 0;
        }
        goto block_192;
    case 11:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (var_s1 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 5;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_192;
    case 12:                                        /* switch 1 */
        if (lobby_func_00572110((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) != 0) {

        } else {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E));
        }
        goto block_192;
    case 13:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (var_s1 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 2;
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_192;
    case 14:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_9 = Lb_select((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_9) {                        /* switch 7; irregular */
        case 0:                                     /* switch 7 */
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 0U;
            memcpy((*(void **)&cw) + 0xEE7, &g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30), 8);
            memcpy((*(void **)&cw) + 0xEEF, &g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30) + 8, 0x11);
            net_Delete_FriendData(&g_main_302CB0, (u8) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), 0x32);
            var_a0 = (u8) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A));
            if ((s32) var_a0 < 0x63) {
                if ((0x63 - var_a0) >= 9) {
                    temp_a1_3 = (*(void **)&cw);
                    var_v1 = temp_a1_3 + (var_a0 * 0x24A);
                    var_v0 = temp_a1_3 + (var_a0 * 0x24A);
                    do {
                        var_a4 = var_v1 + 0x1266;
                        var_a3 = var_v0 + 0x101C;
                        var_a2 = 0x125;
loop_138:
                        var_a2 -= 1;
                        (*(s16 *)var_a3) = (*(s16 *)var_a4);
                        var_a4 += 2;
                        var_a3 += 2;
                        if (var_a2 > 0) {
                            goto loop_138;
                        }
                        var_a4_2 = var_v1 + 0x14B0;
                        var_a3_2 = var_v0 + 0x1266;
                        var_a2_2 = 0x125;
loop_140:
                        var_a2_2 -= 1;
                        (*(s16 *)var_a3_2) = (*(s16 *)var_a4_2);
                        var_a4_2 += 2;
                        var_a3_2 += 2;
                        if (var_a2_2 > 0) {
                            goto loop_140;
                        }
                        var_a4_3 = var_v1 + 0x16FA;
                        var_a3_3 = var_v0 + 0x14B0;
                        var_a2_3 = 0x125;
loop_142:
                        var_a2_3 -= 1;
                        (*(s16 *)var_a3_3) = (*(s16 *)var_a4_3);
                        var_a4_3 += 2;
                        var_a3_3 += 2;
                        if (var_a2_3 > 0) {
                            goto loop_142;
                        }
                        var_a4_4 = var_v1 + 0x1944;
                        var_a3_4 = var_v0 + 0x16FA;
                        var_a2_4 = 0x125;
loop_144:
                        var_a2_4 -= 1;
                        (*(s16 *)var_a3_4) = (*(s16 *)var_a4_4);
                        var_a4_4 += 2;
                        var_a3_4 += 2;
                        if (var_a2_4 > 0) {
                            goto loop_144;
                        }
                        var_a4_5 = var_v1 + 0x1B8E;
                        var_a3_5 = var_v0 + 0x1944;
                        var_a2_5 = 0x125;
loop_146:
                        var_a2_5 -= 1;
                        (*(s16 *)var_a3_5) = (*(s16 *)var_a4_5);
                        var_a4_5 += 2;
                        var_a3_5 += 2;
                        if (var_a2_5 > 0) {
                            goto loop_146;
                        }
                        var_a4_6 = var_v1 + 0x1DD8;
                        var_a3_6 = var_v0 + 0x1B8E;
                        var_a2_6 = 0x125;
loop_148:
                        var_a2_6 -= 1;
                        (*(s16 *)var_a3_6) = (*(s16 *)var_a4_6);
                        var_a4_6 += 2;
                        var_a3_6 += 2;
                        if (var_a2_6 > 0) {
                            goto loop_148;
                        }
                        var_a4_7 = var_v1 + 0x2022;
                        var_a3_7 = var_v0 + 0x1DD8;
                        var_a2_7 = 0x125;
loop_150:
                        var_a2_7 -= 1;
                        (*(s16 *)var_a3_7) = (*(s16 *)var_a4_7);
                        var_a4_7 += 2;
                        var_a3_7 += 2;
                        if (var_a2_7 > 0) {
                            goto loop_150;
                        }
                        var_a4_8 = var_v1 + 0x226C;
                        var_a3_8 = var_v0 + 0x2022;
                        var_a2_8 = 0x125;
loop_152:
                        var_a2_8 -= 1;
                        (*(s16 *)var_a3_8) = (*(s16 *)var_a4_8);
                        var_a4_8 += 2;
                        var_a3_8 += 2;
                        if (var_a2_8 > 0) {
                            goto loop_152;
                        }
                        var_v1 += 0x1250;
                        var_a0 = (var_a0 + 8) & 0xFF;
                        var_v0 += 0x1250;
                    } while ((s32) var_a0 < 0x5B);
                }
                if ((s32) var_a0 < 0x63) {
                    temp_a2_4 = (*(void **)&cw);
                    var_a4_9 = temp_a2_4 + (var_a0 * 0x24A);
                    var_a3_9 = temp_a2_4 + ((var_a0 & 0xFF) * 0x24A);
                    do {
                        var_a2_9 = var_a4_9 + 0x1266;
                        var_a1 = var_a3_9 + 0x101C;
                        var_v1_2 = 0x125;
loop_158:
                        var_v1_2 -= 1;
                        (*(s16 *)var_a1) = (*(s16 *)var_a2_9);
                        var_a2_9 += 2;
                        var_a1 += 2;
                        if (var_v1_2 > 0) {
                            goto loop_158;
                        }
                        var_a4_9 += 0x24A;
                        var_a0 = (var_a0 + 1) & 0xFF;
                        var_a3_9 += 0x24A;
                    } while ((s32) var_a0 < 0x63);
                }
            }
            (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) = net_Check_FriendSuu(&g_main_302CB0, 0x32);
            lobby_func_0056D6E0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) >= (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606))) {
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) = (s16) ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606)) - 1);
            } else if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) != 0) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) - 1);
            }
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 0x11;
            SetDialogData(0x1D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        case 3:                                     /* switch 7 */
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 2;
            break;
        }
        goto block_192;
    case 15:                                        /* switch 1 */
        if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) == 0) {
            temp_v0_10 = SaveNetFile_ForLobby((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)));
            if ((temp_v0_10 != -1) && (temp_v0_10 != 1)) {
                goto block_176;
            }
            lobby_func_0056C3F0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) == -1) {
                return 3;
            }
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E));
            SetDialogData(0x3E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 1U;
block_176:
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            goto block_192;
        }
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) - 1);
        goto block_192;
    case 16:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (var_s1 & 0xFFFF & 0x20) {
            cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 3;
        }
        goto block_192;
    case 17:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (var_s1 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) + 1);
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_192;
    case 18:                                        /* switch 1 */
        if (getFriendNow(6, 1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) != 0) {

        } else {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 2;
        }
        goto block_192;
    case 19:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (var_s1 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E));
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = 0;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_192;
    default:                                        /* switch 1 */
        return 3;
    }
}

