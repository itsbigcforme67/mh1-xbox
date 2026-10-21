/* G online (PC): lobby_func_00574D70, written from Monster Hunter G's code at lobby 0x574D70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 BsProxyUrlstr;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Draw_menu_square();    /* extern */
s32 Draw_square();    /* extern */
s32 flfntSetSize();             /* extern */
s32 lobby_func_005737C0();                   /* extern */
s32 lobby_func_00574750();          /* extern */
s32 lobby_func_005748B0();     /* extern */
s32 lobby_func_00574AE0();          /* extern */
s32 lobby_func_00574CB0();              /* extern */
s32 lobby_func_005C14D0();        /* extern */
s32 put_mainWindow();                           /* extern */
s32 put_mainWindowTex();                        /* extern */
extern u8 g_lb_636B4C;
extern u8 g_lb_636B58;
extern u8 g_lb_636B5C;

void lobby_func_00574D70(s64 arg0, s64 arg1, s64 arg2) {
    s64 temp_s3;
    s64 temp_v1;

    temp_v1 = (s64) (arg2 << 0x38) >> 0x38;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) == 2) {
            Draw_menu_square((s64) ((((s64) (arg0 << 0x30) >> 0x30) - 6) << 0x30) >> 0x30, 0x1A0, 0x110);
            temp_s3 = (s64) (arg1 << 0x30) >> 0x30;
            Draw_square(arg0, (s64) ((temp_s3 + 0x26) << 0x30) >> 0x30, 0x192, 1);
            Draw_square(arg0, (s64) ((temp_s3 + 0xEC) << 0x30) >> 0x30, 0x192, 1);
        } else {
            put_mainWindow();
        }
        break;
    case 1:                                         /* switch 1 */
        put_mainWindowTex();
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
        lobby_func_00574750(arg0, arg1, (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)));
        return;
    case 1:                                         /* switch 2 */
        lobby_func_00574CB0(arg0, arg1);
        if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
            flfntSetSize(0x14, 0x14);
            lobby_func_005737C0(g_lb_636B58);
            return;
        }
        lobby_func_005748B0(arg0, arg1, (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x610)), (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)));
        return;
    case 2:                                         /* switch 2 */
        lobby_func_00574CB0(arg0, arg1);
        if (lobby_func_005C14D0(&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8) == 0) {
            lobby_func_005737C0(g_lb_636B5C);
            return;
        }
        lobby_func_00574AE0(arg0, arg1, (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)));
        return;
    }
}

