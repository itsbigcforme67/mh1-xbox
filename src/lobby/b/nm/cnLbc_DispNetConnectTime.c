#include "lobby_a.h"
extern s32 net_time_flag;
extern u8 COM_R_No_0;
extern u8 model_base_tbl_248[8];
void cnLbc_DispNetConnectTime(void) {
    int temp_v0;

    if (COM_R_No_0 == 3) {
        net_time_flag = 1;
        temp_v0 = pull_set_work(1);
        if (temp_v0 != 0) {
            F(s8, temp_v0, 1) = 0;
            F(s32, temp_v0, 0x20) = (*(s32 *)(model_base_tbl_248 + 4));
            F(s16, F(int, temp_v0, 0x18), 0x14) = (s16) (*(s8 *)(model_base_tbl_248 + 3));
            F(s8, temp_v0, 3) = (s8) (*(s8 *)(model_base_tbl_248 + 2));
        }
    }
}
