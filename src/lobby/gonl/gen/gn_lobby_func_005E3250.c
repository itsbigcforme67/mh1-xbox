/* G online (PC): lobby_func_005E3250, written from Monster Hunter G's code at lobby 0x5E3250 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Init_LobbyBgProcess();
extern u8 CnetSys_w;
extern s32 CnetSys_g;

void lobby_func_005E3250(void *arg0) {
    u8 *var_a3;   /* (s32 *) */
    s32 temp_a1;
    s32 var_a6;
    u32 var_a4;
    u32 var_a5;

    var_a6 = 1;
    var_a4 = 0;
    var_a5 = 0;
    var_a3 = &CnetSys_g;
loop_1:
    var_a4 += 1;
    var_a3 += 0x164;
    if ((u16) (*(u16 *)((u8 *)&CnetSys_w + 0x43B4)) >= var_a4) {
        temp_a1 = (*(s32 *)((u8 *)(arg0) + 0x18));
        if (temp_a1 != (temp_a1 & (*(s32 *)((u8 *)(var_a3) + 0x430C)))) {
            var_a6 = 0;
        } else {
            var_a5 += 1;
            if (var_a5 < 0x1AU) {
                goto loop_1;
            }
        }
    }
    if ((*(u8 *)((u8 *)(arg0) + 0x23)) == 2) {
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
        cnLBS_Init_LobbyBgProcess();
        return;
    }
    if (var_a6 != 0) {
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 1;
        (*(s8 *)((u8 *)(arg0) + 0x22)) = 0;
    }
}

