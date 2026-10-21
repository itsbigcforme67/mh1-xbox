/* G online (PC): CallBack_Event_MatchStart, written from Monster Hunter G's code at lobby 0x586CC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 game_w;
extern u8 player_work;
s32 Lbc_SendMiniData();                       /* extern */
s32 Lbc_set_prim(); /* extern */
s32 lobby_func_0059ACD0();               /* extern */
s32 memcpy();         /* extern */
s32 memset();        /* extern */
extern u8 room_member_handle;
extern u8 room_member_id;
extern u8 room_member_mini_data;

void CallBack_Event_MatchStart(void) {
    u8 *var_s0;   /* (s32 *) */
    u8 *var_s1;   /* (s32 *) */
    u8 *var_s3;   /* (s32 *) */
    s32 var_s2;
    s64 var_s4;
    u8 temp_a0;

    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB74)) = 1;
    Lbc_set_prim(0, 0, 0, 0);
    lobby_func_0059ACD0(0);
    temp_a0 = (*(u8 *)((u8 *)&game_w + 0x3A));
    *(&(*(u8 *)((u8 *)&player_work + 0x916)) + (temp_a0 * 0xA00)) = 0;
    Lbc_SendMiniData(temp_a0);
    var_s4 = 0;
    var_s3 = &room_member_id;
    var_s2 = 0;
    var_s1 = &room_member_handle;
    var_s0 = &room_member_mini_data;
    do {
        memset(var_s3, 0, 8);
        memcpy(var_s3, (*(void **)&cw) + var_s2 + 0x21C, 8);
        memcpy(var_s1, (*(void **)&cw) + var_s2 + 0x224, 0x11);
        memcpy(var_s0, (*(void **)&cw) + var_s2 + 0x238, 0x40);
        var_s3 += 8;
        var_s2 += 0x5C;
        var_s4 = (s64) ((var_s4 + 1) << 0x38) >> 0x38;
        var_s1 += 0x11;
        var_s0 += 0x40;
    } while (var_s4 < 4);
}

