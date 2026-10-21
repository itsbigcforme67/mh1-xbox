/* G online (PC): func_0010D8E0, written from Monster Hunter G's code at lobby 0x10D8E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 func_0010DD20();
extern u8 game_w;
extern u8 push00;
extern u8 User_data;
extern u8 player_work;

void func_0010D8E0(s32 arg0) {
    u8 *var_a4;   /* (s32 *) */
    s32 var_a0;
    s32 var_a3;
    s8 temp_v1;

    var_a0 = arg0;
    if (!(var_a0 & 0xFF)) {
        var_a0 = 1 & 0xFF;
        if (func_0010DD20(&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00), NULL) != 1) {

        } else {
            var_a0 = 2;
        }
    }
    var_a4 = &(*(u8 *)((u8 *)&push00 + 0x20));
    var_a3 = 0x6C;
    do {
        temp_v1 = (*(s8 *)((u8 *)(var_a4) + 6));
        if ((*(u8 *)((u8 *)((&User_data + (temp_v1 >> 3))) + 0xC80)) & (1 << (temp_v1 & 7))) {
            if ((var_a0 & 0xFF) == 1) {
                if (!((*(u8 *)((u8 *)(var_a4) + 7)) & 1)) {

                }
            }
        }
        var_a3 -= 1;
        var_a4 += 8;
    } while (var_a3 != 0);
}

