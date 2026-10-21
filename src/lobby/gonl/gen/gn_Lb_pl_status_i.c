/* G online (PC): Lb_pl_status_i, written from Monster Hunter G's code at lobby 0x58DD90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 cnWrap_SoundRequest();
extern u8 PitMenu;
extern u8 game_w;
extern u8 pNet;
s32 Lb_PlStatusSet();                         /* extern */
s32 Lb_get_comment();                     /* extern */
extern s32 Lb_pl_status_t;
extern s32 lbCommer;
extern s32 lb_sys;
extern s32 player_work;

void Lb_pl_status_i(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 temp_a0;
    u8 *temp_s0;

    temp_a0 = (*(u8 *)((u8 *)&game_w + 0x3A));
    Lbc_init_network_work((s32) temp_a0, arg1, arg2, arg3);
    Lbc_set_prim(0, (s32) &Lb_pl_status_t, 0, 0);
    temp_s0 = (*(void **)((u8 *)(((u8 *)&player_work + (temp_a0 * 0xA00))) + 0x3B0));
    Lb_get_comment((u8 *)&lbCommer + ((*(u16 *)((u8 *)(temp_s0) + 0xC)) * 0x5C));
    if ((temp_s0 == NULL) || ((*(u8 *)((u8 *)(temp_s0) + 0)) == 0)) {
        lb_sys = 0;
        (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
        return;
    }
    (*(s8 *)((u8 *)&PitMenu + 0x10)) = 0;
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) = (u8) (*(u16 *)((u8 *)(temp_s0) + 0xC));
    Lb_PlStatusSet((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)));
    cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

