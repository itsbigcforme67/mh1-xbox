#include "lobby_a.h"
extern s32 no_pl;
extern u8 *lbmw;
extern char pf_room_member[];
void disp_lm_room_member(void) {
    s32 arr[3];
    s32 j;
    s32 *pp;
    s8 m;
    s32 r;
    s32 sel;
    s32 id;

    switch (lbmw[9]) {
    case 0:
        Disp_lb_menu();
        j = 0;
        pp = arr;
        do {
            m = lbmw[j + 0xD];
            if (m > 0) {
                r = Lb_room_member(m & 0xFF, *(u8 *)0x39DAD4);
            } else {
                r = no_pl;
            }
            *pp = r;
            j++;
            pp++;
        } while ((u32)j < 3);
        sel = -1;
        if (lbmw[8] != 0) {
            sel = lbmw[0xA];
        }
        *(s32 *)(pf_room_member + 0xC) = (s32)arr;
        DispFrameList(pf_room_member, 0, sel);
        break;
    case 1:
    case 2:
        r = Lb_room_member(lbmw[lbmw[0xA] + 0xD], 1);
        if (r != 0) {
            id = Lb_get_plID(r) & 0xFF;
            Lb_PlayerStatus((u8 *)lb_player + id * 0x38, lbmw[0xB]);
            if (lbmw[0xB] == 0) {
                Disp_FriendListEntry(0x110, (lbmw[9] - 1) & 0xFF);
            }
        }
        break;
    case 3:
        break;
    }
}
