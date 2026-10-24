/* G online (PC): lb_member_outCheck, written from Monster Hunter G's code at lobby 0x566600 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 armor_model_free();
s32 memcmp();
void * memset();
s32 release_prim();
extern u8 cw;
s32 func_00114AC0();                   /* extern */
extern s32 lbCommer;
extern u8 lb_player;
extern u8 player_work;

s32 lb_member_outCheck(void) {
    u8 *var_s0;   /* (s32 *) */
    u8 *var_s1;   /* (s32 *) */
    u8 *var_s2;   /* (s32 *) */
    s32 var_s3;
    s32 var_s4;
    s32 var_s5;

    var_s5 = 0;
    var_s4 = 0;
    var_s3 = 0;
    var_s2 = &lbCommer;
    var_s1 = &player_work;
    var_s0 = &lb_player;
    do {
        if ((memcmp(var_s2, (*(s32 *)&cw) + var_s3 + 0x38C, 8) != 0) && ((*(s8 *)((u8 *)(((*(s32 *)&cw) + var_s3)) + 0x38C)) != 0)) {
            (*(s8 *)((u8 *)(var_s1) + 1)) = 0;
            var_s4 = 1;
            (*(s8 *)((u8 *)(var_s1) + 0)) = 0;
            func_00114AC0(var_s1);
            armor_model_free(var_s1);
            if ((*(s32 *)((u8 *)(var_s1) + 0x564)) != 0) {
                release_prim((s32) (*(s16 *)((u8 *)(var_s1) + 0x568)));
                (*(s32 *)((u8 *)(var_s1) + 0x564)) = 0;
            }
            memset(var_s1, 0, 0xA00);
            memset((*(s32 *)&cw) + var_s3 + 0x38C, 0, 0x5C);
            (*(s8 *)((u8 *)(var_s0) + 0x24)) = 0;
        }
        var_s3 += 0x5C;
        var_s5 = ((s8) ((var_s5 + 1)));
        var_s2 += 0x5C;
        var_s1 += 0xA00;
        var_s0 += 0x38;
    } while (var_s5 < 0xA);
    return var_s4;
}

