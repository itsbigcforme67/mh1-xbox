/* lb_dd11 - browser: check_rowspan 0x00606160-0x00606274. Hand-written from the asm (working copy in lb_dr2.c). */
#include "lobby_f.h"
extern u8 *bsw;

u8 *check_upTD_rowspan();
int check_rowspan_sub();
int SetTableData();
void set_TH_TD_data_1st();
int check_rowspan(void) {
    u8 *var_v0;
    u8 *temp_a0;
    u8 *temp_v0;
    u8 *var_s1;
    s32 var_s0;

    temp_a0 = bsw;
    temp_v0 = (u8 *)(temp_a0 + (*(u16 *)(temp_a0 + 0xD894) * 0x5C));
    var_s0 = *(s32 *)(temp_v0 + 0x24E0);
    var_s1 = temp_v0 + 0x24E0;
    if (var_s0 == 0) {
        return -1;
    }
    var_v0 = check_upTD_rowspan(var_s1);
    if (var_v0 == 0) {
        return 0;
    }
    do {
        if (check_rowspan_sub(var_s1, var_s0, var_v0) < 0) {
            return -1;
        }
        if (SetTableData(4) < 0) {
            return -1;
        }
        temp_a0 = bsw;
        temp_v0 = (u8 *)(temp_a0 + (*(u16 *)(temp_a0 + 0xD894) * 0x5C));
        var_s0 = *(s32 *)(temp_v0 + 0x24E0);
        var_s1 = temp_v0 + 0x24E0;
        if (var_s0 == 0) {
            return -1;
        }
        set_TH_TD_data_1st(var_s1, var_s0);
        var_v0 = check_upTD_rowspan(var_s1);
    } while (var_v0 != 0);
    return 0;
}
