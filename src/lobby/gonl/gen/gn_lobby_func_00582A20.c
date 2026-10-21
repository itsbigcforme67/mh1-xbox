/* G online (PC): lobby_func_00582A20, written from Monster Hunter G's code at lobby 0x582A20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Lobby_LobbyExit();
s32 Check_CallBackWait();
s32 Lb_eat_to_bell();
s32 Lb_put_inputMsgForHTML();
s32 Lbc_set_prim();
s32 SetDialogData();
s32 To_EnterPlaza2Lobby();
s32 To_LogOut();
s32 cnWrap_SoundRequest();
extern u8 cw;
extern u8 pNet;
s32 Fade_busy_ck(); /* extern */
s32 Get_sw();                                /* extern */
s32 cnLBS_LobbyExit();              /* extern */
s32 fade_set(); /* extern */
s32 font_print_double(); /* extern */
s32 font_set_stack_no();                 /* extern */
s32 sprintf();         /* extern */
extern s32 g_lb_63E538;

void lobby_func_00582A20(void) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s32 *)(g_frame + 0x20))
    s32 temp_s0;
    u32 temp_v1;
    u8 temp_a0_2;
    u8 temp_a2;
    u8 *temp_a0;
    u8 *temp_a0_3;
    u8 *temp_a0_4;
    u8 *temp_a3;
    u8 *temp_a4;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_s0 = Get_sw(0) & 0xFFFF;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_a3 = (*(void **)&cw);
    temp_a2 = (*(u8 *)((u8 *)(temp_a3) + 0xB9D));
    temp_a4 = temp_a3 + 0xB9D;
    switch (temp_a2) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        (*(u8 *)((u8 *)(temp_a3) + 0xB9D)) = (u8) (temp_a2 + 1);
        (*(u32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x12CU;
        Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D));
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 0U;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x26;
        cnLBS_LobbyExit(CallBack_Result_Lobby_LobbyExit);
        break;
    case 0x1:                                       /* switch 1 */
        Check_CallBackWait();
        break;
    case 0x2:                                       /* switch 1 */
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) == 2) {
            (*(s8 *)((u8 *)(temp_a3) + 0xFA61)) = 0;
            fade_set(1, 2, temp_a2, temp_a3, temp_a4);
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
        }
        break;
    case 0x3:                                       /* switch 1 */
        if ((Fade_busy_ck(1, 2, temp_a2, temp_a3, temp_a4) & 0xFF) != 1) {
            Lbc_set_prim(0, 0, 0, 0);
            To_EnterPlaza2Lobby(1);
        }
        break;
    case 0x63:                                      /* switch 1 */
        (*(u8 *)((u8 *)(temp_a3) + 0xB9D)) = 0U;
        To_LogOut(4);
        break;
    default:                                        /* switch 1 */
        To_LogOut(4);
        break;
    }
    temp_a0_2 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27));
    switch (temp_a0_2) {                            /* switch 2; irregular */
    case 0:                                         /* switch 2 */
        temp_a0_3 = (*(void **)&cw);
        temp_v1 = (*(u32 *)((u8 *)(temp_a0_3) + 0xBB4)) - 1;
        (*(u32 *)((u8 *)(temp_a0_3) + 0xBB4)) = temp_v1;
        if (temp_v1 == 0) {
            temp_a0_4 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_a0_4) + 0x27)) = (u8) ((*(u8 *)((u8 *)(temp_a0_4) + 0x27)) + 1);
            return;
        }
        font_set_stack_no(5);
        temp_v1_2 = (*(void **)&cw);
        sprintf(&sp20, &g_lb_63E538, ((*(u32 *)((u8 *)(temp_v1_2) + 0xBB4)) / 30) + ((u32) (*(u32 *)((u8 *)(temp_v1_2) + 0xBB4)) >> 0x1F));
        font_print_double(0x1D6, 0x54, 1, 5, &sp20);
        return;
    case 1:                                         /* switch 2 */
        Lb_put_inputMsgForHTML();
        if (temp_s0 & 0xFFFF & 0x20) {
            temp_v1_3 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_3) + 0x27)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x27)) + 1);
            SetDialogData(0xF);
            cnWrap_SoundRequest(0);
        }
        return;
    }
}

