#include "lobby_a.h"
extern u8 COM_R_No_1;
extern char conn_mcs_jmp_tbl_871[];
s32 internet_to_modem(void) {
    s32 var_s0;
    u8 temp_v1;

    temp_v1 = COM_R_No_1;
    if (temp_v1 == 6) {
        var_s0 = 1;
    } else {
        var_s0 = -1;
        if (temp_v1 == 7) {

        } else {
            var_s0 = 0;
        }
    }
    ((int (**)())&conn_mcs_jmp_tbl_871)[temp_v1 & 0xFF]();
    return var_s0;
}
