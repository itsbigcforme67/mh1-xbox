/* G online (PC): lm_member_list_mv, written from Monster Hunter G's code at lobby 0x58BF90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_PlStatusSet();
s32 Plaza_add_friend();
s32 cnWrap_SoundRequest();
s32 lobby_func_00571450();
s32 memcmp();
extern u8 PitMenu;
extern u8 cw;
extern u8 game_w;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Lb_get_pl_stat2();                    /* extern */
s32 cnLBS_Get_ConditionSearchUser();   /* extern */
extern s32 g_main_2EFBC4;
extern s32 lbCommer;
extern u8 player_work;

s32 lm_member_list_mv(s32 arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    s32 temp_a1_4;
    s32 temp_a3;
    s32 temp_s0;
    s32 temp_v0;
    s32 var_s2;
    s32 var_a1;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a1_3;
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a1_5;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *temp_v1_5;

    var_s2 = arg0;
    temp_a1 = (*(void **)&pNet);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0x22));
    switch (temp_a0) {                              /* irregular */
    case 0:
        temp_a3 = var_s2 & 0xFFFF;
        if (temp_a3 & 0x20) {
            temp_a0_2 = (*(u8 *)((u8 *)(temp_a1) + 0x29));
            if (*(&player_work + (temp_a0_2 * 0xA00)) != 0) {
                Lb_PlStatusSet((s32) temp_a0_2, (s32) temp_a1, arg2, temp_a3);
                temp_v1 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x22)) + 1);
                cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&PitMenu + 0x10)) = 0;
            } else {
                cnWrap_SoundRequest(7, (s32) temp_a1, arg2, temp_a3);
            }
            goto block_46;
        }
        (*(u8 *)((u8 *)&PitMenu + 0x10)) = 1;
        if (temp_a3 & 0x40) {
            (*(s16 *)((u8 *)&PitMenu + 0x12)) = 0xA;
            return var_s2;
        }
        temp_a0_3 = (*(u8 *)((u8 *)(temp_a1) + 0x29));
        var_a1 = ((s16) ((s32) temp_a0_3));
        if (temp_a3 & 0x2000) {
            var_a1 = ((s16) ((var_a1 - 1)));
            if (var_a1 < 0) {
                var_a1 = 9;
            }
            if ((((s16) (var_a1))) == (*(u8 *)((u8 *)&game_w + 0x3A))) {
                var_a1 = ((s16) ((var_a1 - 1)));
            }
            if ((((s16) (var_a1))) < 0) {
                var_a1 = 9;
            }
        } else if (temp_a3 & 0x1000) {
            var_a1 = ((s16) ((var_a1 + 1)));
            if (var_a1 >= 0xA) {
                var_a1 = 0;
            }
            if ((((s16) (var_a1))) == (*(u8 *)((u8 *)&game_w + 0x3A))) {
                var_a1 = ((s16) ((var_a1 + 1)));
            }
            if ((((s16) (var_a1))) >= 0xA) {
                var_a1 = 0;
            }
        }
        if ((((s16) (var_a1))) != (temp_a0_3 & 0xFF)) {
            (*(u8 *)((u8 *)(temp_a1) + 0x29)) = (u8) var_a1;
            cnWrap_SoundRequest(1, (s32) var_a1, (s32) (temp_a1 + 0x29), temp_a3);
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0U;
        }
        (*(u8 *)((u8 *)&PitMenu + 0x12)) = 0x10;
block_46:
    default:
        return ((*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) == 0) ? 0 : var_s2;
    case 1:
        if (Lb_get_pl_stat2((s8) (*(u8 *)((u8 *)(temp_a1) + 0x29)), temp_a1) == 2) {
            temp_v1_2 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_2) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x22)) - 1);
            cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)&PitMenu + 0x10)) = 1;
            (*(u8 *)((u8 *)&PitMenu + 0x12)) = 0x10;
        } else {
            temp_s0 = var_s2 & 0xFFFF;
            if (temp_s0 & 0x40) {
                temp_v1_3 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_3) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x22)) - 1);
                cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(u8 *)((u8 *)&PitMenu + 0x10)) = 1;
                (*(u8 *)((u8 *)&PitMenu + 0x12)) = 0x10;
                var_s2 = temp_s0 & ~0x40 & 0xFFFF;
            } else {
                temp_v1_4 = (*(void **)&pNet);
                temp_a1_2 = temp_v1_4 + 0x23;
                if (((*(u8 *)((u8 *)(temp_v1_4) + 0x23)) == 0) && (temp_s0 & 0x200)) {
                    (*(u8 *)((u8 *)(temp_v1_4) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x22)) + 1);
                    lobby_func_00571450(M2C_ERROR(/* Read from unset register $a0 */), (s32) temp_a1_2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
                    cnLBS_Get_ConditionSearchUser(&g_main_2EFBC4);
                    temp_a1_3 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29));
                    (*(void **)((u8 *)(&(*(s32 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) = (void *) ((*(void **)&cw) + (temp_a1_3 * 0x5C) + 0x38C);
                    cnWrap_SoundRequest(6, (s32) temp_a1_3, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                } else if (temp_s0 & 0xC00) {
                    (*(u8 *)((u8 *)(temp_v1_4) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x23)) ^ 1);
                    cnWrap_SoundRequest(9, (s32) temp_a1_2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                }
            }
        }
        goto block_46;
    case 2:
        temp_v0 = Plaza_add_friend((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        if ((temp_v0 != 1) && (temp_v0 != 0)) {

        } else {
            temp_a1_4 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) * 0x5C;
            if (memcmp((u8 *)&lbCommer + temp_a1_4, (*(void **)&cw) + temp_a1_4 + 0x38C, 8) != 0) {
                temp_a1_5 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_a1_5) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_a1_5) + 0x22)) - 2);
                (*(u8 *)((u8 *)&PitMenu + 0x10)) = 1;
                (*(u8 *)((u8 *)&PitMenu + 0x12)) = 0x10;
            } else {
                temp_v1_5 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_5) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_v1_5) + 0x22)) - 1);
            }
        }
        goto block_46;
    }
}

