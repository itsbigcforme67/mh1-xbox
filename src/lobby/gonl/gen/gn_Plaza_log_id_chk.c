/* G online (PC): Plaza_log_id_chk, written from Monster Hunter G's code at lobby 0x58F460 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 plaza_log_id_chk_sub();
extern u8 PitMenu;
extern u8 lb_rule_member;
s32 memcmp();             /* extern */
s32 strcmp();                      /* extern */
extern s32 g_lb_63E918;
extern u8 my_user_id;

u32 Plaza_log_id_chk(s32 **arg0) {
    u8 *var_s0;   /* (s32 **) */
    u8 *var_s3;   /* (s32 *) */
    s32 var_s1;
    u8 var_s2;

    var_s2 = (*(u8 *)((u8 *)&PitMenu + 0x26));
    if (var_s2 == 0) {
        *arg0 = NULL;
        return 0U;
    }
    var_s3 = &(*(u8 *)((u8 *)&PitMenu + 0x2A));
    var_s1 = 0;
    if (var_s2 != 0) {
        var_s0 = arg0;
        do {
            if ((strcmp(var_s3 + 0x46, &my_user_id) != 0) && (strcmp(var_s3 + 0x46, (*(s32 * *)((u8 *)&lb_rule_member + 0x14))) != 0) && (memcmp(var_s3 + 0x46, &g_lb_63E918, 5) != 0) && (plaza_log_id_chk_sub((s32) (var_s3 + 0x46), arg0, var_s1) == 1)) {
                (*(s32 **)var_s0) = var_s3;
                var_s1 += 1;
                var_s0 += 4;
            }
            var_s2 -= 1;
            var_s3 += 0x5F;
        } while (var_s2 != 0);
    }
    if ((u32) var_s1 < 0x40U) {
        *(arg0 + (var_s1 * 4)) = 0;
    }
    return (u32) var_s1;
}

