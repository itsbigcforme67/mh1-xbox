/* lb_gn01 - near-match fixes 0x005D62F0-0x005D639C: lb_send_dataTU. Whole file in lb_n.c. */
#include "lobby_f.h"













extern u8 D_3E55F0[], D_3E5FF0[], D_3E69F0[], D_3E73F0[], D_3E7DF0[], D_3E87F0[], D_3E91F0[];




#ifdef VERSION_MH2
#ifdef VERSION_MH2
extern char lb_my_lbsid[];
#ifdef VERSION_MH2
extern char lb_my_lbsid[];
void lb_send_dataTU(len, type, data, a3)
int len;
int type;
u8 *data;
int a3;
{
    if (Online_ck() != 0 && Lbs_CheckMatchingFlag() != 1) {
        sendDat[0] = type;
        if (len & 0xFF) {
            flMemcpy(sendDat + 1, data, len & 0xFF);
        }
        if (memcmp(lb_my_lbsid, (void *)a3, 8) == 0) {
            Lb_send_data_to_myself((u8)(((u8)len) + 1), type, data);
            return;
        }
        cnLBS_Send_ChatBinaryTU(a3, sendDat, (u8)(((u8)len) + 1), CallBack_Result_SendChatBinaryTU);
    }
}
#else
void lb_send_dataTU(len, type, data, a3)
int len;
u8 type;
u8 *data;
int a3;
{
    if (Online_ck() != 0 && Lbs_CheckMatchingFlag() != 1) {
        sendDat[0] = type;
        if (len & 0xFF) {
            flMemcpy(sendDat + 1, data, len & 0xFF);
        }
        if (memcmp(lb_my_lbsid, (void *)a3, 8) == 0) {
            ((void (*)())Lb_send_data_to_myself)((u8)((len & 0xFF) + 1), type, data);
            return;
        }
        cnLBS_Send_ChatBinaryTU(a3, sendDat, (u8)(((u8)len) + 1), CallBack_Result_SendChatBinaryTU);
    }
}
#endif
#else
void lb_send_dataTU(len, type, data, a3)
int len;
u8 type;
u8 *data;
int a3;
{
    if (Online_ck() != 0 && Lbs_CheckMatchingFlag() != 1) {
        if (memcmp((void *)0x5CAA38, (void *)a3, 8) == 0) {
            Lb_send_data_to_myself((u8)((len & 0xFF) + 1), type, data);
            return;
        }
        sendDat[0] = type;
        if (len & 0xFF) {
            flMemcpy(sendDat + 1, data, len & 0xFF);
        }
        cnLBS_Send_ChatBinaryTU(a3, sendDat, (u8)(((u8)len) + 1), CallBack_Result_SendChatBinaryTU);
    }
}
#endif
#else
void lb_send_dataTU(len, type, data, a3)
int len;
u8 type;
u8 *data;
int a3;
{
    if (Online_ck() != 0 && Lbs_CheckMatchingFlag() != 1) {
        sendDat[0] = type;
        if (len & 0xFF) {
            flMemcpy(sendDat + 1, data, len & 0xFF);
        }
        cnLBS_Send_ChatBinaryTU(a3, sendDat, (u8)(((u8)len) + 1), CallBack_Result_SendChatBinaryTU);
    }
}
#endif
