#include "lobby_f.h"
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern s8 PPP_ErrorStatus;
extern u8 * pNet;
extern u8 BsLbsCount;
extern u8 COM_R_No_1;
extern u8 mcs_connect_flag;
typedef struct { u8 pad00[0x4]; u8 x04; u8 x05; u8 padEND[0x2A]; } CNW;
extern CNW CnetWork;
s32 server_select_01(void) {
    s32 var_s0;
    u8 temp_v0;

    var_s0 = 0;
    switch (CnetWork.x05) {        /* irregular */
    case 0:
        CnetWork.x04 = (u8) (CnetWork.x04 + 1);
        if (CnetWork.x04 < BsLbsCount) {
            if (get_next_server(CnetWork.x05) == 0) {
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
            CnetWork.x04 = (u8) (CnetWork.x04 + 1);
        }
        if (CnetWork.x04 < BsLbsCount) {
            if (get_next_server(1U, CnetWork.x05) == 0) {
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
