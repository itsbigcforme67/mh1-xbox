/* G online (PC): Lb_join, written from Monster Hunter G's code at lobby 0x589820 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_eat_to_bell();
void * Lb_get_quest_str();
s32 Lb_set_mini_data();
s32 Lbc_SendMiniData();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 SetDialogData();
s32 cnWrap_SoundRequest();
s32 flMemset();
s32 lobby_func_00559C30();
s32 lobby_func_00559CE0();
void * memset();
void * strcpy();
extern u8 g_main_2EFABC;
extern u8 g_main_2EFAC0;
extern u8 D_0035EFEB;
extern u8 RoomRule;
extern u8 cw;
extern u8 g_main_2EFABC;
extern u8 g_main_2EFAC0;
extern u8 game_w;
extern u8 mhRule;
extern u8 npc_dialog_table;
extern u8 pNet;
extern u8 quest_price;
extern u8 select_w;
#ifdef __MWERKS__
s32 Lb_get_quest_type();                         /* extern */
#else
signed char Lb_get_quest_type();                         /* extern */   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_menu_quest_info();                    /* extern */
s32 Lb_put_set01();                      /* extern */
s64 Lb_talk_check_default();                 /* extern */
s32 Lbc_DownloadQuest();                    /* extern */
s32 Lbc_GuestReadRoom();                          /* extern */
s32 Lbc_ReadRoomInfo();                           /* extern */
u16 *Lbs_GetRoomInfo();                       /* extern */
s32 Lbs_GuestEnterRoom();                         /* extern */
s32 Lbs_RoomExit();                               /* extern */
s32 check_room_require();                         /* extern */
s32 get_questLevelNum();                     /* extern */
void *get_quest_info();                             /* extern */
s32 join_input_password();                 /* extern */
s32 lb_join_talk();                             /* extern */
s32 lb_select_room();                             /* extern */
s32 lb_select_room_list();                        /* extern */
s32 lb_select_set_data();                    /* extern */
s32 sub_main_func_004C8930();      /* extern */
extern s32 Lb_join_trans;
extern s32 User_data;
extern s32 lbCommer;
extern s32 lb_pit;
extern s32 lb_sys;
extern s32 player_work;
extern s32 quest_title;

void Lb_join(void) {
    s32 temp_a0_2;
    s32 temp_s1;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v1;
    s32 temp_v1_2;
    s8 temp_a1;
    s8 var_v0;
    u8 *temp_s0;   /* (u16 *) */
    u8 *temp_v0_8;   /* (u16 *) */
    u8 temp_a0;
    void **temp_s0_2;

    temp_s1 = Get_sw2(0) & 0xFFFF;
    temp_s0 = Lbs_GetRoomInfo((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)));
    switch ((*(s8 *)((u8 *)&lb_sys + 0x1C))) {                           /* switch 1 */
    case 0:                                         /* switch 1 */
        if (get_questLevelNum(1) == 0) {
            Lb_put_set01(0xE);
            lb_sys = 0;
            (*(s8 *)((u8 *)&lb_sys + 0x88)) = 0x14;
            (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0;
            return;
        }
        (*(s8 *)&D_0035EFEB) = 0;
        (*(s8 *)((u8 *)&lb_sys + 0x1C)) += 1;
        SetDialogData(0x67, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        flMemset(&lb_pit, 0, 0xC);
        (*(s32 *)((u8 *)&lb_pit + 0x4)) = (*(s32 *)((u8 *)&npc_dialog_table + 0x18C));
        Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        Lbc_set_prim(0, (s32) &Lb_join_trans, 0, 0);
    default:                                        /* switch 1 */
block_106:
        lb_join_talk();
        return;
    case 1:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0 = Lbc_ReadRoomInfo((*(s8 *)((u8 *)&lb_sys + 0x1C)));
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(s8 *)((u8 *)&lb_pit + 0x8)) = 0;
            lb_pit = 0;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) += 1;
            if (sub_main_func_004C8930((u8 *)&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00), &User_data) == 1) {
                Lb_put_set01(0x11);
            }
            (*(u8 *)&D_0035EFEB) = 1;
            break;
        case 1:                                     /* switch 2 */
            (*(u8 *)&D_0035EFEB) = 0;
            var_v0 = 0x10;
block_105:
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = var_v0;
            break;
        }
        goto block_106;
    case 2:                                         /* switch 1 */
        if ((((s8) (Lb_talk_check_default(0)))) != 0) {
            var_v0 = (*(s8 *)((u8 *)&lb_sys + 0x1C)) + 1;
            goto block_105;
        }
        goto block_106;
    case 3:                                         /* switch 1 */
        temp_v0_2 = lb_select_room_list((*(s8 *)((u8 *)&lb_sys + 0x1C)));
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            var_v0 = (*(s8 *)((u8 *)&lb_sys + 0x1C)) + 1;
            goto block_105;
        case 3:                                     /* switch 3 */
            var_v0 = 0xF;
            goto block_105;
        case 1:                                     /* switch 3 */
            (*(u8 *)&D_0035EFEB) = 0;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0x10;
            SetDialogData(0x53, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        goto block_106;
    case 4:                                         /* switch 1 */
        (*(s8 *)((u8 *)&lb_sys + 0x1C)) += 1;
        (*(u8 *)&D_0035EFEB) = 0;
        SetDialogData(0x67, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        goto block_106;
    case 5:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (Lbc_GuestReadRoom((*(u8 *)((u8 *)&lb_sys + 0x7E))) != 0) {

        } else {
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) += 1;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x22)) = 0;
            (*(s32 *)&g_main_2EFAC0) = (s32) *Lbs_GetRoomInfo((*(u8 *)((u8 *)&lb_sys + 0x7E)));
            (*(s8 *)&g_main_2EFABC) = 0;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x32)) = (s8) (*(s32 *)&g_main_2EFAC0);
            lb_select_set_data((u16) (*(s32 *)&g_main_2EFAC0));
            (*(u8 *)&D_0035EFEB) = 1;
        }
        goto block_106;
    case 6:                                         /* switch 1 */
        temp_v0_3 = lb_select_room((*(s8 *)((u8 *)&lb_sys + 0x1C)));
        switch (temp_v0_3) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            lb_pit = 0;
            (*(s8 *)((u8 *)&lb_pit + 0x9)) = 0;
            (*(s8 *)((u8 *)&lb_pit + 0x8)) = 1;
            (*(u8 *)&D_0035EFEB) = 0;
            var_v0 = (*(s8 *)((u8 *)&lb_sys + 0x1C)) + 1;
            goto block_105;
        case 3:                                     /* switch 4 */
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 3;
            break;
        }
        goto block_106;
    case 7:                                         /* switch 1 */
        temp_a0 = (*(u8 *)((u8 *)(temp_s0) + 0x16));
        switch (temp_a0) {                          /* switch 5; irregular */
        case 3:                                     /* switch 5 */
            if (((s32) (*(u16 *)((u8 *)(temp_s0) + 6)) >= (s32) (*(u16 *)((u8 *)(temp_s0) + 8))) || ((*(s8 *)&g_main_2EFABC) != 0)) {
                if (temp_a0 != 4) {
                default:                            /* switch 5 */
                    Lb_put_set01(8);
                } else {
                case 4:                             /* switch 5 */
                    Lb_put_set01(9);
                }
                (*(s8 *)((u8 *)&lb_pit + 0x8)) = 0;
                lb_pit = 0;
                (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 3;
                (*(u8 *)&D_0035EFEB) = 1;
                cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else if ((((s8) (Lb_talk_check_default(0)))) != 0) {
                if ((*(s8 *)((u8 *)&lb_pit + 0x9)) == 0) {
                    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA5F)) != 0) {
                        lb_pit = 0;
                        (*(s8 *)((u8 *)&lb_pit + 0x8)) = 7;
                        (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0xE;
                        (*(u8 *)&D_0035EFEB) = 1;
                    } else {
                        (*(u8 *)&D_0035EFEB) = 0;
                        var_v0 = (*(s8 *)((u8 *)&lb_sys + 0x1C)) + 1;
                        goto block_105;
                    }
                } else {
                    (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 3;
                    (*(u8 *)&D_0035EFEB) = 1;
                }
            }
            break;
        }
        goto block_106;
    case 8:                                         /* switch 1 */
        if (check_room_require((*(s8 *)((u8 *)&lb_sys + 0x1C))) == 0) {
            if ((*(u8 *)((u8 *)(temp_s0) + 0x17)) != 0) {
                lb_pit = 0;
                (*(s8 *)((u8 *)&lb_pit + 0x8)) = 2;
                (*(s8 *)((u8 *)&lb_sys + 0x1D)) = 0;
                memset(&(*(s32 *)((u8 *)&RoomRule + 0x2)), 0, 0x10);
                (*(u8 *)&D_0035EFEB) = 0;
                var_v0 = (*(s8 *)((u8 *)&lb_sys + 0x1C)) + 1;
                goto block_105;
            }
            lb_pit = 0;
            (*(s8 *)((u8 *)&lb_pit + 0x8)) = 3;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0xA;
            SetDialogData(0x67, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)&D_0035EFEB) = 1;
            (*(u16 *)((u8 *)&mhRule + 0x52)) = (u16) (*(s32 *)&g_main_2EFAC0);
        } else {
            SetDialogData(0x56, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0x11;
            (*(u8 *)&D_0035EFEB) = 0;
        }
        goto block_106;
    case 9:                                         /* switch 1 */
        Lb_talk_check_default(1);
        temp_v0_4 = join_input_password(&(*(s32 *)((u8 *)&RoomRule + 0x2)));
        switch (temp_v0_4) {                        /* switch 6; irregular */
        case 0:                                     /* switch 6 */
            (*(s8 *)((u8 *)&lb_pit + 0x8)) = 3;
            lb_pit = 0;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) += 1;
            SetDialogData(0x67, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)&D_0035EFEB) = 0;
            (*(u16 *)((u8 *)&mhRule + 0x52)) = (u16) (*(s32 *)&g_main_2EFAC0);
            break;
        case 3:                                     /* switch 6 */
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 3;
            (*(s8 *)((u8 *)&lb_pit + 0x8)) = 0;
            (*(u8 *)&D_0035EFEB) = 1;
            lb_pit = 0;
            cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        goto block_106;
    case 10:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if ((s32) (*(u16 *)((u8 *)&mhRule + 0x52)) < 0xC350) {
            var_v0 = (*(s8 *)((u8 *)&lb_sys + 0x1C)) + 1;
            goto block_105;
        }
        temp_v0_5 = Lbc_DownloadQuest((s32) (lobby_func_00559CE0((s32) (*(u16 *)((u8 *)&mhRule + 0x52)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) << 0x38) >> 0x38, (*(u16 *)((u8 *)&mhRule + 0x52)));
        switch (temp_v0_5) {                        /* switch 7; irregular */
        case 0:                                     /* switch 7 */
            if ((*(u16 *)((u8 *)(get_quest_info()) + 0x1E)) != (*(u16 *)((u8 *)&mhRule + 0x52))) {
                SetDialogData(0x53, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                var_v0 = 0x10;
                (*(u8 *)&D_0035EFEB) = 0;
            } else {
                var_v0 = (*(s8 *)((u8 *)&lb_sys + 0x1C)) + 1;
            }
            goto block_105;
        case 1:                                     /* switch 7 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            var_v0 = 0x10;
            (*(u8 *)&D_0035EFEB) = 0;
            goto block_105;
        }
        goto block_106;
    case 11:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_6 = Lbs_GuestEnterRoom((*(s8 *)((u8 *)&lb_sys + 0x1C)));
        switch (temp_v0_6) {                        /* switch 8; irregular */
        case 0:                                     /* switch 8 */
            temp_v1 = (*(s32 *)&g_main_2EFAC0);
            if (temp_v1 != *Lbs_GetRoomInfo((*(u8 *)((u8 *)&lb_sys + 0x7E)))) {
                var_v0 = 0x13;
                (*(u8 *)&D_0035EFEB) = 0;
                goto block_105;
            }
            temp_v0_7 = lobby_func_00559C30(temp_v1 & 0xFFFF, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Lb_menu_quest_info(temp_v0_7);
            temp_a1 = Lb_get_quest_type(temp_v0_7) | 0x10;
            *(&(*(u8 *)((u8 *)&player_work + 0x916)) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00)) = temp_a1;
            temp_v0_8 = Lbs_GetRoomInfo((*(u8 *)((u8 *)&lb_sys + 0x7E)), temp_a1);
            temp_a0_2 = (*(s32 *)&g_main_2EFAC0);
            (*(s16 *)((u8 *)&game_w + 0x2C)) = (s16) temp_a0_2;
            (*(s16 *)((u8 *)&select_w + 0xD0)) = (s16) temp_a0_2;
            (*(u16 *)((u8 *)&mhRule + 0x52)) = (u16) temp_a0_2;
            (*(s16 *)((u8 *)&game_w + 0xFA)) = (s16) ((s32) ((*(u16 *)temp_v0_8) & 0xFFFF0000) >> 0x10);
            Lbc_SendMiniData(temp_a0_2, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Lb_set_mini_data((*(void **)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x3A8, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Lb_set_mini_data((u8 *)&lbCommer + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x1C, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            temp_s0_2 = (*(void ***)((u8 *)(temp_v0_7) + 0x18));
            (*(s8 *)((u8 *)&lb_pit + 0x8)) = 3;
            (*(u8 *)&D_0035EFEB) = 0;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) += 1;
            lb_pit = 0;
            SetDialogData(0x52, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            temp_v1_2 = (*(s32 *)&g_main_2EFAC0);
            (*(s32 *)&quest_price) = 0;
            if (temp_v1_2 >= 0xC350) {
                strcpy(&quest_title, Lb_get_quest_str(temp_v1_2 & 0xFFFF, 0, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)));
            } else {
                strcpy(&quest_title, *temp_s0_2);
            }
            break;
        case 1:                                     /* switch 8 */
            (*(u8 *)&D_0035EFEB) = 0;
            var_v0 = 0x10;
            goto block_105;
        }
        goto block_106;
    case 12:                                        /* switch 1 */
        Lb_talk_check_default(1);
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s1 & 0xFFFF & 0x20) {
            (*(u8 *)&D_0035EFEB) = 1;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) += 1;
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_106;
    case 13:                                        /* switch 1 */
        lb_sys = 0;
        (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
        (*(s8 *)((u8 *)&lb_sys + 0x88)) = 0x14;
        (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0;
        goto block_106;
    case 14:                                        /* switch 1 */
        if ((((s8) (Lb_talk_check_default(0)))) != 0) {
        case 15:                                    /* switch 1 */
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0;
            lb_sys = 0;
            (*(s8 *)((u8 *)&lb_sys + 0x88)) = 0x14;
            (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
        }
        goto block_106;
    case 16:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s1 & 0xFFFF & 0x20) {
            (*(u8 *)&D_0035EFEB) = 1;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0xF;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_106;
    case 17:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s1 & 0xFFFF & 0x20) {
            (*(u8 *)&D_0035EFEB) = 1;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 3;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_106;
    case 18:                                        /* switch 1 */
        if ((((s8) (Lb_talk_check_default(0)))) != 0) {
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 3;
            (*(u8 *)&D_0035EFEB) = 1;
        }
        goto block_106;
    case 19:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (Lbs_RoomExit((*(s8 *)((u8 *)&lb_sys + 0x1C))) == 1) {
            SetDialogData(0x68, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            var_v0 = 0x10;
            goto block_105;
        }
        goto block_106;
    }
}

