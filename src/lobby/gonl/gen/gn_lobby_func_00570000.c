/* G online (PC): lobby_func_00570000, written from Monster Hunter G's code at lobby 0x570000 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Eft25_set();
s32 Get_sw();
s32 Get_sw_on2();
s32 Lb_cursorUD();
s32 SetDialogData();
s32 cnWrap_SoundRequest();
extern u8 pNet;
extern u8 tl_member_buff;
extern s8 g_lb_6B46A0;
extern s8 g_lb_6B46A1;
extern s16 g_lb_6B46A2;
extern s8 g_lb_6B46A4;
extern s8 g_lb_6B46A5;
extern s8 g_lb_6B46A6;
extern s8 g_lb_6B46A8;
extern s8 g_lb_6B46A9;
extern s8 g_lb_6B46AA;

s32 lobby_func_00570000(void) {
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_s1;
    s32 var_a1;

    temp_s1 = Get_sw(0) & 0xFFFF;
    switch ((*(u8 *)((u8 *)&tl_member_buff + 0x1383))) {                           /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_a1 = temp_s1 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1 & 0x20) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1);
            cnWrap_SoundRequest(0, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            goto block_36;
        }
        if (temp_a1 & 0x40) {
            return 3;
        }
        if (temp_a1 & 0xC00) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) = Eft25_set((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)), 5, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
        } else {
            if (((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 2) && ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 1)) {
                var_a1 = 7;
            } else {
                var_a1 = 4;
            }
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = Lb_cursorUD((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)), var_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
block_36:
        return 2;
    case 1:                                         /* switch 1 */
        temp_a1_2 = temp_s1 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_a1_2 & 0x20) {
            SetDialogData(0x1E, temp_a1_2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            g_lb_6B46A2 = 0;
            g_lb_6B46A0 = 0x50;
            if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == 0) {
                g_lb_6B46A1 = 1;
            } else {
                g_lb_6B46A1 = 2;
            }
            g_lb_6B46A4 = 2;
            g_lb_6B46A5 = 4;
            g_lb_6B46A8 = 3;
            g_lb_6B46A6 = 1 << (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370));
            switch ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373))) { /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                g_lb_6B46A9 = 2;
                g_lb_6B46AA = 0;
                break;
            case 1:                                 /* switch 2 */
                g_lb_6B46A9 = 3;
                g_lb_6B46AA = 3;
                break;
            default:                                /* switch 2 */
                g_lb_6B46A9 = 1;
                g_lb_6B46AA = (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) + 2;
                break;
            }
            return 0;
        }
        if (temp_a1_2 & 0x40) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            cnWrap_SoundRequest(3, temp_a1_2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_36;
    default:                                        /* switch 1 */
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
        return 3;
    }
}

