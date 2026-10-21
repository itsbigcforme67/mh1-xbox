/* G online (PC): CallBack_Event_MatchEntryUser, written from Monster Hunter G's code at lobby 0x587850 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_chat();
s32 cnWrap_SoundRequest();
extern u8 ClassInfo;
extern u8 cw;
s32 cnLBS_Get_MatchEntryJoinUser(); /* extern */
extern s32 lb_sys;
extern s8 mhRule;

void CallBack_Event_MatchEntryUser(void) {
    s32 temp_a2_2;
    s32 var_s0;
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a2;

    temp_a2 = (*(void **)&cw);
    var_s0 = 0;
    if (((*(u16 *)((u8 *)(temp_a2) + 0xF752)) + 1) == (mhRule + 1)) {
        var_s0 = 1;
    }
    cnLBS_Get_MatchEntryJoinUser((*(u8 *)((u8 *)&ClassInfo + 0x8)) & 0xFFFF, temp_a2 + 0xF752, temp_a2 + 0xF754);
    if ((lb_sys != 8) && (lb_sys != 9) && ((mhRule + 1) >= 2)) {
        temp_a1 = (*(void **)&cw);
        if (((*(u16 *)((u8 *)(temp_a1) + 0xF752)) == mhRule) && ((*(u8 *)((u8 *)(temp_a1) + 0xF751)) != 0)) {
            Lb_put_chat(0, 0);
            cnWrap_SoundRequest(0x12);
        }
    }
    if (var_s0 == 1) {
        temp_a2_2 = mhRule + 1;
        if (temp_a2_2 >= 2) {
            temp_a1_2 = (*(void **)&cw);
            if ((((*(u16 *)((u8 *)(temp_a1_2) + 0xF752)) + 1) < temp_a2_2) && ((*(u8 *)((u8 *)(temp_a1_2) + 0xF751)) != 0)) {
                Lb_put_chat(1, 0);
            }
        }
    }
}

