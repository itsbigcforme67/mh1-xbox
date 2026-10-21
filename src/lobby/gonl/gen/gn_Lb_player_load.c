/* G online (PC): Lb_player_load, written from Monster Hunter G's code at lobby 0x566E50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 armor_create_model();
s32 parts_init();
s32 pl_chr_set3();
s32 pl_create_model();
s32 weapon_create_model();
s32 yure_init();
extern u8 cw;
void Lb_player_load(void *arg0) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB6F)) = 1;
    pl_create_model((s32) (*(u16 *)((u8 *)(arg0) + 0xC)));
    weapon_create_model((s32) (*(u8 *)((u8 *)(arg0) + 0x34C)), (s32) (*(u16 *)((u8 *)(arg0) + 0xC)), 0);
    armor_create_model(arg0);
    yure_init(arg0);
    pl_chr_set3(arg0, 1, 0, 0, 0);
    pl_chr_set3(arg0, 0x65, 0, 0, 1);
    parts_init(arg0);
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB6F)) = 0;
    (*(s8 *)((u8 *)(((*(u16 *)((u8 *)(arg0) + 0xC)) + (*(void **)&cw))) + 0xB64)) = 1;
}

