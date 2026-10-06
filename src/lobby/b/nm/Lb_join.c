#include "lobby_b.h"
extern s32 quest_price;
extern u32 joinQuest;
extern char Lb_join_trans[];
extern char User_data[];
extern char RoomRule[];
extern char RoomRule[];
extern char D_3E5506[];
extern char quest_title[];
extern char quest_title[];
extern u8 *npc_dialog_table[];
void Lb_join(void) {
    s8 sx1;
    s8 sx2;
    s8 sx3;
    s8 sx4;
    int temp_s0_2;
    s32 temp_s1;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s8 temp_a1;
    u32 temp_a0;
    u32 temp_v1_2;
    u8 temp_v1;
    u8 *temp_s0;
    u8 *temp_v0_7;
    LBQUEST *var_v0;

    temp_s1 = Get_sw2(0) & 0xFFFF;
    temp_s0 = (u8 *)Lbs_GetRoomInfo(pNet->sel);
    switch (lb_sys.x06) {          /* switch 1 */
    case 0:                                         /* switch 1 */
        if (Event_flag_ck(4) == 0) {
            Lb_put_set01(0xE);
            lb_sys.x68 = 0;
            lb_sys.x87 = 0x14;
            lb_sys.x6C = 0;
            lb_sys.x06 = 0;
            return;
        }
        lb_sys.x06 = (s8) (lb_sys.x06 + 1);
        flMemset(&lb_pit, 0, 0xC);
        lb_pit.pos = npc_dialog_table[0x63];
        Lbc_init_network_work();
        Lbc_set_prim(0, &Lb_join_trans, 0);
        F(s8, (u8 *)cw, 0x2C08) = 0;
    default:                                        /* switch 1 */
block_88:
        lb_join_talk();
        return;
    case 1:                                         /* switch 1 */
        temp_v0 = Lbc_ReadRoomInfo();
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            F(s8, (u8 *)cw, 0x2C08) = 1;
            lb_pit.x08 = 0;
            lb_pit.x0 = 0;
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            Gunner_wasure_ck(&User_data, 1);
            break;
        case 1:                                     /* switch 2 */
            F(s8, (u8 *)cw, 0x2C08) = 1;
            lb_sys.x06 = 0xF;
            *(s8 *)0x3F36AB = 0;
            break;
        }
        goto block_88;
    case 2:                                         /* switch 1 */
        if ((sx1 = Lb_talk_check_default(0)) != 0) {
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
        }
        goto block_88;
    case 3:                                         /* switch 1 */
        temp_v0_2 = lb_select_room_list();
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            F(s8, (u8 *)cw, 0x2C08) = 0;
            break;
        case 3:                                     /* switch 3 */
            lb_sys.x06 = 0xE;
            break;
        case 1:                                     /* switch 3 */
            *(u8 *)0x3F36AB = 0;
            lb_sys.x06 = 0xF;
            SetDialogData(0x32, 3);
            break;
        }
        goto block_88;
    case 4:                                         /* switch 1 */
        temp_v0_3 = Lbc_ReadRoomInfo();
        switch (temp_v0_3) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            break;
        case 1:                                     /* switch 4 */
            *(u8 *)0x3F36AB = 0;
            lb_sys.x06 = 0xF;
            break;
        }
        goto block_88;
    case 5:                                         /* switch 1 */
        if (Lbc_GuestReadRoom(lb_sys.x73) != 0) {

        } else {
            F(s8, (u8 *)cw, 0x2C08) = 1;
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            pNet->depth = 0;
            joinQuest = (u32) ((u32) (F(s32, Lbs_GetRoomInfo(lb_sys.x73), 0x158) & 0x1FE) >> 1);
            pNet->x12 = (s8) joinQuest;
            lb_select_set_data((u8) joinQuest);
        }
        goto block_88;
    case 6:                                         /* switch 1 */
        temp_v0_4 = lb_select_room();
        switch (temp_v0_4) {                        /* switch 5; irregular */
        case 0:                                     /* switch 5 */
            lb_pit.x0 = 0;
            lb_pit.x09 = 0;
            lb_pit.x08 = 1;
            *(u8 *)0x3F36AB = 0;
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            break;
        case 3:                                     /* switch 5 */
            lb_sys.x06 = 3;
            break;
        }
        goto block_88;
    case 7:                                         /* switch 1 */
        temp_v1 = F(u8, temp_s0, 0x10);
        switch (temp_v1) {                          /* switch 6; irregular */
        case 4:                                     /* switch 6 */
            Lb_put_set01(7);
block_46:
            lb_pit.x08 = 0;
            lb_pit.x0 = 0;
            lb_sys.x06 = 3;
            *(u8 *)0x3F36AB = 1;
            cnWrap_SoundRequest(3);
            break;
        default:                                    /* switch 6 */
            Lb_put_set01(6);
            goto block_46;
        case 3:                                     /* switch 6 */
            if ((sx2 = Lb_talk_check_default(0)) != 0) {
                if (lb_pit.x09 == 0) {
                    if (F(u8, (u8 *)cw, 0x35D3) != 0) {
                        lb_pit.x0 = 0;
                        lb_pit.x08 = 7;
                        lb_sys.x06 = 0xD;
                        *(u8 *)0x3F36AB = 1;
                    } else {
                        *(u8 *)0x3F36AB = 0;
                        lb_sys.x06 = (s8) (lb_sys.x06 + 1);
                    }
                } else {
                    lb_sys.x06 = 3;
                    *(u8 *)0x3F36AB = 1;
                }
            }
            break;
        }
        goto block_88;
    case 8:                                         /* switch 1 */
        if (check_room_require() == 0) {
            if (F(u8, temp_s0, 0x11) != 0) {
                lb_pit.x0 = 0;
                lb_pit.x08 = 2;
                lb_sys.x07 = 0;
                memset((int)&RoomRule + 2, 0, 0x10);
                *(u8 *)0x3F36AB = 0;
                lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            } else {
                lb_pit.x0 = 0;
                lb_pit.x08 = 3;
                lb_sys.x06 = 0xA;
                *(u8 *)0x3F36AB = 1;
                F(u32, &mhRule, 0x54) = (u32) joinQuest;
            }
        } else {
            lb_pit.x0 = 0;
            lb_sys.x06 = 0x10;
            lb_pit.x08 = 5;
            *(u8 *)0x3F36AB = 1;
        }
        goto block_88;
    case 9:                                         /* switch 1 */
        Lb_talk_check_default(1);
        temp_v0_5 = join_input_password((int)&RoomRule + 2);
        switch (temp_v0_5) {                        /* switch 7; irregular */
        case 0:                                     /* switch 7 */
            lb_pit.x08 = 3;
            lb_pit.x0 = 0;
            *(u8 *)0x3F36AB = 1;
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            F(u32, &mhRule, 0x54) = (u32) joinQuest;
            break;
        case 3:                                     /* switch 7 */
            lb_sys.x06 = 3;
            lb_pit.x08 = 0;
            *(u8 *)0x3F36AB = 1;
            lb_pit.x0 = 0;
            cnWrap_SoundRequest(3);
            break;
        }
        goto block_88;
    case 10:                                        /* switch 1 */
        F(s8, (u8 *)cw, 0x2C08) = 0;
        temp_v0_6 = Lbs_GuestEnterRoom();
        switch (temp_v0_6) {                        /* switch 8; irregular */
        case 0:                                     /* switch 8 */
            F(s8, (u8 *)cw, 0x2C08) = 1;
            temp_v1_2 = joinQuest;
            if (temp_v1_2 >= 0xC8) {
                var_v0 = get_quest_info(1);
            } else {
                var_v0 = lb_quest_all[temp_v1_2];
            }
            Lb_menu_quest_info(var_v0);
            temp_a1 = Lb_get_quest_type(var_v0) | 0x10;
            (*(int *)((u8 *)&D_3E5506 + (game_w.master * 0xA00))) = temp_a1;
            temp_v0_7 = (u8 *)Lbs_GetRoomInfo(lb_sys.x73);
            temp_a0 = joinQuest;
            *(s16 *)0x3F341C = (s16) temp_a0;
            *(s16 *)0x3F33DC = (s16) temp_a0;
            F(u32, &mhRule, 0x54) = temp_a0;
            *(s16 *)0x3F3608 = (s16) ((F(s32, temp_v0_7, 0x158) & 0x01FFFE00) >> 9);
            Lbc_SendMiniData(temp_a0);
            Lb_set_mini_data((u8 *)cw + (game_w.master * 0x2FC) + 0x1346);
            Lb_set_mini_data((int)&lbCommer + (game_w.master * 0x5C) + 0x1C);
            temp_s0_2 = F(int, var_v0, 0x18);
            *(u8 *)0x3F36AB = 0;
            lb_pit.x08 = 3;
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            lb_pit.x0 = 0;
            SetDialogData(0x31, 3);
            quest_price = 0;
            if (joinQuest >= 0xC8) {
                strcpy(&quest_title, Lb_get_quest_str(0));
            } else {
                strcpy(&quest_title, (*(s32 *)temp_s0_2));
            }
            break;
        case 1:                                     /* switch 8 */
            F(s8, (u8 *)cw, 0x2C08) = 1;
            lb_sys.x06 = 0xF;
            *(u8 *)0x3F36AB = 0;
            break;
        }
        goto block_88;
    case 11:                                        /* switch 1 */
        Lb_talk_check_default(1);
        pNet->x0C = 1;
        if (temp_s1 & 0xFFFF & 0x20) {
            *(u8 *)0x3F36AB = 1;
            lb_sys.x06 = (s8) (lb_sys.x06 + 1);
            cnWrap_SoundRequest(0);
        }
        goto block_88;
    case 12:                                        /* switch 1 */
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        lb_sys.x87 = 0x14;
        lb_sys.x06 = 0;
        Info_Initialization();
        goto block_88;
    case 13:                                        /* switch 1 */
        if ((sx3 = Lb_talk_check_default(0)) != 0) {
        case 14:                                    /* switch 1 */
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            lb_sys.x87 = 0x14;
            lb_sys.x6C = 0;
            Info_Initialization();
        }
        goto block_88;
    case 15:                                        /* switch 1 */
        pNet->x0C = 1;
        if (temp_s1 & 0xFFFF & 0x20) {
            *(u8 *)0x3F36AB = 1;
            lb_sys.x06 = 0xE;
            cnWrap_SoundRequest(3);
        }
        goto block_88;
    case 16:                                        /* switch 1 */
        if ((sx4 = Lb_talk_check_default(0)) != 0) {
            lb_sys.x06 = 3;
        }
        goto block_88;
    }
}
