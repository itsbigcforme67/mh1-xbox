/* G online (PC): Lb_set_player, written from Monster Hunter G's code at lobby 0x566C30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_pl_to_normal();
s32 Lb_trans_pl();
void * get_prim_ptr();
void * memcpy();
s16 get_prim();                                     /* extern */
extern s32 lb_player;
extern s32 pl01_adr_tbl;
extern u8 player_work;

void Lb_set_player(s32 arg0, void *arg1, void *arg2) {
    s16 temp_a0;
    u16 temp_a3;
    void **temp_s1;
    u8 *temp_s0;

    temp_a3 = arg0 & 0xFF;
    temp_s1 = (u8 *)&lb_player + (temp_a3 * 0x38);
    temp_s0 = &player_work + (temp_a3 * 0xA00);
    *temp_s1 = temp_s0;
    (*(s8 *)((u8 *)(temp_s0) + 0)) = 1;
    (*(u16 *)((u8 *)(temp_s0) + 0xC)) = temp_a3;
    memcpy(temp_s1 + 4, arg2, 0x11);
    memcpy(temp_s1 + 0x24, arg1, 8);
    memcpy(temp_s0 + 0x8D4, arg2, 0x11);
    (*(s32 *)((u8 *)(temp_s0) + 0xC0)) = 0x3F800000;
    (*(s32 *)((u8 *)(temp_s0) + 0xBC)) = 0x3F800000;
    (*(s32 *)((u8 *)(temp_s0) + 0xB8)) = 0x3F800000;
    (*(s16 *)((u8 *)(temp_s0) + 0x300)) = 2;
    (*(s32 *)((u8 *)(temp_s0) + 0x798)) = 0x3F800000;
    (*(s32 *)((u8 *)(temp_s0) + 0x1A0)) = 0x3F800000;
    (*(s32 *)((u8 *)(temp_s0) + 0x1F0)) = 0x3F800000;
    (*(s16 *)((u8 *)(temp_s0) + 0x568)) = get_prim();
    (*(s8 *)((u8 *)(temp_s0) + 0x4D4)) = 1;
    temp_a0 = (*(s16 *)((u8 *)(temp_s0) + 0x568));
    if (temp_a0 != -1) {
        (*(void **)((u8 *)(temp_s0) + 0x564)) = get_prim_ptr((s32) temp_a0);
        (*(s32 *)((u8 *)((*(void **)((u8 *)(temp_s0) + 0x564))) + 0x18)) = (s32) (*(u16 *)((u8 *)(temp_s0) + 0xC));
        (*(s32 (**)(void *))((u8 *)((*(void **)((u8 *)(temp_s0) + 0x564))) + 0x14)) = Lb_trans_pl;
    }
    Lb_pl_to_normal(temp_s0, 0, 0, 0);
    (*(s32 **)((u8 *)(temp_s0) + 0x3CC)) = &pl01_adr_tbl;
    (*(s32 (**)(void *))((u8 *)((*(s32 **)((u8 *)(temp_s0) + 0x3CC))) + 0xC))(temp_s0);
}

