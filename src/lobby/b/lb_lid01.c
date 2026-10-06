/* lb_lid01 - agent C 0x005B8170-0x005B84CC: login id select (separate index/offset locals fix s0/s1). */
#include "lobby_a.h"
extern char D_3C6FC8[];
typedef struct { u8 pad00[0x5]; u8 x05; u8 padEND[0x2A]; } CNW;
extern CNW CnetWork;
void cnetGet_Login_UserID(u8, u8 *);
void cnetGet_Login_UserHandle(u8, u8 *);
void cnetGet_Login_UserMiniData(u8, u8 *);
void lbc_login_id_select(void) {
    s32 s3;
    s32 s2;
    s32 s1;
    s32 s0;
    s32 ix;
    s32 of;
    u8 st;
    u8 *stp;
    s32 r;

    st = cw[0x2C34];
    stp = cw + 0x2C34;
    switch (st) {
    case 0:
        *stp = st + 1;
        cw[2] = 0;
        cw[1] = 0;
        memset(cw + 0xB, 0, 0x20);
        memset(cw + 0x2B, 0, 0x44);
        memset(cw + 0x6F, 0, 0x100);
        cw[0] = cnetGet_Login_NoOfUserAccount();
        s3 = 0;
        if (0 < cw[0]) {
            s2 = 0;
            s1 = 0;
            s0 = 0;
            do {
                cnetGet_Login_UserID(s3, cw + s2 + 0xB);
                cnetGet_Login_UserHandle(s3, cw + s1 + 0x2B);
                cnetGet_Login_UserMiniData(s3, cw + s0 + 0x6F);
                s3 += 1;
                s2 += 8;
                s1 += 0x11;
                s0 += 0x40;
            } while (s3 < cw[0]);
        }
        if (cnWrap_IsBBConnect() != 0) {
            if (cw[0] == 0) {
                cw[0x2C33] = 5;
                cw[0x2C34] = 0;
                return;
            }
        }
        break;
    case 1:
        *stp = st + 1;
        Lbc_init_network_work();
        F(u8, pNet, 6) = cw[2];
        F(u8, pNet, 8) = cw[1];
        return;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            cw[0x2C34]++;
            if (CnetWork.x05 == 0) {
                fade_set(2);
                return;
            }
        }
        break;
    case 3:
        r = net_SetMenu_SelectHandleName(&network_work);
        switch (r) {
        case 0:
            cw[0x2C34]++;
            return;
        case -1:
        case -2:
            break;
        }
        break;
    case 4:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            cw[0x2C34]++;
            Lbc_set_prim(0, 0, 0);
        }
    case 5:
        cw[0x2C34]++;
        ix = F(u8, pNet, 8);
        cw[1] = ix;
        CallBackWaitInit();
        Set_userdata((int)&player_work + (game_w.master * 0xA00));
        of = ix * 0x11;
        memcpy(cw + of + 0x2B, D_3C6FC8, 0x10);
        if ((s8)cw[0xB + ix * 8] == 0) {
            cnLBS_Send_LoginUserAccount(0, cw + of + 0x2B, cw + (ix << 6) + 0x6F);
        } else {
            cnLBS_Send_LoginUserAccount(cw + ix * 8 + 0xB, cw + of + 0x2B, cw + (ix << 6) + 0x6F);
        }
        cnetGet_Login_DecideUserID(cw + 0x440);
        cnetGet_Login_DecideUserHandle(cw + 0x448);
        return;
    case 6:
        Check_CallBackWait();
        break;
    }
}
