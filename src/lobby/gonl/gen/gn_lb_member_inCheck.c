/* G online (PC): lb_member_inCheck, written from Monster Hunter G's code at lobby 0x566810 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_set_mini_data_to_pl();
s32 Lb_set_player();
s32 lobby_func_00595530();
void * memcpy();
void * strcpy();
extern u8 cw;
extern u8 game_w;
s32 Lb_send_myChair();                        /* extern */
s32 Online_ck();                                  /* extern */
extern s8 lbCommer;
extern u8 player_work;

void lb_member_inCheck(void) {
    u8 *var_s0;   /* (s32 *) */
    s32 var_s2;
    s32 var_s5;
    s8 *var_s1;
    s8 *var_s3;
    u8 temp_a0;
    u8 temp_a0_3;
    u8 *temp_a0_2;
    u8 *temp_s4;

    temp_a0 = (*(u8 *)((u8 *)&game_w + 0x3A));
    temp_s4 = &player_work + (temp_a0 * 0xA00);
    if (Online_ck(temp_a0) != 0) {
        var_s3 = &lbCommer;
        var_s5 = 0;
        var_s2 = 0;
        var_s1 = &lbCommer;
        var_s0 = &player_work;
        do {
            if (*var_s3 != 0) {
                temp_a0_2 = (*(s32 *)&cw) + var_s2;
                if ((*(s8 *)((u8 *)(temp_a0_2) + 0x38C)) == 0) {
                    strcpy(temp_a0_2 + 0x38C, var_s1);
                    strcpy((*(s32 *)&cw) + var_s2 + 0x394, var_s3 + 8);
                    memcpy((*(s32 *)&cw) + var_s2 + 0x3A8, var_s3 + 0x1C, 0x40);
                    Lb_set_mini_data_to_pl(var_s5, var_s3 + 0x1C, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    Lb_set_player(var_s5 & 0xFF, var_s3, var_s3 + 8, M2C_ERROR(/* Read from unset register $a3 */));
                    (*(s8 *)((u8 *)(((*(s32 *)&cw) + (((s8) ((s32) var_s5))))) + 0xB64)) = 0;
                    (*(s8 *)((u8 *)(var_s0) + 1)) = 1;
                    if (lobby_func_00595530((s32) (*(u8 *)((u8 *)(temp_s4) + 0x14)), (s32) (*(u8 *)((u8 *)(temp_s4) + 0x15)), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) == 1) {
                        temp_a0_3 = (*(u8 *)((u8 *)&game_w + 0x14));
                        if (temp_a0_3 == 0x4D) {
                            Lb_send_myChair(temp_a0_3);
                        }
                    }
                }
            }
            var_s3 += 0x5C;
            var_s5 = (s32) (((s8) ((var_s5 + 1))));
            var_s2 += 0x5C;
            var_s1 += 0x5C;
            var_s0 += 0xA00;
        } while (var_s5 < 0xA);
    }
}

