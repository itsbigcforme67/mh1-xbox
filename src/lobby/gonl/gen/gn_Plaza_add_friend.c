/* G online (PC): Plaza_add_friend, written from Monster Hunter G's code at lobby 0x5714A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_select();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
s32 func_00235C80();
s32 lobby_func_0056C350();
s32 lobby_func_0056C3F0();
s32 lobby_func_00571450();
s32 memcmp();
void * memcpy();
void * memset();
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 lb_sys;
extern u8 pNet;
extern u8 tl_member_buff;
s32 SaveNetFile_ForLobby(); /* extern */
s32 net_Check_FriendData(); /* extern */
s32 net_Check_FriendFree();       /* extern */
extern s32 g_main_302CB0;
extern s32 my_user_id;

s32 Plaza_add_friend(void) {
    s16 temp_v0_3;
    s32 temp_a4;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 temp_v1_2;
    s32 var_s0;
    u8 temp_a0;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_s2;
    u8 *temp_s2_2;
    u8 *temp_v1;
    u8 *temp_v1_3;

    temp_a4 = Get_sw2(0) & 0xFFFF;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_a2 = (*(void **)&pNet);
    temp_a0 = (*(u8 *)((u8 *)(temp_a2) + 0x25));
    temp_a3 = temp_a2 + 0x25;
    switch (temp_a0) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x615)) = 1;
        temp_v0 = Lb_select((s32) temp_a0, 1, (s32) temp_a2, (s32) temp_a3);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x25)) + 1);
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x615)) = 0;
            if (memcmp((*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)), &my_user_id, 8) == 0) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 5U;
                SetDialogData(0x24, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else if (net_Check_FriendData(&g_main_302CB0, 0x32, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600))) == -1) {
                if (net_Check_FriendFree(&g_main_302CB0, 0x32) == -1) {
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 5U;
                    SetDialogData(0x2A, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                } else {
                    SetDialogData(0x29, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                }
            } else {
                SetDialogData(0x28, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
            break;
        case 3:                                     /* switch 2 */
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 5U;
            SetDialogData(0x25, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x615)) = 0;
            break;
        }
    default:                                        /* switch 1 */
block_49:
        return 2;
    case 1:                                         /* switch 1 */
        temp_v0_2 = Lb_select((s32) temp_a0, 1, (s32) temp_a2, (s32) temp_a3);
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            var_s0 = net_Check_FriendData(&g_main_302CB0, 0x32, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)));
            if (var_s0 == -1) {
                var_s0 = net_Check_FriendFree(&g_main_302CB0, 0x32);
                if (var_s0 == -1) {
                    var_s0 = 0;
                }
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 0;
            } else {
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 1;
            }
            temp_s2 = (u8 *)&g_main_302CB0 + (var_s0 * 0x30);
            memset(temp_s2, 0, 8);
            memcpy(temp_s2, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)), 8);
            temp_s2_2 = temp_s2 + 8;
            memset(temp_s2_2, 0, 0x10);
            memcpy(temp_s2_2, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) + 8, 0x10);
            temp_v1_2 = var_s0 * 0x24A;
            (*(s8 *)((u8 *)((temp_v1_2 + (*(void **)&cw))) + 0x1265)) = 0;
            (*(s8 *)((u8 *)((temp_v1_2 + (*(void **)&cw))) + 0x1064)) = 0;
            (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1A)) = (s16) ((s32) (lobby_func_0056C350(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) << 0x38) >> 0x38);
            temp_v1_3 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_3) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x25)) + 1);
            func_00235C80(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        case 3:                                     /* switch 3 */
            lobby_func_00571450(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
            break;
        }
        goto block_49;
    case 2:                                         /* switch 1 */
        temp_v0_3 = (*(s16 *)((u8 *)(temp_a2) + 0x1A));
        if (temp_v0_3 == 0) {
            temp_v0_4 = SaveNetFile_ForLobby(temp_a0, 1, temp_a2, temp_a3, temp_a4);
            if ((temp_v0_4 != 1) && (temp_v0_4 != -1)) {
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                goto block_49;
            }
            lobby_func_0056C3F0(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
            return 0;
        }
        (*(s16 *)((u8 *)(temp_a2) + 0x1A)) = (s16) (temp_v0_3 - 1);
        goto block_49;
    case 3:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_a2) + 0x25)) = (u8) (temp_a0 + 1);
        /* fallthrough */
    case 4:                                         /* switch 1 */
        if (temp_a4 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
            cnWrap_SoundRequest(0, 1, (s32) temp_a2, (s32) temp_a3);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
            (*(s8 *)&D_0035EFEB) = 1;
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
                (*(s8 *)((u8 *)&lb_sys + 0x94)) = 1;
            }
            return 0;
        }
        goto block_49;
    case 5:                                         /* switch 1 */
        if (temp_a4 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)(temp_a2) + 0x25)) = 0U;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
            cnWrap_SoundRequest(0, 1, (s32) temp_a2, (s32) temp_a3);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
            (*(u8 *)&D_0035EFEB) = 1;
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
                (*(s8 *)((u8 *)&lb_sys + 0x94)) = 1;
            }
            return 1;
        }
        goto block_49;
    }
}

