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
    s32 var_s0;
    u8 temp_v0;

    var_s0 = 0;
    switch (CnetWork.x05) {        /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        COM_R_No_0 = 2;
        COM_R_No_1 = 0;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        create_server_table(0, CnetWork.x05);
        if (get_next_server() == 0) {
            return -1;
        }
block_21:
    default:                                        /* switch 1 */
        return var_s0;
    case 2:                                         /* switch 1 */
        /* fallthrough */
    case 1:                                         /* switch 1 */
        COM_R_No_1 = 2;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        memset(&network_work, 0, 0x2C);
        cnLbc_MoveMenuServerSelect(&network_work);
        goto block_21;
    case 3:                                         /* switch 1 */
        temp_v0 = mcs_connect_flag;
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            if (CnetWork.x06 == 0) {
                COM_R_No_0 = 2;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                create_server_table(1, CnetWork.x05);
                goto block_21;
            }
            COM_R_No_0 = 2;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            create_server_table(0, CnetWork.x05);
            if (get_next_server() == 0) {
                return -1;
            }
            goto block_21;
        case 1:                                     /* switch 2 */
            MMBB_LOGIN = 1;
            var_s0 = -5;
            mcs_connect_flag = 2U;
            goto block_21;
        case 2:                                     /* switch 2 */
            if (CnetWork.x06 == 0) {
                COM_R_No_0 = 2;
                COM_R_No_1 = 0;
                COM_R_No_2 = 0;
                COM_R_No_3 = 0;
                create_server_table(0, CnetWork.x05);
                goto block_21;
            }
            COM_R_No_0 = 2;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            create_server_table(0, CnetWork.x05);
            if (get_next_server() == 0) {
                return -1;
            }
            goto block_21;
        }
        break;
    }
}
