/* lb_bz05 - lobby UI/client 0x005B2570-0x005B25F4: tk_sw_on_ck, tk_sw_new_ck, cpn_GetCNData (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char Friend_data[];
extern char tmp_friend_data[];
extern char CNFile[];

s32 tk_sw_on_ck(s32 arg0) {
    return (arg0 & (*(u16 *)0x3F3714 | *(u16 *)0x3F3728)) != 0;
}

s32 tk_sw_new_ck(s32 arg0) {
    return (*(u16 *)0x3F3710 & arg0) != 0;
}

void cpn_GetCNData(void) {
    memcpy(&Friend_data, &CNFile, 0x960);
    memcpy(&tmp_friend_data, &CNFile, 0x960);
}
