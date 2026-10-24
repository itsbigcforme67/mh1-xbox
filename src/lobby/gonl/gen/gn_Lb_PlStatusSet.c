/* G online (PC): Lb_PlStatusSet, written from Monster Hunter G's code at lobby 0x59E1C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Skill_set_PL();
void *GetAdrsMiniData();                            /* extern */
s32 Lb_get_comment();                     /* extern */
extern s32 lb_player;
extern u8 player_work;

void Lb_PlStatusSet(s32 arg0) {
    s32 temp_s0;
    u8 *temp_a0;
    u8 *temp_v0;

    temp_v0 = GetAdrsMiniData();
    temp_s0 = arg0 & 0xFF;
    temp_a0 = &player_work + (temp_s0 * 0xA00);
    (*(s16 *)((u8 *)(temp_a0) + 0x35E)) = (s16) (*(s16 *)((u8 *)(temp_v0) + 0xA));
    (*(s16 *)((u8 *)(temp_a0) + 0x360)) = (s16) (*(s16 *)((u8 *)(temp_v0) + 0xC));
    (*(s16 *)((u8 *)(temp_a0) + 0x362)) = (s16) (*(s16 *)((u8 *)(temp_v0) + 0xE));
    (*(u8 *)((u8 *)(temp_a0) + 0x352)) = (u8) (*(u8 *)((u8 *)(temp_v0) + 3));
    (*(u8 *)((u8 *)(temp_a0) + 0x353)) = (u8) (*(u8 *)((u8 *)(temp_v0) + 4));
    (*(u8 *)((u8 *)(temp_a0) + 0x354)) = (u8) (*(u8 *)((u8 *)(temp_v0) + 5));
    (*(u8 *)((u8 *)(temp_a0) + 0x355)) = (u8) (*(u8 *)((u8 *)(temp_v0) + 6));
    (*(u8 *)((u8 *)(temp_a0) + 0x356)) = (u8) (*(u8 *)((u8 *)(temp_v0) + 7));
    (*(u8 *)((u8 *)(temp_a0) + 0x357)) = (u8) (*(u8 *)((u8 *)(temp_v0) + 8));
    Skill_set_PL(temp_a0);
    Lb_get_comment((u8 *)&lb_player + (temp_s0 * 0x38) + 0x24);
}

