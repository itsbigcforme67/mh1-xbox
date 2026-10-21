/* G online (PC): lobby_func_00569610, written from Monster Hunter G's code at lobby 0x569610 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EF390;
extern u8 Psw;
extern u8 g_main_2EF390;
extern u8 jtbl_0063DF50;
extern u8 pNet;
s16 Get_sw2();                               /* extern */
s32 Name_ID_change();                           /* extern */
s32 Plaza_chat_init();                          /* extern */
s32 SetHelpLineMsg();                   /* extern */
s32 cnWrap_SoundRequest();               /* extern */
s32 func_0012D080();                         /* extern */
s32 lobby_func_00569D10();             /* extern */
s32 lobby_func_0056AFF0();             /* extern */
s32 lobby_func_0056B4B0();             /* extern */
s32 lobby_func_0056B730();             /* extern */
s32 lobby_func_0056C480();                      /* extern */
s32 lobby_func_0056ED00();             /* extern */
s32 lobby_func_0056FF80();             /* extern */
s32 lobby_func_00571AF0();             /* extern */
s32 lobby_func_0059F6C0();                   /* extern */
s32 lobby_func_0059F740();               /* extern */
s32 plaza_moveMain();                  /* extern */
s32 plaza_setMyComment();                /* extern */
s32 tl_menu_cursor_down();               /* extern */
s32 tl_menu_cursor_up();              /* extern */
extern u8 ClassInfo;
extern u8 g_lb_5FBCC0;
extern u8 lb_ibox_str;
extern u8 tl_etc;
extern u8 g_main_2EEE70;
extern u8 g_main_3064AF;

void lobby_func_00569610(void) {
    s16 temp_a0_5;
    s16 temp_v0;
    s32 *temp_a0;
    s32 temp_a1_2;
    s32 temp_a1_3;
    s32 temp_s0;
    s32 temp_s1;
    u8 temp_v1_2;
    u8 temp_v1_3;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a0_4;
    u8 *temp_a1;
    u8 *temp_v1;
    u8 *temp_v1_4;

    temp_s1 = *(&g_main_2EEE70 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) * 4));
    temp_v0 = Get_sw2(0);
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = temp_v0;
    lobby_func_0059F6C0(tl_etc);
    temp_s0 = temp_v0 & 0xFFFF & 0xFFFF;
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = 0x10U;
    if (temp_s0 & 0x2000) {
        tl_menu_cursor_up((s32 *)0x10);
        SetHelpLineMsg((*(u8 *)((u8 *)((((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2A)) * 4) + temp_s1)) + 1)) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (temp_s0 & 0x1000) {
        tl_menu_cursor_down(0x10);
        SetHelpLineMsg((*(u8 *)((u8 *)((((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2A)) * 4) + temp_s1)) + 1)) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (temp_s0 & 0xC00) {
        temp_a1 = (*(void **)&pNet);
        (*(u8 *)((u8 *)(temp_a1) + 0x29)) = (u8) ((*(u8 *)((u8 *)(temp_a1) + 0x29)) ^ 1);
        temp_v1 = (*(void **)&pNet);
        temp_a1_2 = (*(u8 *)((u8 *)(temp_v1) + 0x29)) * 4;
        temp_a0 = &g_main_2EEE70 + temp_a1_2;
        if (*(*temp_a0 + ((*(u8 *)((u8 *)(temp_v1) + 0x2A)) * 4)) != 1) {
            tl_menu_cursor_up(temp_a0, temp_a1_2);
        }
        SetHelpLineMsg((*(u8 *)((u8 *)((((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2A)) * 4) + M2C_ERROR(/* Read from unset register $t2 */))) + 1)) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (temp_s0 & 0x20) {
        temp_a0_2 = (*(void **)&pNet);
        (*(u8 *)((u8 *)(temp_a0_2) + 0x28)) = (u8) (*(u8 *)((u8 *)((((*(u8 *)((u8 *)(temp_a0_2) + 0x2A)) * 4) + temp_s1)) + 1));
        temp_a0_3 = (*(void **)&pNet);
        (*(u8 *)((u8 *)(temp_a0_3) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0x22)) + 1);
        temp_a0_4 = (*(void **)&pNet);
        temp_v1_2 = (*(u8 *)((u8 *)(temp_a0_4) + 0x28));
        if ((temp_v1_2 != 0xC) && (temp_v1_2 != 0xF)) {
            lobby_func_0059F6C0(*(&lb_ibox_str + ((*(s16 *)((u8 *)((((*(u8 *)((u8 *)(temp_a0_4) + 0x2A)) * 4) + temp_s1)) + 2)) * 4)));
        }
        cnWrap_SoundRequest(0);
        temp_a1_3 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) * 2;
        temp_a0_5 = *(&g_lb_5FBCC0 + temp_a1_3);
        if (temp_a0_5 != -1) {
            SetHelpLineMsg(temp_a0_5, temp_a1_3);
        }
        temp_v1_3 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
        switch (temp_v1_3) {
        case 3:
        case 4:
            lobby_func_00571AF0(&jtbl_0063DF50);
            lobby_func_0056C480();
            break;
        case 1:
            lobby_func_0056AFF0(&jtbl_0063DF50);
            break;
        case 5:
            lobby_func_0056ED00(&jtbl_0063DF50);
            lobby_func_0056C480();
            break;
        case 6:
            lobby_func_0056FF80(&jtbl_0063DF50);
            break;
        case 0:
            lobby_func_00569D10(&jtbl_0063DF50);
            if (*(&g_main_3064AF + (ClassInfo * 0x15C)) == 0x31) {
                lobby_func_0059F740((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x788, ClassInfo);
            } else {
                lobby_func_0059F740((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x888, ClassInfo);
            }
            break;
        case 2:
        case 7:
        case 11:
            plaza_moveMain(&jtbl_0063DF50);
            break;
        case 14:
            lobby_func_0056B730(&jtbl_0063DF50);
            break;
        case 15:
            lobby_func_0056B4B0(&jtbl_0063DF50);
            break;
        case 8:
            plaza_setMyComment(0);
            break;
        }
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = 0;
        return;
    }
    if (((*(u16 *)((u8 *)&Psw + 0x4)) & 0x100) || (func_0012D080(0x10) == 1)) {
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = 0x10U;
        cnWrap_SoundRequest(0xE);
        temp_v1_4 = (*(void **)&pNet);
        (*(u8 *)((u8 *)(temp_v1_4) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x22)) + 1);
        Plaza_chat_init();
        return;
    }
    if (temp_s0 & 0x200) {
        Name_ID_change();
    }
}

