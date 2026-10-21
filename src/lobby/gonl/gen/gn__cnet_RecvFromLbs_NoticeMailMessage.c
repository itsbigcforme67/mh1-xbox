/* G online (PC): _cnet_RecvFromLbs_NoticeMailMessage, written from Monster Hunter G's code at lobby 0x5840B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 fade_set();
extern u8 cw;
s32 cnWrap_BgmVolume();                  /* extern */
extern u8 put_back;

void _cnet_RecvFromLbs_NoticeMailMessage(void) {
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    (*(u8 *)((u8 *)(temp_v1) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9B)) + 1);
    cnWrap_BgmVolume(0);
    Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    Lbc_set_prim((s32) &put_back, 0, 0, 0);
    fade_set(0xA);
}

