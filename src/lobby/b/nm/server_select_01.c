#include "lobby_f.h"
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern s8 PPP_ErrorStatus;
extern u8 * pNet;
extern u8 BsLbsCount;
extern u8 COM_R_No_1;
extern u8 mcs_connect_flag;
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
s32 server_select_01(void) {
    s32 var_s0;
    u8 temp_v0;

    var_s0 = 0;
    switch (F(u8, &CnetWork, 5)) {        /* irregular */
    case 0:
        F(u8, &CnetWork, 4) = (u8) (F(u8, &CnetWork, 4) + 1);
        if (F(u8, &CnetWork, 4) < BsLbsCount) {
            if (get_next_server(F(u8, &CnetWork, 5)) == 0) {
                return -1;
            }
            var_s0 = 1;
            goto block_23;
        }
        PPP_ErrorStatus = -0x11;
        var_s0 = -2;
block_23:
    default:
        return var_s0;
    case 2:
        /* fallthrough */
    case 1:
        if (BsLbsCount == 1) {
            var_s0 = -2;
            PPP_ErrorStatus = -0x11;
        } else {
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_1 = (u8) (COM_R_No_1 + 1);
            memset(&network_work, 0, 0x2C);
            F(s8, pNet, 6) = 1;
            cnLbc_MoveMenuServerSelect(&network_work);
        }
        goto block_23;
    case 3:
        temp_v0 = mcs_connect_flag;
        if ((temp_v0 == 1) || (temp_v0 == 2)) {
            mcs_connect_flag = 0U;
        } else {
            F(u8, &CnetWork, 4) = (u8) (F(u8, &CnetWork, 4) + 1);
        }
        if (F(u8, &CnetWork, 4) < BsLbsCount) {
            if (get_next_server(1U, F(u8, &CnetWork, 5)) == 0) {
                return -1;
            }
            var_s0 = 1;
            goto block_23;
        }
        PPP_ErrorStatus = -0x11;
        var_s0 = -2;
        goto block_23;
    }
}
