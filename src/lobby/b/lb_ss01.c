/* lb_ss01 - agent C 0x005B6270-0x005B6418: server_select_00 (server list/connect state machine; if-else chain, not a switch). */
#include "lobby_a.h"
extern s8 COM_R_No_0;
extern s8 COM_R_No_1;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern s8 MMBB_LOGIN;
extern u8 mcs_connect_flag;
typedef struct { u8 pad00[0x5]; u8 x05; u8 x06; u8 padEND[0x29]; } CNW;
extern CNW CnetWork;
s32 server_select_00(void) {
    s32 ret = 0;

    if (CnetWork.x05 == 0) {
        COM_R_No_0 = 2;
        COM_R_No_1 = 0;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        create_server_table(0, CnetWork.x05);
        if (get_next_server() == 0) {
            return -1;
        }
    } else if (CnetWork.x05 == 1 || CnetWork.x05 == 2) {
        COM_R_No_1 = 2;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        memset(&network_work, 0, 0x2C);
        cnLbc_MoveMenuServerSelect(&network_work);
    } else if (CnetWork.x05 == 3) {
        if (mcs_connect_flag == 0) {
            if (CnetWork.x06 == 0) {
                COM_R_No_0 = 2;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                create_server_table(1, CnetWork.x05);
            } else {
                COM_R_No_0 = 2;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                create_server_table(0, CnetWork.x05);
                if (get_next_server() == 0) {
                    return -1;
                }
            }
        } else if (mcs_connect_flag == 1) {
            MMBB_LOGIN = 1;
            ret = -5;
            mcs_connect_flag = 2;
        } else if (mcs_connect_flag == 2) {
            if (CnetWork.x06 == 0) {
                COM_R_No_0 = 2;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                create_server_table(0, CnetWork.x05);
            } else {
                COM_R_No_0 = 2;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                create_server_table(0, CnetWork.x05);
                if (get_next_server() == 0) {
                    return -1;
                }
            }
        }
    }
    return ret;
}
