/* G online (PC): plaza_checkFriend, written from Monster Hunter G's code at lobby 0x571B90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_cursorUD();
s32 Plaza_add_friend();
s32 cnWrap_SoundRequest();
s32 lobby_func_00570FF0();
s32 lobby_func_00571040();
s32 lobby_func_00571450();
s32 lobby_func_00571A90();
s32 lobby_func_00572110();
void * memcpy();
void * memset();
void * strcpy();
extern u8 g_main_2EFBAC;
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 g_main_2EFBAC;
extern u8 pNet;
extern u8 tl_member_buff;
s32 SoftKeyboard_alive_check();                     /* extern */
extern s8 g_lb_653A50;
extern u8 RecvMailInfo;

s32 plaza_checkFriend(s16 arg0) {
    s16 var_s1;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 var_a0;
    s8 temp_a1_2;
    s8 temp_v0;
    u8 temp_a0;
    u8 temp_a1_3;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a0_4;
    u8 *temp_a0_5;
    u8 *temp_a0_6;
    u8 *temp_a0_7;
    u8 *temp_a0_8;
    u8 *temp_a3;
    u8 *temp_v0_2;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *var_a1;
    u8 *var_a2;

    var_s1 = arg0;
    if (SoftKeyboard_alive_check() != 0) {
        var_s1 = 0;
    }
    lobby_func_00571A90(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    temp_a0 = (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230));
    switch (temp_a0) {                              /* irregular */
    case 0:
        temp_a1 = var_s1 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = var_s1;
        if (temp_a1 & 0x20) {
            temp_a3 = (*(void **)&g_main_2EFBAC);
            if ((*(u8 *)((u8 *)(temp_a3) + 0x235)) != 0) {
                var_a0 = 0x4D;
                var_a1 = (*(void **)&cw) + 0xEE6;
                var_a2 = &RecvMailInfo + ((*(s8 *)((u8 *)(temp_a3) + 0x236)) * 0x9A);
                do {
                    var_a0 -= 1;
                    temp_v0 = (*(s8 *)((u8 *)(var_a2) + 1));
                    (*(s8 *)((u8 *)(var_a1) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2) + 0));
                    var_a2 += 2;
                    (*(s8 *)((u8 *)(var_a1) + 1)) = temp_v0;
                    var_a1 += 2;
                } while (var_a0 > 0);
                temp_a1_2 = (*(s8 *)((u8 *)(temp_a3) + 0x236));
                *(&RecvMailInfo + (temp_a1_2 * 0x9A)) = 0;
                (*(u8 *)((u8 *)(temp_a3) + 0x230)) = (u8) ((*(u8 *)((u8 *)(temp_a3) + 0x230)) + 1);
                cnWrap_SoundRequest(0, (s32) temp_a1_2, (s32) var_a2, (s32) temp_a3);
            } else {
                cnWrap_SoundRequest(7, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), (s32) temp_a3);
            }
            goto block_54;
        }
        if (temp_a1 & 0x40) {
            return 3;
        }
        if (temp_a1 & 0x80) {
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 2U;
            (*(s8 *)&D_0035EFEB) = 0;
            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)) = 0;
            g_lb_653A50 = 0;
            memset((*(void **)&g_main_2EFBAC) + 0x9A, 0, 0x114);
            memset((*(void **)&g_main_2EFBAC) + 0x23C, 1, 3);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) = 0;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        } else if (temp_a1 & 0x200) {
            temp_a0_2 = (*(void **)&g_main_2EFBAC);
            if ((*(u8 *)((u8 *)(temp_a0_2) + 0x235)) != 0) {
                (*(u8 *)((u8 *)(temp_a0_2) + 0x23A)) = (u8) (*(u8 *)((u8 *)(temp_a0_2) + 0x230));
                (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 3U;
                lobby_func_00571450((s32) temp_a0_2, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), 2);
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
                temp_a0_3 = (*(void **)&g_main_2EFBAC);
                memcpy(temp_a0_3 + 0x9A, &RecvMailInfo + ((*(s8 *)((u8 *)(temp_a0_3) + 0x236)) * 0x9A) + 1, 8);
                temp_a0_4 = (*(void **)&g_main_2EFBAC);
                memcpy(temp_a0_4 + 0xA2, &RecvMailInfo + ((*(s8 *)((u8 *)(temp_a0_4) + 0x236)) * 0x9A) + 9, 0x10);
                (*(void **)((u8 *)(&(*(s32 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) = (void *) ((*(void **)&g_main_2EFBAC) + 0x9A);
                cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                cnWrap_SoundRequest(7, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), 2);
            }
        } else {
            temp_v0_2 = (*(void **)&g_main_2EFBAC);
            temp_a1_3 = (*(u8 *)((u8 *)(temp_v0_2) + 0x235));
            if ((s32) temp_a1_3 >= 2) {
                (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x236)) = Lb_cursorUD((s32) (*(s8 *)((u8 *)(temp_v0_2) + 0x236)), (s32) temp_a1_3, M2C_ERROR(/* Read from unset register $a2 */), 2);
            }
        }
block_54:
        return 2;
    case 1:
        temp_a0_5 = (*(void **)&pNet);
        temp_a2 = var_s1 & 0xFFFF;
        (*(s16 *)((u8 *)(temp_a0_5) + 0x20)) = var_s1;
        if (temp_a2 & 0x40) {
            temp_v1 = (*(void **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_v1) + 0x230)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x230)) - 1);
            cnWrap_SoundRequest(3, 4, temp_a2, 2);
        } else if (temp_a2 & 0x80) {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF00)) = 0;
            strcpy((*(void **)&g_main_2EFBAC) + 0x9A, (*(void **)&cw) + 0xEE7);
            strcpy((*(void **)&g_main_2EFBAC) + 0xA2, (*(void **)&cw) + 0xEEF);
            memset((*(void **)&g_main_2EFBAC) + 0xF6, 0, 0x5C);
            memset((*(void **)&g_main_2EFBAC) + 0x152, 0, 0x5C);
            memset((*(void **)&g_main_2EFBAC) + 0x23C, 1, 3);
            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x23C)) = 0;
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 2U;
            (*(u8 *)&D_0035EFEB) = 0;
            (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)) = 0;
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        } else if (temp_a2 & 0x200) {
            temp_a0_6 = (*(void **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_a0_6) + 0x23A)) = (u8) (*(u8 *)((u8 *)(temp_a0_6) + 0x230));
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 3U;
            lobby_func_00571450((s32) temp_a0_6, 4, temp_a2, 2);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
            strcpy((*(void **)&g_main_2EFBAC) + 0x9A, (*(void **)&cw) + 0xEE7);
            strcpy((*(void **)&g_main_2EFBAC) + 0xA2, (*(void **)&cw) + 0xEEF);
            (*(void **)((u8 *)(&(*(s32 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) = (void *) ((*(void **)&g_main_2EFBAC) + 0x9A);
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        } else if (temp_a2 & 1) {
            temp_v1_2 = (*(void **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_v1_2) + 0x23A)) = (u8) (*(u8 *)((u8 *)(temp_v1_2) + 0x230));
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 4U;
            lobby_func_00570FF0((s32) temp_a0_5, 4, temp_a2, 2);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
            temp_a0_7 = (*(void **)&g_main_2EFBAC);
            memcpy(temp_a0_7 + 0x9A, &RecvMailInfo + ((*(s8 *)((u8 *)(temp_a0_7) + 0x236)) * 0x9A) + 1, 8);
            temp_a0_8 = (*(void **)&g_main_2EFBAC);
            memcpy(temp_a0_8 + 0xA2, &RecvMailInfo + ((*(s8 *)((u8 *)(temp_a0_8) + 0x236)) * 0x9A) + 9, 0x10);
            (*(void **)((u8 *)(&(*(s32 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) = (void *) ((*(void **)&g_main_2EFBAC) + 0x9A);
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        goto block_54;
    case 2:
        if (lobby_func_00572110((s32) temp_a0, 4, M2C_ERROR(/* Read from unset register $a2 */), 2) != 0) {

        } else {
            (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 0U;
            (*(u8 *)&D_0035EFEB) = 1;
        }
        goto block_54;
    case 3:
        temp_v0_3 = Plaza_add_friend((s32) temp_a0, 4, M2C_ERROR(/* Read from unset register $a2 */), 2);
        if ((temp_v0_3 != 1) && (temp_v0_3 != 0)) {

        } else {
            temp_v1_3 = (*(void **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_v1_3) + 0x230)) = (u8) (*(u8 *)((u8 *)(temp_v1_3) + 0x23A));
        }
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        goto block_54;
    case 4:
        temp_v0_4 = lobby_func_00571040((s32) temp_a0, 4, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        if ((temp_v0_4 != 1) && (temp_v0_4 != 0)) {

        } else {
            temp_v1_4 = (*(void **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_v1_4) + 0x230)) = (u8) (*(u8 *)((u8 *)(temp_v1_4) + 0x23A));
        }
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        goto block_54;
    default:
        return 3;
    }
}

