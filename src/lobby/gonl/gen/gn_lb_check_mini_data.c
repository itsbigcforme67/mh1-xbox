/* G online (PC): lb_check_mini_data, written from Monster Hunter G's code at lobby 0x559160 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_set_mini_data_to_pl();
s32 Lb_set_player();
s32 memcmp();
void * memcpy();
extern u8 cw;
extern u8 game_w;
s32 Lb_player_release();                  /* extern */
extern s32 my_user_mini_data;
extern s32 player_work;

s32 lb_check_mini_data(s32 arg0, void *arg1, void *arg2) {
    s32 temp_s1;
    s32 var_s6;
    s32 temp_s0;
    u8 *temp_s2;

    var_s6 = 0;
    temp_s0 = ((s8) (arg0));
    temp_s1 = temp_s0 * 0x5C;
    temp_s2 = (*(s32 *)&cw) + temp_s1 + 0x3A8;
    if ((memcmp(temp_s2 + 3, arg2 + 3, 6) != 0) || (memcmp(temp_s2 + 0xA, arg2 + 0xA, 6) != 0)) {
        if ((*(s8 *)((u8 *)((temp_s0 + (*(s32 *)&cw))) + 0xB64)) == 0) {
            Lb_set_mini_data_to_pl((s32) arg0, arg2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Lb_set_player(arg0 & 0xFF, arg1, (*(s32 *)&cw) + temp_s1 + 0x394, M2C_ERROR(/* Read from unset register $a3 */));
        } else {
            Lb_player_release((u8 *)&player_work + (temp_s0 * 0xA00));
            Lb_set_mini_data_to_pl((s32) arg0, arg2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Lb_set_player(arg0 & 0xFF, arg1, (*(s32 *)&cw) + temp_s1 + 0x394, M2C_ERROR(/* Read from unset register $a3 */));
            var_s6 = 1;
        }
        (*(s8 *)((u8 *)((temp_s0 + (*(s32 *)&cw))) + 0xB64)) = 0;
    }
    memcpy(temp_s2, arg2, 0x40);
    if (temp_s0 == (*(u8 *)((u8 *)&game_w + 0x3A))) {
        memcpy(&my_user_mini_data, arg2, 0x1C);
    }
    return var_s6;
}

