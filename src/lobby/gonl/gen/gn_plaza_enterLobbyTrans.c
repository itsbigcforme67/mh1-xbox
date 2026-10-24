/* HAND-FIXED (the switch's empty cases). G online (PC): plaza_enterLobbyTrans, written from Monster Hunter G's code at lobby 0x573DA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_page_num();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_print_double();
s32 font_set_palette();
s32 lobby_func_005737C0();
s32 lobby_func_00576A70();
s32 lobby_func_005791D0();
s32 put_mainWindow();
s32 put_main_cursor2();
s32 put_member_info();
extern u8 ClassInfo;
extern u8 lb_quest_all;
extern u8 pNet;
extern u8 tl_msg_tbl;
s32 font_print(); /* extern */
s32 func_0012BDD0();        /* extern */
s32 sprintf(); /* extern */
extern void *g_lb_636B40;
extern s32 g_lb_63E1F0;
extern s32 g_lb_63E210;
extern s32 g_lb_63E230;
extern u8 LobbyInfo;
extern u8 lit_2316;
extern u8 tl_member_buff;

void plaza_enterLobbyTrans(s32 arg0, s32 arg1) {
    u8 g_frame[0x120] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s32 *)(g_frame + 0xA0))
#define spE0 (*(s32 *)(g_frame + 0xE0))
    u8 *var_s2;   /* (s32 *) */
    s16 temp_s1;
    s16 var_a3;
    s32 var_s3;
    s32 var_s5;
    s32 var_s7;
    s32 temp_fp;
    s32 temp_s0;
    s32 temp_s6;
    s32 var_s0_2;
    s32 var_s0_3;
    s32 var_s1;
    s32 var_s4;
    s32 var_s4_2;
    s8 *var_s0;
    u8 temp_a0;
    u8 temp_v1;
    u8 *temp_a1;
    u8 *temp_v0;
    u8 *var_s3_2;

    var_s5 = 0;
    put_mainWindow(arg0, arg1);
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        flfntSetSize(0x12, 0x12);
        temp_a1 = (*(void **)&pNet);
        temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0x23));
        switch (temp_a0) {                          /* switch 1; irregular */
        case 5:                                     /* switch 1 */
            lobby_func_00576A70();
            return;
        case 3:                                     /* switch 1 */
            lobby_func_005791D0(0);
            var_s4 = ((s16) ((arg1 + 0x3E)));
            if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) == 0) {
                flfntSetSize(0x16, 0x12);
                font_set_palette(4);
                lobby_func_005737C0(g_lb_636B40);
                return;
            }
            var_s3 = 0;
            var_s0 = (u8 *)&(*(u8 *)((u8 *)&lb_quest_all + 0xA0)) + ((*(s16 *)((u8 *)(&tl_member_buff) + 6)) * 0x2E0);
loop_12:
            if ((var_s3 + ((*(s16 *)((u8 *)(&tl_member_buff) + 6)) * 8)) < 0xA) {
                if (*var_s0 != 0) {
                    put_member_info((s32) (((s16) (((((s16) (arg0))) + 0xA)))), (s32) var_s4, var_s0, var_s0 + 8, var_s0 + 0x1C, 1);
                }
                var_s3 += 1;
                var_s4 = ((s16) ((var_s4 + 0x16)));
                var_s0 += 0x5C;
                if (var_s3 >= 8) {

                } else {
                    goto loop_12;
                }
            }
            if ((*(s16 *)((u8 *)(&tl_member_buff) + 8)) >= 2) {
                Put_page_num(0x208, 0x5C, (s32) (*(s16 *)((u8 *)(&tl_member_buff) + 6)), (s32) (*(s16 *)((u8 *)(&tl_member_buff) + 8)), 0);
                return;
            }
            break;
        default:                                    /* switch 1 */
            put_main_cursor2(arg0, arg1, (s32) (*(u8 *)((u8 *)(temp_a1) + 0x2B)));
            lobby_func_005791D0(2);
            temp_fp = ((s16) (arg0));
            var_s4_2 = ((s16) ((arg1 + 0x3E)));
            temp_s6 = temp_fp + 0xA;
            var_s7 = (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) * 7;
            var_s3_2 = (u8 *)&LobbyInfo + (var_s7 * 0x15C);
            do {
                if (var_s7 < (s32) (*(u16 *)((u8 *)&ClassInfo + 0x6))) {
                    temp_s1 = (*(s16 *)((u8 *)(var_s3_2) + 0x12));
                    var_s0_2 = ((s16) (((*(s16 *)((u8 *)(var_s3_2) + 6)) - temp_s1)));
                    if (var_s0_2 < 0) {
                        var_s0_2 = 0;
                    }
                    sprintf(&spA0, &lit_2316, var_s3_2 + 0x1A);
                    if ((u16) (*(s16 *)((u8 *)(var_s3_2) + 6)) == 0) {
                        var_s1 = ((s16) ((s32) temp_s1));
                        var_s0_3 = ((s16) (var_s0_2));
                        var_s2 = var_s0_3 + var_s1;
                        sprintf(&spE0, &g_lb_63E1F0, var_s2, (*(u16 *)((u8 *)(var_s3_2) + 8)), var_s0_3, var_s1);
                    } else {
                        var_s1 = ((s16) ((s32) temp_s1));
                        var_s0_3 = ((s16) (var_s0_2));
                        var_s2 = var_s0_3 + var_s1;
                        sprintf(&spE0, &g_lb_63E210, var_s2, (*(u16 *)((u8 *)(var_s3_2) + 8)), var_s0_3, var_s1);
                    }
                    temp_v1 = (*(u8 *)((u8 *)(var_s3_2) + 0x16));
                    switch (temp_v1) {              /* switch 2; irregular */
                    case 4:                         /* switch 2 */
                    case 3:                         /* switch 2 */
                        var_a3 = (*(s16 *)((u8 *)(var_s3_2) + 0));
                        break;
                    case 2:                         /* switch 2 */
                        sprintf(&spA0, &lit_2316, (*(s32 * *)((u8 *)&tl_msg_tbl + 0x8)));
                        /* fallthrough */
                    default:                        /* switch 2 */
                        var_a3 = 0xA;
                        break;
                    }
                    if (var_s5 == (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B))) {
                        font_print_double((s32) (((s16) (temp_s6))), (s32) var_s4_2, 1, (s32) var_a3, &spA0);
                        flfntLocate(0x1B8, (s32) (((s16) (((((s16) (var_s4_2))) + 2)))));
                        font_set_palette(1);
                        font_print(&g_lb_63E230, var_s2, (*(u16 *)((u8 *)(var_s3_2) + 8)), var_s0_3, var_s1);
                        flfntLocate(0x1B6, (s32) var_s4_2);
                        func_0012BDD0(&lit_2316, &spE0);
                    } else {
                        font_set_palette((s32) var_a3);
                        flfntLocate((s32) (((s16) (temp_s6))), (s32) var_s4_2);
                        font_print(&lit_2316, &spA0);
                        flfntLocate(0x1B6, (s32) var_s4_2);
                        func_0012BDD0(&lit_2316, &spE0);
                    }
                }
                var_s5 += 1;
                var_s4_2 = ((s16) ((var_s4_2 + 0x16)));
                var_s3_2 += 0x15C;
                var_s7 += 1;
            } while (var_s5 < 7);
            temp_s0 = temp_fp + 0x13C;
            flfntLocate((s32) (((s16) (temp_s0))), (s32) var_s4_2);
            temp_v0 = (*(void **)&pNet);
            Put_page_num((s32) (((s16) (temp_s0))), (s32) var_s4_2, (s32) (*(s16 *)((u8 *)(temp_v0) + 0x1C)), (s32) (*(s16 *)((u8 *)(temp_v0) + 0x1E)), 0);
            break;
        case 0: case 2: case 4:                     /* HAND-FIXED: nothing drawn (m2c put them in a dangling else) */
            break;
        }
    }
}

