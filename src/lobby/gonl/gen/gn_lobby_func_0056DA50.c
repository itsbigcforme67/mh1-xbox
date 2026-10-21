/* G online (PC): lobby_func_0056DA50, written from Monster Hunter G's code at lobby 0x56DA50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Eft25_set();
s32 Get_sw();
s32 Get_sw_on2();
s32 Lb_cursorUD();
s32 SetDialogData();
s32 cnWrap_SoundRequest();
void * memcpy();
void * memset();
s32 plaza_req_input();
void * strcpy();
extern u8 pNet;
extern u8 tl_member_buff;
s8 Visual_main();                          /* extern */
extern s32 g_lb_5FBC28;
extern s32 g_lb_5FBC29;
extern s32 g_lb_653A50;
extern s8 g_lb_6B46D0;
extern s8 g_lb_6B46D1;
extern s32 g_main_2EEE68;
extern s8 seekStr;

s32 lobby_func_0056DA50(void) {
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_s1;
    s32 var_a1;
    s8 var_v0_2;
    u8 var_v0;

    temp_s1 = Get_sw(0) & 0xFFFF;
    switch ((*(u8 *)((u8 *)&tl_member_buff + 0x1383))) {                           /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_a1 = temp_s1 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1 & 0x20) {
            var_v0 = 2;
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 0) {
                goto block_13;
            }
            if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == 1) {
                SetDialogData(0x1E);
                cnWrap_SoundRequest(0);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                return 0;
            }
            memcpy(&g_lb_653A50, &g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, 7);
            var_v0 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1;
block_13:
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = var_v0;
            cnWrap_SoundRequest(0);
            goto block_51;
        }
        if (temp_a1 & 0x40) {
            return 3;
        }
        if (temp_a1 & 0xC00) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) = Eft25_set((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)), 3);
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
            memset(&g_lb_6B46D0, 0, 0x110);
        } else {
            var_a1 = 2;
            switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370))) { /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                if (seekStr != 0) {

                } else {
                    var_a1 = 1;
                }
                break;
            case 2:                                 /* switch 2 */
                var_a1 = 6;
                break;
            default:                                /* switch 2 */
                var_a1 = 5;
                break;
            }
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = Lb_cursorUD((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)), var_a1);
        }
block_51:
    default:                                        /* switch 1 */
        return 2;
    case 1:                                         /* switch 1 */
        if (plaza_req_input(&g_lb_653A50, 1) == 1) {
            strcpy(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44) + 4, &g_lb_653A50);
            if (Visual_main(&g_lb_653A50) != 0) {
                *(&g_lb_6B46D1 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = Visual_main(&g_lb_653A50);
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
                    *(&g_lb_6B46D0 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = 2;
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
                }
            } else {
                *(&g_lb_6B46D1 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 0x44)) = 0;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            }
        }
        goto block_51;
    case 2:                                         /* switch 1 */
        temp_a1_2 = temp_s1 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1_2 & 0x20) {
            SetDialogData(0x1E);
            cnWrap_SoundRequest(0);
            switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370))) { /* switch 3; irregular */
            case 0:                                 /* switch 3 */
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                break;
            case 1:                                 /* switch 3 */
                var_v0_2 = 3;
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                seekStr = (s8) (*(s32 *)((u8 *)&g_main_2EEE68 + (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373))));
                g_lb_6B46D1 = 1;
block_44:
                g_lb_6B46D0 = var_v0_2;
                break;
            case 2:                                 /* switch 3 */
                var_v0_2 = 6;
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
                seekStr = (s8) (*(s32 *)((u8 *)&g_lb_5FBC28 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 2)));
                (*(u8 *)((u8 *)&seekStr + 0x1)) = (*(s32 *)((u8 *)&g_lb_5FBC29 + ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) * 2)));
                g_lb_6B46D1 = 1;
                goto block_44;
            }
            return 0;
        }
        if (temp_a1_2 & 0x40) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            cnWrap_SoundRequest(3);
        }
        goto block_51;
    case 3:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s1 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
        }
        goto block_51;
    }
}

