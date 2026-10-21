/* G online (PC): lobby_func_00571040, written from Monster Hunter G's code at lobby 0x571040 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 Lb_select();
s32 SaveNetFile_ForLobby();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
s32 func_00235C80();
s32 lobby_func_0056C350();
s32 lobby_func_0056C3F0();
s32 lobby_func_00570FF0();
s32 lobby_func_005C1510();
s32 lobby_func_005C1560();
s32 memcmp();
void * memcpy();
void * memset();
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 lb_sys;
extern u8 pNet;
extern u8 tl_member_buff;
extern s32 g_main_302290;
extern u8 my_user_id;

s32 lobby_func_00571040(void) {
    s16 temp_v0_3;
    s32 temp_a4;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 var_v0;
    u8 temp_a0;
    u8 *temp_a2;
    u8 *temp_s1;
    u8 *temp_s1_2;
    u8 *temp_v1;
    u8 *temp_v1_2;

    temp_a4 = Get_sw2(0) & 0xFFFF;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_a2 = (*(void **)&pNet);
    temp_a0 = (*(u8 *)((u8 *)(temp_a2) + 0x25));
    switch (temp_a0) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x615)) = 1;
        temp_v0 = Lb_select();
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x25)) + 1);
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x615)) = 0;
            if (memcmp((*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)), &my_user_id, 8) == 0) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 5U;
                SetDialogData(0x24);
            } else if (lobby_func_005C1560(&g_main_302290, 8, (s32) (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600))) == -1) {
                if (lobby_func_005C1510(&g_main_302290, 8) == -1) {
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 5U;
                    SetDialogData(0x32);
                } else {
                    SetDialogData(0x31);
                    SetDialogYesNo(0);
                }
            } else {
                SetDialogData(0x30);
                SetDialogYesNo(0);
            }
            break;
        case 3:                                     /* switch 2 */
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 5U;
            SetDialogData(0x2D);
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x615)) = 0;
            break;
        }
    default:                                        /* switch 1 */
block_49:
        return 2;
    case 1:                                         /* switch 1 */
        temp_v0_2 = Lb_select();
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            var_v0 = lobby_func_005C1560(&g_main_302290, 8, (s32) (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)));
            if (var_v0 == -1) {
                var_v0 = lobby_func_005C1510(&g_main_302290, 8);
                if (var_v0 == -1) {
                    var_v0 = 0;
                }
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 0;
            } else {
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 1;
            }
            temp_s1 = (u8 *)&g_main_302290 + (var_v0 * 0x18);
            memset(temp_s1, 0, 8);
            memcpy(temp_s1, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)), 8);
            temp_s1_2 = temp_s1 + 8;
            memset(temp_s1_2, 0, 0x10);
            memcpy(temp_s1_2, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) + 8, 0x10);
            (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1A)) = (s16) (((s8) (lobby_func_0056C350())));
            temp_v1_2 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_2) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x25)) + 1);
            func_00235C80();
            break;
        case 3:                                     /* switch 3 */
            lobby_func_00570FF0();
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
            break;
        }
        goto block_49;
    case 2:                                         /* switch 1 */
        temp_v0_3 = (*(s16 *)((u8 *)(temp_a2) + 0x1A));
        if (temp_v0_3 == 0) {
            temp_v0_4 = SaveNetFile_ForLobby();
            if ((temp_v0_4 != 1) && (temp_v0_4 != -1)) {
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                goto block_49;
            }
            lobby_func_0056C3F0();
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
            cnWrap_SoundRequest(0);
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
            cnWrap_SoundRequest(0);
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

