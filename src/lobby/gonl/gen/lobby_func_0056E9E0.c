/* G online (PC): lobby_func_0056E9E0, written from Monster Hunter G's code at lobby 0x56E9E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBB0;
extern u8 g_main_2EFBB0;
extern u8 pNet;
extern u8 tl_member_buff;
s32 cnLBS_Get_SerchUserPlace(); /* extern */
s32 lobby_func_00580910();             /* extern */

s32 lobby_func_0056E9E0(void) {
    s32 temp_a1;
    s32 temp_v0;
    s32 temp_v0_2;
    u8 temp_a2;
    u8 temp_v1;
    u8 temp_v1_3;
    u8 temp_v1_5;
    u8 temp_v1_7;
    u8 *temp_a0;
    u8 *temp_v0_3;
    u8 *temp_v1_2;
    u8 *temp_v1_4;
    u8 *temp_v1_6;

    temp_a1 = (*(u8 *)((u8 *)&tl_member_buff + 0x137C)) * 0x118;
    temp_v0 = (*(s32 *)&g_main_2EFBB0) + temp_a1 + 4;
    switch ((*(u8 *)((u8 *)&tl_member_buff + 0x1383))) {                           /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1);
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 0U;
        /* fallthrough */
    case 1:                                         /* switch 1 */
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) + 1);
        temp_a0 = (*(void **)&pNet);
        temp_a2 = (*(u8 *)((u8 *)(temp_a0) + 0x27));
        if ((temp_a2 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 7)) >= (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D))) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            return 0;
        }
        if (*(temp_v0 + (temp_a2 * 0x28)) == 0) {
            (*(u8 *)((u8 *)(temp_a0) + 0x27)) = (u8) (temp_a2 + 1);
            temp_v1 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27));
            if ((s32) temp_v1 < (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D))) {
                if ((s32) temp_v1 >= 7) {
                    goto block_12;
                }
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) - 1);
                goto block_39;
            }
block_12:
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            return 0;
        }
block_39:
        return 2;
    case 2:                                         /* switch 1 */
        temp_v0_2 = lobby_func_00580910(temp_v0 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) * 0x28), temp_a1, (*(u8 *)((u8 *)&tl_member_buff + 0x137C)) * 7);
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 3:                                     /* switch 2 */
            (*(s8 *)((u8 *)((((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) * 0x24A) + &(*(u8 *)((u8 *)&tl_member_buff + 0x18)))) + 0x15A)) = 1;
            (*(s8 *)((u8 *)((((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) * 0x24A) + &(*(u8 *)((u8 *)&tl_member_buff + 0x18)))) + 0x35D)) = 0;
            temp_v1_2 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_2) + 0x27)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x27)) + 1);
            temp_v1_3 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27));
            if ((s32) temp_v1_3 < (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D))) {
                if ((s32) temp_v1_3 >= 7) {
                    goto block_22;
                }
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) - 1);
                goto block_39;
            }
block_22:
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            return 0;
        case 0:                                     /* switch 2 */
            (*(s8 *)((u8 *)((((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) * 0x24A) + &(*(u8 *)((u8 *)&tl_member_buff + 0x18)))) + 0x15A)) = 1;
            temp_v0_3 = &(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) * 0x24A);
            cnLBS_Get_SerchUserPlace(temp_v0_3 + 0x154, temp_v0_3 + 0x156, temp_v0_3 + 0x158, temp_v0_3 + 0x35D);
            temp_v1_4 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_4) + 0x27)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x27)) + 1);
            temp_v1_5 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27));
            if ((s32) temp_v1_5 < (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D))) {
                if ((s32) temp_v1_5 >= 7) {
                    goto block_28;
                }
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) - 1);
                goto block_39;
            }
block_28:
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            return 0;
        case 1:                                     /* switch 2 */
            (*(s8 *)((u8 *)((((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) * 0x24A) + &(*(u8 *)((u8 *)&tl_member_buff + 0x18)))) + 0x35D)) = 2;
            temp_v1_6 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_6) + 0x27)) = (u8) ((*(u8 *)((u8 *)(temp_v1_6) + 0x27)) + 1);
            temp_v1_7 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27));
            if ((s32) temp_v1_7 < (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D))) {
                if ((s32) temp_v1_7 >= 7) {
                    goto block_34;
                }
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) - 1);
                goto block_39;
            }
block_34:
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
            return 0;
        }
        break;
    default:                                        /* switch 1 */
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0U;
        return 0;
    }
}

