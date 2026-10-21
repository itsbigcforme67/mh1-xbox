/* G online (PC): Lb_set_mini_data_to_pl, written from Monster Hunter G's code at lobby 0x58B9D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_weapon_id();
s32 memcpy(); /* extern */
extern u8 Battle_type;
extern u8 player_work;

void Lb_set_mini_data_to_pl(s64 arg0) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s32 *)(g_frame + 0x30))
#define sp33 (*(s32 *)(g_frame + 0x33))
#define sp3A (*(s32 *)(g_frame + 0x3A))
#define sp40 (*(u8 *)(g_frame + 0x40))
#define sp44 (*(u8 *)(g_frame + 0x44))
#define sp48 (*(u8 *)(g_frame + 0x48))
#define sp49 (*(u8 *)(g_frame + 0x49))
#define sp4A (*(u8 *)(g_frame + 0x4A))
    s16 temp_a3;
    s16 temp_a4;
    u8 *temp_s1;

    temp_s1 = (u8 *)&player_work + ((((s8) (arg0))) * 0xA00);
    memcpy(&sp30, (s32 *)0x1C);
    (*(s32 *)((u8 *)(temp_s1) + 0x5FC)) = sp44;
    (*(u8 *)((u8 *)(temp_s1) + 0x11)) = sp49;
    (*(u8 *)((u8 *)(temp_s1) + 0x34E)) = sp48;
    (*(u8 *)((u8 *)(temp_s1) + 0x8D3)) = sp4A;
    (*(u8 *)((u8 *)(temp_s1) + 0x916)) = sp40;
    (*(u8 *)((u8 *)(temp_s1) + 0x34C)) = Get_weapon_id(&sp3A);
    temp_a4 = (*(s16 *)((u8 *)(&sp3A) + 0));
    temp_a3 = (*(s16 *)((u8 *)(&sp3A) + 2));
    (*(s16 *)((u8 *)(temp_s1) + 0x35E)) = temp_a4;
    (*(s16 *)((u8 *)(temp_s1) + 0x360)) = temp_a3;
    (*(s16 *)((u8 *)(temp_s1) + 0x362)) = (s16) (*(s16 *)((u8 *)(&sp3A) + 4));
    (*(u8 *)((u8 *)(temp_s1) + 2)) = (u8) (*(s32 *)((u8 *)&Battle_type + (*(u8 *)((u8 *)(temp_s1) + 0x34C))));
    memcpy(temp_s1 + 0x352, &sp33, 6, temp_a3, temp_a4);
}

