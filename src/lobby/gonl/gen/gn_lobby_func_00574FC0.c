/* G online (PC): lobby_func_00574FC0, written from Monster Hunter G's code at lobby 0x574FC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 pNet;
extern u8 tl_member_buff;
s32 disp_status(); /* extern */
s32 flfntSetSize();             /* extern */
s32 lobby_func_005737C0();                   /* extern */
s32 lobby_func_005775C0();     /* extern */
s32 lobby_func_005778B0();              /* extern */
s32 net_Check_FriendSuu();        /* extern */
s32 put_mainWindow();                        /* extern */
s32 put_mainWindowTex();                     /* extern */
extern s32 g_lb_636B4C;
extern s32 g_main_302CB0;

void lobby_func_00574FC0(s32 arg0, s32 arg1, s64 arg2) {
    s32 temp_a4;
    s64 temp_a2;
    u8 *temp_a2_2;

    temp_a2 = ((s8) (arg2));
    switch (temp_a2) {                              /* irregular */
    case 0:
        put_mainWindow(temp_a2);
        break;
    case 1:
        put_mainWindowTex(temp_a2);
        break;
    }
    if (((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) != 0) || ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) == 0xF)) {
        return;
    }
    flfntSetSize(0x12, 0x12);
    if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) == 0xC) {
        lobby_func_005778B0(arg0, arg1);
        return;
    }
    if (net_Check_FriendSuu(&g_main_302CB0, 0x32) == 0) {
        flfntSetSize(0x14, 0x14);
        lobby_func_005737C0(g_lb_636B4C);
        return;
    }
    if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) == 5) {
        temp_a4 = (*(s32 *)&cw);
        temp_a2_2 = (u8 *)&g_main_302CB0 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x30);
        disp_status(arg0, arg1, temp_a2_2, temp_a2_2 + 8, temp_a4 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x24A) + 0x101C, (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x614)), 3, temp_a4 + 0xAF8, (s64) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)));
        return;
    }
    lobby_func_005775C0(arg0, arg1, (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)), (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)));
}

