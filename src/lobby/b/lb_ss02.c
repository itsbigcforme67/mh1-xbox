/* lb_ss02 - agent C 0x005B6420-0x005B65B8: server_select_01 (next server / retry; get_next_server takes no args). */
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
    s32 ret = 0;

    if (CnetWork.x05 == 0) {
        CnetWork.x04 = (u8) (CnetWork.x04 + 1);
        if (CnetWork.x04 < BsLbsCount) {
            if (get_next_server() == 0) {
                return -1;
            }
            ret = 1;
        } else {
            PPP_ErrorStatus = -0x11;
            ret = -2;
        }
    } else if (CnetWork.x05 == 1 || CnetWork.x05 == 2) {
        if (BsLbsCount == 1) {
            ret = -2;
            PPP_ErrorStatus = -0x11;
        } else {
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_1 = (u8) (COM_R_No_1 + 1);
            memset(&network_work, 0, 0x2C);
            F(s8, pNet, 6) = 1;
            cnLbc_MoveMenuServerSelect(&network_work);
        }
    } else if (CnetWork.x05 == 3) {
        if (mcs_connect_flag == 1 || mcs_connect_flag == 2) {
            mcs_connect_flag = 0;
        } else {
            CnetWork.x04 = (u8) (CnetWork.x04 + 1);
        }
        if (CnetWork.x04 < BsLbsCount) {
            if (get_next_server() == 0) {
                return -1;
            }
            ret = 1;
        } else {
            PPP_ErrorStatus = -0x11;
            ret = -2;
        }
    }
    return ret;
}
