/* G online (PC): lobby_func_0057E990, written from Monster Hunter G's code at lobby 0x57E990 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 pNet;
s32 CallBackWaitInit();                         /* extern */
s32 Check_CallBackWait(); /* extern */
s32 Get_sw();                                /* extern */
s32 Init_InterruptFlag(); /* extern */
s32 Lb_eat_to_bell();                     /* extern */
s32 Lb_put_inputMsgForHTML();                 /* extern */
s32 Lbc_init_network_work(); /* extern */
s32 SetDialogData();                     /* extern */
s32 cnWrap_SoundRequest();               /* extern */
s32 font_print_double(); /* extern */
s32 font_set_stack_no();                 /* extern */
s32 lobby_func_005E3AB0();             /* extern */
s32 sprintf();         /* extern */
extern u8 CallBack_Result_Plaza_PlazaEntry2;
extern u8 g_lb_63E538;

void lobby_func_0057E990(void) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s32 *)(g_frame + 0x20))
    s32 temp_s0;
    u32 temp_v1;
    u8 temp_a0;
    u8 temp_a1;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a3;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_s0 = Get_sw(0) & 0xFFFF;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_a3 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_a3) + 0xB9C));
    switch (temp_a1) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        (*(u8 *)((u8 *)(temp_a3) + 0xB9C)) = (u8) (temp_a1 + 1);
        Lbc_init_network_work(2, temp_a1, 1, temp_a3);
        (*(u32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x12CU;
        Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xD;
        lobby_func_005E3AB0(&CallBack_Result_Plaza_PlazaEntry2);
    default:                                        /* switch 1 */
block_13:
        temp_a0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27));
        switch (temp_a0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a0_2 = (*(void **)&cw);
            temp_v1 = (*(u32 *)((u8 *)(temp_a0_2) + 0xBB4)) - 1;
            (*(u32 *)((u8 *)(temp_a0_2) + 0xBB4)) = temp_v1;
            if (temp_v1 == 0) {
                temp_a0_3 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_a0_3) + 0x27)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0x27)) + 1);
                return;
            }
            font_set_stack_no(5);
            temp_v1_2 = (*(void **)&cw);
            sprintf(&sp20, &g_lb_63E538, ((*(u32 *)((u8 *)(temp_v1_2) + 0xBB4)) / 30) + ((u32) (*(u32 *)((u8 *)(temp_v1_2) + 0xBB4)) >> 0x1F));
            font_print_double(0x1D6, 0x54, 1, 5);
            return;
        case 1:                                     /* switch 2 */
            Lb_put_inputMsgForHTML(temp_a0);
            if (temp_s0 & 0xFFFF & 0x20) {
                temp_v1_3 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_3) + 0x27)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x27)) + 1);
                SetDialogData(0xF);
                cnWrap_SoundRequest(0);
            }
            return;
        }
        break;
    case 0x1:                                       /* switch 1 */
        Check_CallBackWait(2, temp_a1, 1, temp_a3);
        goto block_13;
    case 0x2:                                       /* switch 1 */
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) == 2) {
            (*(s8 *)((u8 *)(temp_a3) + 0xB99)) = 1;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
            Init_InterruptFlag(2, temp_a1, 1, temp_a3);
            return;
        }
        goto block_13;
    case 0x63:                                      /* switch 1 */
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) == 2) {
            (*(s8 *)((u8 *)(temp_a3) + 0xB9A)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
            return;
        }
        goto block_13;
    }
}

