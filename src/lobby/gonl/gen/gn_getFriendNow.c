/* G online (PC): getFriendNow, written from Monster Hunter G's code at lobby 0x56BFA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00580910();
extern u8 g_main_2EFAD8;
extern u8 cw;
extern u8 g_main_2EFAD8;
extern u8 lb_sys;
extern u8 pNet;
extern u8 tl_member_buff;
s32 cnLBS_Get_SerchUserPlace(); /* extern */
extern s16 g_lb_6B42B4;
extern s8 g_lb_6B42BD;
extern s32 g_main_302CB0;

s32 getFriendNow(u8 arg0, s32 arg1) {
    s32 temp_s0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1_3;
    u8 temp_a1;
    u8 temp_a1_2;
    u8 temp_v1_2;
    u8 temp_v1_4;
    u8 temp_v1_5;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_v0;
    u8 *temp_v1;

    temp_v0 = (u8 *)&g_main_302CB0 + (g_lb_6B42B4 * 0x150);
    switch (g_lb_6B42BD) {                           /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&lb_sys + 0xA0)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 2))) = (*(u16 *)&g_main_2EFAD8);
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) + 1);
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = arg0;
        /* fallthrough */
    case 1:                                         /* switch 1 */
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) + 1);
        temp_v1 = (*(void **)&pNet);
        temp_a1 = (*(u8 *)((u8 *)(temp_v1) + 0x27));
        if ((temp_a1 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 7)) >= 0x32) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
            return 0;
        }
        temp_v1_2 = temp_a1 + 1;
        if (*(temp_v0 + (temp_a1 * 0x30)) == 0) {
            (*(u8 *)((u8 *)(temp_v1) + 0x27)) = temp_v1_2;
            if ((temp_v1_2 & 0xFF) >= (((s8) (arg1)))) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
                return 0;
            }
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) - 1);
            goto block_27;
        }
block_27:
        return 2;
    case 2:                                         /* switch 1 */
        temp_a1_2 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27));
        temp_s0 = temp_a1_2 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 7);
        temp_v0_2 = lobby_func_00580910(temp_v0 + (temp_a1_2 * 0x30));
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1_3 = temp_s0 * 0x24A;
            (*(s8 *)((u8 *)((temp_v1_3 + (*(s32 *)&cw))) + 0x1062)) = 1;
            temp_v0_3 = (*(s32 *)&cw) + temp_v1_3;
            cnLBS_Get_SerchUserPlace(temp_v0_3 + 0x105C, temp_v0_3 + 0x105E, temp_v0_3 + 0x1060, temp_v0_3 + 0x1265);
            temp_a0 = (*(void **)&pNet);
            temp_v1_4 = (*(u8 *)((u8 *)(temp_a0) + 0x27)) + 1;
            (*(u8 *)((u8 *)(temp_a0) + 0x27)) = temp_v1_4;
            if ((temp_v1_4 & 0xFF) >= (((s8) (arg1)))) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
                return 0;
            }
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) - 1);
            goto block_27;
        case 1:                                     /* switch 2 */
        case 3:                                     /* switch 2 */
            (*(s8 *)((u8 *)(((temp_s0 * 0x24A) + (*(s32 *)&cw))) + 0x1265)) = 2;
            temp_a0_2 = (*(void **)&pNet);
            temp_v1_5 = (*(u8 *)((u8 *)(temp_a0_2) + 0x27)) + 1;
            (*(u8 *)((u8 *)(temp_a0_2) + 0x27)) = temp_v1_5;
            if ((temp_v1_5 & 0xFF) >= (((s8) (arg1)))) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
                return 0;
            }
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) - 1);
            goto block_27;
        }
        break;
    default:                                        /* switch 1 */
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60D)) = 0;
        return 0;
    }
}

