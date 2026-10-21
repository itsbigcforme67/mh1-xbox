/* G online (PC): lobby_func_00574D70, written from Monster Hunter G's code at lobby 0x574D70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Draw_square();
s32 flfntSetSize();
s32 lobby_func_005737C0();
s32 lobby_func_00574750();
s32 lobby_func_005748B0();
s32 lobby_func_00574AE0();
s32 lobby_func_00574CB0();
s32 lobby_func_005C14D0();
s32 put_mainWindow();
s32 put_mainWindowTex();
extern u8 BsProxyUrlstr;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Draw_menu_square(); /* extern */
extern void *g_lb_636B4C;
extern void *g_lb_636B58;
extern void *g_lb_636B5C;

void lobby_func_00574D70(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_s3;
    s32 temp_v1;

    temp_v1 = ((s8) (arg2));
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) == 2) {
            Draw_menu_square(((s16) (((((s16) (arg0))) - 6))), 0x1A0, 0x110, 1, 0xB0462000);
            temp_s3 = ((s16) (arg1));
            Draw_square((s32) arg0, (s32) (((s16) ((temp_s3 + 0x26)))), 0x192, 1, -0x71A3D4);
            Draw_square((s32) arg0, (s32) (((s16) ((temp_s3 + 0xEC)))), 0x192, 1, -0x71A3D4);
        } else {
            put_mainWindow(arg0, arg1);
        }
        break;
    case 1:                                         /* switch 1 */
        put_mainWindowTex(arg0, arg1);
        break;
    }
    if (((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) != 0) || ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) == 0xF)) {
        return;
    }
    flfntSetSize(0x12, 0x12);
    switch ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C))) {  /* switch 2; irregular */
    case 3:                                         /* switch 2 */
        if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
            flfntSetSize(0x14, 0x14);
            lobby_func_005737C0(g_lb_636B4C);
            return;
        }
        lobby_func_00574750(arg0, arg1, (s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)));
        return;
    case 1:                                         /* switch 2 */
        lobby_func_00574CB0(arg0, arg1);
        if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
            flfntSetSize(0x14, 0x14);
            lobby_func_005737C0(g_lb_636B58);
            return;
        }
        lobby_func_005748B0(arg0, arg1, (s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x610)), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)));
        return;
    case 2:                                         /* switch 2 */
        lobby_func_00574CB0(arg0, arg1);
        if (lobby_func_005C14D0(&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8) == 0) {
            lobby_func_005737C0(g_lb_636B5C);
            return;
        }
        lobby_func_00574AE0(arg0, arg1, (s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)));
        return;
    }
}

