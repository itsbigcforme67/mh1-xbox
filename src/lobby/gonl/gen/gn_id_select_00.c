/* G online (PC): id_select_00, written from Monster Hunter G's code at lobby 0x5BA4D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbc_set_prim();
s32 SetHelpLineMsg();
s32 SetSceneTitle();
s32 memcmp();
s32 transOtSelectHandleName();
extern u8 User_data;
extern u8 cw;

void id_select_00(void *arg0) {
    s32 var_s1;
    s8 var_s2;

    Lbc_set_prim(0, 0, (s32) transOtSelectHandleName, 0);
    (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
    SetSceneTitle(4);
    SetHelpLineMsg(1);
    (*(s8 *)((u8 *)(arg0) + 0x29)) = 0;
    var_s2 = 0;
    var_s1 = 0;
loop_1:
    if (memcmp((*(s32 *)&cw) + var_s1 + 0x2B, &(*(u8 *)((u8 *)&User_data + 0x8)), 0x10) == 0) {
        (*(s8 *)((u8 *)(arg0) + 0x29)) = var_s2;
        return;
    }
    var_s2 = (s8) (((s8) ((var_s2 + 1))));
    var_s1 += 0x11;
    if (var_s2 >= 3) {
        return;
    }
    goto loop_1;
}

