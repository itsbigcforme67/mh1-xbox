/* G online (PC): CallBack_Result_Plaza_LobbyMember, written from Monster Hunter G's code at lobby 0x57F2F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_set_mini_data_to_pl();
s32 Lb_set_player();
s32 To_LogOut();
s32 lobby_func_0058BAA0();
s32 memcmp();
void * memcpy();
void * memset();
extern u8 cw;
extern u8 game_w;
extern u8 lb_sys;
s32 cnLBS_Get_LobbyMemberList(); /* extern */
extern s8 lbCommer;
extern s32 my_user_id;

void CallBack_Result_Plaza_LobbyMember(s64 arg0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    u8 g_frame[0xE0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp78 (*(s64 *)(g_frame + 0x78))
#define sp80 (*(s8 *)(g_frame + 0x80))
#define sp88 (*(s32 *)(g_frame + 0x88))
#define sp9C (*(s32 *)(g_frame + 0x9C))
    s32 temp_s3;
    s32 var_s2;
    s8 *var_s3;
    s8 *var_v1;
    s8 var_s1;
    s8 var_s4;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_s4;
    u8 *temp_v0;
    u8 *temp_v1;
    u8 *temp_v1_2;

    temp_a1 = (*(void **)&cw);
    sp78 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0xE)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp78 == 0) {
            var_s2 = 0;
            (*(u8 *)((u8 *)&game_w + 0x3C)) = 0;
            do {
                memset(&sp80, 0, 0x5C);
                cnLBS_Get_LobbyMemberList(var_s2 & 0xFFFF, &sp80, &sp88, &sp9C);
                lobby_func_0058BAA0(&sp9C, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                if (sp80 != 0) {
                    var_s1 = 0;
                    var_v1 = &lbCommer;
loop_6:
                    if (*var_v1 != 0) {
                        var_s1 += 1;
                        var_v1 += 0x5C;
                        if (var_s1 >= 0xA) {

                        } else {
                            goto loop_6;
                        }
                    }
                    var_s4 = 0;
                    var_s3 = &lbCommer;
                    do {
                        if (memcmp(&sp80, var_s3, 8) == 0) {
                            var_s1 = var_s4;
                        }
                        var_s4 += 1;
                        var_s3 += 0x5C;
                    } while (var_s4 < 0xA);
                    temp_s3 = var_s1 * 0x5C;
                    memset((*(void **)&cw) + temp_s3 + 0x38C, 0, 0x5C);
                    temp_s4 = &lbCommer + temp_s3;
                    memset(temp_s4, 0, 0x5C);
                    memcpy((*(void **)&cw) + temp_s3 + 0x38C, &sp80, 8);
                    memcpy((*(void **)&cw) + temp_s3 + 0x394, &sp88, 0x10);
                    memcpy((*(void **)&cw) + temp_s3 + 0x3A8, &sp9C, 0x40);
                    memcpy(temp_s4, (*(void **)&cw) + temp_s3 + 0x38C, 8);
                    memcpy(temp_s4 + 8, (*(void **)&cw) + temp_s3 + 0x394, 0x10);
                    memcpy(temp_s4 + 0x1C, (*(void **)&cw) + temp_s3 + 0x3A8, 0x40);
                    if (memcmp(&sp80, &my_user_id, 8) == 0) {
                        (*(s8 *)((u8 *)&game_w + 0x3A)) = var_s1;
                    }
                    if ((*(u8 *)((u8 *)&game_w + 0x3C)) != 0) {
                        temp_v1 = (*(void **)&cw);
                        if (memcmp(temp_v1 + 3, temp_v1 + temp_s3 + 0x38C, 8) < 0) {
                            goto block_18;
                        }
                    } else {
block_18:
                        memset((*(void **)&cw) + 3, 0, 8);
                        temp_v1_2 = (*(void **)&cw);
                        memcpy(temp_v1_2 + 3, temp_v1_2 + temp_s3 + 0x38C, 8);
                    }
                    Lb_set_mini_data_to_pl((s32) (((s8) ((s32) var_s1))), (*(void **)&cw) + temp_s3 + 0x3A8, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    temp_v0 = (*(void **)&cw) + temp_s3;
                    (*(u8 *)((u8 *)&game_w + 0x3C)) = (u8) ((*(u8 *)((u8 *)&game_w + 0x3C)) + 1);
                    Lb_set_player(var_s1 & 0xFF, temp_v0 + 0x38C, temp_v0 + 0x394, M2C_ERROR(/* Read from unset register $a3 */));
                }
                var_s2 += 1;
            } while (var_s2 < 0xA);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA5D)) = (u8) (*(u8 *)((u8 *)&game_w + 0x3C));
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            (*(s8 *)((u8 *)&lb_sys + 0x1A)) = 0;
            return;
        }
        To_LogOut(4, (s32) temp_a1, (s32) (temp_a1 + 0xBA9), arg3);
    }
}

