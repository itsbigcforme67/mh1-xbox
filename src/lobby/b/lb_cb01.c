/* lb_cb01 - agent C 0x005C13C0-0x005C14E4: CallBack_Event_LobbyLeaver (drop leaver from lbCommer, recompute cw+3 min id; '0 > memcmp()' + p3 local, see lb_p01.c). */
#include "lobby_b.h"
typedef struct { u8 pad0[3]; s8 x03; } CWS_ll;
#define CWX ((CWS_ll *)cw)
void CallBack_Event_LobbyLeaver(CNET_RES res) {
    char user[0x5C];
    s32 i;
    u8 *p;
    s32 off;
    s32 j;

    cnLBS_Get_RoomLeaveUser(user);
    i = 0;
    p = (u8 *)lbCommer;
    do {
        if (memcmp(p, user, 8) == 0) {
            memset(&lbCommer[i], 0, 0x5C);
            break;
        }
        i++;
        p += 0x5C;
    } while (i < 8);
    if (memcmp(cw + 3, user, 8) == 0) {
        j = 0;
        off = 0;
        cw[3] = 0;
        do {
            u8 *k;
            u8 *p3;
            u8 *e;
            e = cw + off;
            if ((s8)e[0x132C] != 0 && ((p3 = (k = cw) + 3, *(s8 *)p3 == 0) || 0 > memcmp(p3, e + 0x132C, 8))) {
                memcpy(cw + 3, cw + off + 0x132C, 8);
            }
            j++;
            off += 0x2FC;
        } while (j < 8);
    }
}
