/* G online (PC): lbc_login_patch, written from Monster Hunter G's code at lobby 0x57AD50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 Lbs_load();
s32 Net_trans_set();
s32 To_LogOut();
s32 all_reset();
s32 cnLBS_Answer_PatchFinish();
s32 cnLBS_Get_PatchInformation();
s32 cnWrap_InitWork();
s32 ms_net_patch_set();
s32 ms_net_patch_set_init();
extern u8 cw;
extern u8 jtbl_575_0065E890;
extern u8 network_work;

void lbc_login_patch(void) {
    s32 temp_v0;
    u8 temp_a1;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_v1;

    temp_a2 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0xB9C));
    temp_a3 = temp_a2 + 0xB9C;
    switch (temp_a1) {
    case 0:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
        cnLBS_Get_PatchInformation((*(void **)&cw) + 0x11A94);
        ms_net_patch_set_init();
        all_reset();
        (*(s8 *)((u8 *)&network_work + 0x31)) = 1;
        return;
    case 1:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        (*(s8 *)((u8 *)&network_work + 0x31)) = 1;
        return;
    case 2:
        (*(s8 *)((u8 *)&network_work + 0x31)) = 1;
        temp_v0 = ms_net_patch_set();
        if (temp_v0 == 1) {
            temp_v1 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9C)) + 1);
            cnWrap_InitWork(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        } else if (temp_v0 == -1) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 6U;
            cnWrap_InitWork(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        Net_trans_set();
        return;
    case 3:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        Lbs_load((s32) &jtbl_575_0065E890, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        (*(s8 *)((u8 *)&network_work + 0x31)) = 0;
        return;
    case 4:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        cnLBS_Answer_PatchFinish();
        return;
    case 5:
        Check_CallBackWait((s32) &jtbl_575_0065E890, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        return;
    case 6:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        Lbs_load((s32) &jtbl_575_0065E890, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        (*(s8 *)((u8 *)&network_work + 0x31)) = 0;
        return;
    case 7:
        (*(s8 *)((u8 *)(temp_a2) + 0xB70)) = 1;
        To_LogOut(1, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        return;
    default:
        To_LogOut(4, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        return;
    }
}

