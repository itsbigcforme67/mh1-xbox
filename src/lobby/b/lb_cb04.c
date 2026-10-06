/* lb_cb04 - agent C 0x005C0DB0-0x005C0EAC: CallBack_Event_MatchStart (copy the four room members into room_member_* tables, start stage). */
#include "lobby_b.h"
extern char room_member_id[];
extern char room_member_handle[];
extern char room_member_mini_data[];
void CallBack_Event_MatchStart(CNET_RES res) {
    s8 i;
    s32 off;
    char *id;
    char *handle;
    char *mini;

    F(s8, (u8 *)cw, 0x2C0C) = 1;
    Lbc_set_prim(0, 0, 0);
    *(s8 *)0x3F3404 = 0;
    Lb_send_stage();
    fade_set(0xA);
    i = 0;
    off = 0;
    id = room_member_id;
    handle = room_member_handle;
    mini = room_member_mini_data;
    do {
        memcpy(id, cw + off + 0x73C, 8);
        memcpy(handle, cw + off + 0x744, 0x11);
        memcpy(mini, cw + off + 0x756, 0x40);
        off += 0x2FC;
        id += 8;
        i++;
        handle += 0x11;
        mini += 0x40;
    } while (i < 4);
}
