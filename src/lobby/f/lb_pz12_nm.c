/* lb_pz12_nm - lobby.bin 0x00597640-0x00597B04: Plaza_add_friend(id), near-match (15 instructions differ: only the register numbers of sw/a/st: original has sw in a3, a in a1, step pointer in a2; mine a1/a2/a3). NOT built. */
#pragma readonly_strings on
#include "lbui_proto.h"
#define X05 (*(u8 *)&a->x05)
extern u8 my_user_id[];
extern u8 D_32D471[];
int Lb_select();
int SaveNetFile_ForLobby();
void SaveNetFile_init();
int net_Check_FriendData();
int net_Check_FriendFree();
void cpn_PutCNData();
void str_pause();
void str_fadein_vol();
void SetDialogYesNo();

int Plaza_add_friend(id)
u8 *id;
{
    u16 sw = Get_sw2(0);
    LB_NETW *a;
    int r;
    u8 *e;
    u8 *st;

    pNet->x0C = 1;
    a = pNet;
    st = &a->x05;
    switch (*st) {
    case 0:
        (*st)++;
        SetDialogData(0x1D, 2);
        SetDialogYesNo(0);
        cw[0x2C08] = 0;
        *(s8 *)0x3F36AB = 0;
        break;
    case 1:
        switch (Lb_select()) {
        case 0:
            pNet->x05++;
            if (memcmp(id, my_user_id, 8) == 0) {
                pNet->x05 = 6;
                SetDialogData(0x1E, 3);
            } else if (net_Check_FriendData(Friend_data, 0x32, id) == -1) {
                if (net_Check_FriendFree(Friend_data, 0x32) == -1) {
                    pNet->x05 = 6;
                    SetDialogData(0x24, 3);
                } else {
                    SetDialogData(0x23, 2);
                    SetDialogYesNo(0);
                }
            } else {
                SetDialogData(0x22, 2);
                SetDialogYesNo(0);
            }
            break;
        case 3:
            pNet->x05 = 6;
            SetDialogData(0x1F, 3);
            break;
        }
        break;
    case 2:
        switch (Lb_select()) {
        case 0:
            r = net_Check_FriendData(Friend_data, 0x32, id);
            if (r == -1) {
                r = net_Check_FriendFree(Friend_data, 0x32);
                if (r == -1) {
                    r = 0;
                }
                pNet->x0D = 0;
            } else {
                pNet->x0D = 1;
            }
            e = Friend_data + r * 0x30;
            memset(e, 0, 0x30);
            memcpy(e, id, 9);
            memcpy(e + 8, id + 8, 0x11);
            str_pause(0, 1);
            str_pause(1, 1);
            *(u8 *)&pNet->x12 = 3;
            cpn_PutCNData();
            SaveNetFile_init();
            pNet->x05++;
            cw[0x2C08] = 0;
            *(s8 *)0x3F36AB = 0;
            break;
        case 3:
            pNet->x05 = 0;
            break;
        }
        break;
    case 3:
        if (*(u8 *)&a->x12 == 0) {
            switch (SaveNetFile_ForLobby()) {
            case -1:
                str_pause(0, 0);
                str_pause(1, 0);
                if (((LB_CW *)cw)->x35D5 != 0) {
                    str_fadein_vol(0, 0x1E, D_32D471[*(u8 *)0x3F3404 * 2]);
                }
                cw[0x2C08] = 1;
                *(s8 *)0x3F36AB = 1;
                pNet->x05 = 0;
                return 0;
            case 1:
                str_pause(0, 0);
                str_pause(1, 0);
                if (((LB_CW *)cw)->x35D5 != 0) {
                    str_fadein_vol(0, 0x1E, D_32D471[*(u8 *)0x3F3404 * 2]);
                }
                pNet->x05 = 0;
                cw[0x2C08] = 1;
                *(s8 *)0x3F36AB = 1;
                return 0;
            }
        } else {
            *(u8 *)&a->x12 = *(u8 *)&a->x12 - 1;
        }
        break;
    case 4:
        X05++;
        break;
    case 5:
        if (sw & 0x20) {
            a->x05 = 0;
            cnWrap_SoundRequest(0);
            cw[0x2C08] = 1;
            *(s8 *)0x3F36AB = 1;
            return 0;
        }
        break;
    case 6:
        if (sw & 0x20) {
            a->x05 = 0;
            cnWrap_SoundRequest(0);
            cw[0x2C08] = 1;
            *(s8 *)0x3F36AB = 1;
            return 1;
        }
        break;
    }
    return 2;
}
