/* lb_num01 - agent C 0x005BD1D0-0x005BD2B4: CallBack_NoticeUserMiniData (store received mini data into lbCommer or call lb_check_mini_data; int counter with (s8) casts at assignment). */
#include "lobby_b.h"
typedef struct { char id[8]; char name[0x10]; u8 pad18[4]; char mini[0x40]; } LUSER;
void CallBack_NoticeUserMiniData(CNET_RES res) {
    LUSER u;
    s32 i;
    u8 *p;

    cnLBS_Get_NoticeUserMiniData(&u);
    if (softdip_ck(0xF1) == 1) {
        lb_check_mini_data((s8)Lb_get_plID(&u), &u, u.mini);
        return;
    }
    i = 0;
    p = (u8 *)lbCommer;
    do {
        if (memcmp(p, &u, 8) == 0) {
            memcpy((u8 *)&lbCommer[(s8)i] + 0x1C, u.mini, 0x40);
            return;
        }
        i = (s8)(i + 1);
        p += 0x5C;
    } while (i < 8);
}
