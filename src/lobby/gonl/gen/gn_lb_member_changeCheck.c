/* G online (PC): lb_member_changeCheck, written from Monster Hunter G's code at lobby 0x566730 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Skill_set_PL();
s32 memcmp();
extern u8 cw;
s32 lb_check_mini_data();     /* extern */
s32 lobby_func_0059B0D0();                   /* extern */
extern s32 lbCommer;
extern u8 player_work;

s32 lb_member_changeCheck(void) {
    u8 *var_s0;   /* (s32 *) */
    u8 *var_s2;   /* (s32 *) */
    s32 var_s1;
    s32 var_s4;
    s32 var_s3;

    var_s4 = 0;
    var_s3 = 0;
    var_s2 = &player_work;
    var_s1 = 0;
    var_s0 = &lbCommer;
    do {
        if (((*(u8 *)((u8 *)(var_s2) + 0)) != 0) && (memcmp(var_s0 + 0x1C, (*(s32 *)&cw) + var_s1 + 0x3A8, 0x40) != 0) && (lb_check_mini_data(var_s3, var_s0, var_s0 + 0x1C) == 1)) {
            Skill_set_PL(var_s2);
            lobby_func_0059B0D0(var_s3 & 0xFF);
            var_s4 = 1;
            (*(s8 *)((u8 *)(var_s2) + 0x3E8)) = 1;
        }
        var_s2 += 0xA00;
        var_s3 = ((s8) ((var_s3 + 1)));
        var_s1 += 0x5C;
        var_s0 += 0x5C;
    } while (var_s3 < 0xA);
    return var_s4;
}

