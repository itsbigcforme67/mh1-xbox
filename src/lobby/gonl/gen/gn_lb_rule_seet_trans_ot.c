/* G online (PC): lb_rule_seet_trans_ot, written from Monster Hunter G's code at lobby 0x55CB50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Draw_square();
s32 Put_2TF();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 reload_tex();
extern u8 mhRule;
s32 Online_ck();                                    /* extern */
s32 func_0012E0C0(); /* extern */
s32 func_0012E140(); /* extern */
extern s32 lb_rule_data_tbl;
extern s32 lb_sys;

void lb_rule_seet_trans_ot(void) {
    u8 *var_s1;   /* (s32 *) */
    s32 var_s0;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    if (lb_sys != 3) {
        if (Online_ck() == 0) {
            goto block_3;
        }
        func_0012E140(0x146, 0x32, 0x122, 0x132);
    } else {
block_3:
        func_0012E0C0(0x146, 0x32, 0x122, 0x132);
    }
    var_s1 = &lb_rule_data_tbl;
    var_s0 = 0;
    do {
        Put_2TF(var_s1);
        var_s0 += 1;
        var_s1 += 0x14;
    } while (var_s0 < 8);
    if (((*(s8 *)((u8 *)&mhRule + 0x4F)) == 5) || ((*(s8 *)((u8 *)&mhRule + 0x4F)) == 6)) {
        Draw_square(0x170, 0xD2, 0xCE, 0x44, -0x3FFF0100);
        return;
    }
    Draw_square(0x170, 0xD2, 0xCE, 0x44, 0x80206020);
}

