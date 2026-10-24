/* G online (PC): lb_guild_make_room, written from Monster Hunter G's code at lobby 0x55B920 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_eat_to_bell();
s32 Lbc_DownloadQuest();
s32 Lbc_GetRoomRule();
s32 Lbc_ReadRoomInfo();
s32 Lbc_ReserveRoom();
s32 Lbc_SetPropaty();
s32 SetDialogData();
s32 cnWrap_SoundRequest();
s32 lobby_func_00559CE0();
void * memcpy();
s32 strcmp();
extern u8 D_0035EFEB;
extern u8 User_data;
extern u8 cw;
extern u8 game_w;
extern u8 lbShop;
extern u8 pNet;
extern u8 select_w;
s32 Lbc_SetRoomRule();                           /* extern */
void *get_quest_info();                             /* extern */
extern u8 g_lb_654830;
extern s32 RoomRule;
extern s32 lb_rule_member;
extern s8 mhRule;

s32 lb_guild_make_room(void) {
    u8 *var_s0;   /* (s32 *) */
    s16 temp_v0_4;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_5;
    s8 var_s1;

    temp_a0 = Get_sw2(0) & 0xFFFF;
    switch ((*(s8 *)((u8 *)&lbShop + 0x3))) {                           /* switch 1 */
    case 0:                                         /* switch 1 */
        if ((s32) (*(u16 *)((u8 *)&mhRule + 0x52)) < 0xC350) {
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = (s8) ((*(s8 *)((u8 *)(&g_lb_654830) + 7)) + 1);
        } else {
            temp_v0 = Lbc_DownloadQuest((s32) ((s32) (lobby_func_00559CE0((s32) (*(u16 *)((u8 *)&mhRule + 0x52)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) << 0x38) >> 0x38), (s32) (*(u16 *)((u8 *)&mhRule + 0x52)), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            switch (temp_v0) {                      /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                if ((*(u16 *)((u8 *)(get_quest_info()) + 0x1E)) != (*(u16 *)((u8 *)&mhRule + 0x52))) {
                    SetDialogData(0x54, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = 7;
                    (*(s8 *)&D_0035EFEB) = 0;
                } else {
                    (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = (s8) ((*(s8 *)((u8 *)(&g_lb_654830) + 7)) + 1);
                }
                break;
            case 1:                                 /* switch 2 */
                SetDialogData(0x54, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = 7;
                (*(u8 *)&D_0035EFEB) = 0;
                break;
            }
        }
    default:                                        /* switch 1 */
block_48:
        return 2;
    case 1:                                         /* switch 1 */
        temp_v0_2 = Lbc_ReadRoomInfo(temp_a0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = (s8) ((*(s8 *)((u8 *)(&g_lb_654830) + 7)) + 1);
            break;
        case 1:                                     /* switch 3 */
            Lb_eat_to_bell((*(void **)&cw) + 0xF75D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = 7;
            (*(u8 *)&D_0035EFEB) = 0;
            break;
        }
        goto block_48;
    case 2:                                         /* switch 1 */
        temp_v0_3 = Lbc_ReserveRoom(temp_a0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_3) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = (s8) ((*(s8 *)((u8 *)(&g_lb_654830) + 7)) + 1);
            break;
        case 1:                                     /* switch 4 */
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = 7;
            (*(u8 *)&D_0035EFEB) = 0;
            break;
        }
        goto block_48;
    case 3:                                         /* switch 1 */
        if (Lbc_GetRoomRule(temp_a0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */), M2C_ERROR(/* Read from unset register $a4 */)) == 1) {
            memcpy(&(*(s32 *)((u8 *)&RoomRule + 0x2)), &(*(s32 *)((u8 *)&mhRule + 0x8)), 9);
            var_s1 = 0;
            if ((s32) (*(u8 *)((u8 *)&RoomRule + 0x97)) > 0) {
                var_s0 = &RoomRule;
loop_30:
                if (strcmp((*(s32 *)((u8 *)&lb_rule_member + (mhRule * 4))), var_s0 + 0xBD) == 0) {
                    (*(s8 *)((u8 *)&RoomRule + 0x9A)) = var_s1;
                } else {
                    var_s1 += 1;
                    var_s0 += 0x41;
                    if (var_s1 < (s32) (*(u8 *)((u8 *)&RoomRule + 0x97))) {
                        goto loop_30;
                    }
                }
            }
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = (s8) ((*(s8 *)((u8 *)(&g_lb_654830) + 7)) + 1);
        }
        goto block_48;
    case 4:                                         /* switch 1 */
        if (Lbc_SetRoomRule(temp_a0) == 1) {
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = (s8) ((*(s8 *)((u8 *)(&g_lb_654830) + 7)) + 1);
        }
        goto block_48;
    case 5:                                         /* switch 1 */
        (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = (s8) ((*(s8 *)((u8 *)(&g_lb_654830) + 7)) + 1);
        temp_v0_4 = (*(s16 *)((u8 *)&User_data + 0x1A));
        (*(u16 *)((u8 *)&select_w + 0xD0)) = (*(u16 *)((u8 *)&mhRule + 0x52));
        (*(s16 *)((u8 *)&game_w + 0xFA)) = temp_v0_4;
        (*(s32 *)((u8 *)&mhRule + 0x58)) = ((*(s32 *)((u8 *)&mhRule + 0x58)) & 0xFFFF0000) | (*(u16 *)((u8 *)&mhRule + 0x52));
        (*(s32 *)((u8 *)&mhRule + 0x58)) = (u16) (*(s32 *)((u8 *)&mhRule + 0x58)) | ((temp_v0_4 & 0xFFFF) << 0x10);
        goto block_48;
    case 6:                                         /* switch 1 */
        temp_v0_5 = Lbc_SetPropaty((*(s32 *)((u8 *)&mhRule + 0x58)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_5) {                        /* switch 5; irregular */
        case 0:                                     /* switch 5 */
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF751)) = 1;
            return 0;
        case 1:                                     /* switch 5 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)(&g_lb_654830) + 7)) = 7;
            (*(u8 *)&D_0035EFEB) = 0;
            goto block_48;
        }
        break;
    case 7:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_a0 & 0xFFFF & 0x20) {
            (*(u8 *)&D_0035EFEB) = 1;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 1;
        }
        goto block_48;
    }
    return 2;
}

