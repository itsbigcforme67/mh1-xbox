/* G online (PC): lobby_func_0057D2E0, written from Monster Hunter G's code at lobby 0x57D2E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_eat_to_bell();
s32 Lb_select();
s32 Lbs_ExitAndEnterPlaza();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 To_EnterPlaza();
s32 To_LogOut();
s32 cnWrap_SoundRequest();
s32 fade_set();
s32 lbc_top_menu_00();
s32 lbc_top_menu_02();
s32 lobby_func_0057DDC0();
s32 lobby_func_0059F740();
s32 lobby_func_0059F750();
s32 str_fadeout();
s32 str_stop();
extern u8 Psw;
extern u8 cw;
extern u8 pNet;
void lobby_func_0057D2E0(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    u8 temp_a1;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_v1 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_v1) + 0xB9A));
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9A)) = (u8) (temp_a1 + 1);
        lbc_top_menu_00();
        break;
    case 1:                                         /* switch 1 */
        temp_v0 = Lbs_ExitAndEnterPlaza();
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1_2 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_2) + 0xB9A)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9A)) + 1);
            lobby_func_0059F740();
            lobby_func_0059F750(0xFA, 0x16A);
            break;
        case 1:                                     /* switch 2 */
            To_LogOut(4);
            break;
        }
        break;
    case 2:                                         /* switch 1 */
        temp_v0_2 = lobby_func_0057DDC0();
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            temp_v1_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_3) + 0xB9A)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9A)) + 1);
            SetDialogData(0xE);
            break;
        case 1:                                     /* switch 3 */
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 5U;
            break;
        case 3:                                     /* switch 3 */
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 4U;
            SetDialogData(0x36);
            SetDialogYesNo(1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            break;
        }
        break;
    case 3:                                         /* switch 1 */
        temp_v0_3 = lbc_top_menu_02();
        switch (temp_v0_3) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            To_EnterPlaza();
            str_fadeout(0, 0xF);
            break;
        case 1:                                     /* switch 4 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D));
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 6U;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            break;
        }
        break;
    case 4:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_4 = Lb_select();
        switch (temp_v0_4) {                        /* switch 5; irregular */
        case 0:                                     /* switch 5 */
            To_LogOut(0);
            fade_set(0xA);
            str_stop(0);
            str_stop(1);
            break;
        case 3:                                     /* switch 5 */
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 2U;
            break;
        }
        break;
    case 5:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if ((*(u16 *)((u8 *)&Psw + 0x4)) & 0x20) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 2U;
            cnWrap_SoundRequest(6);
        }
        break;
    case 6:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if ((*(u8 *)((u8 *)&Psw + 0x4)) & 0x20) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0U;
            SetDialogData(0x19);
            cnWrap_SoundRequest(6);
        }
        break;
    default:                                        /* switch 1 */
        To_LogOut(4);
        break;
    }
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) != 2) {
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    }
}

