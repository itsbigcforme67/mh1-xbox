/* G online (PC): Lb_set_mini_data, written from Monster Hunter G's code at lobby 0x58B8C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EF2A8;
extern u8 User_data;
extern u8 g_main_2EF2A8;
extern u8 game_w;
u8 Get_weapon_job();       /* extern */
s32 memcpy();           /* extern */
extern u8 player_work;

void Lb_set_mini_data(s32 *arg0) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(u8 *)(g_frame + 0x30))
#define sp31 (*(u8 *)(g_frame + 0x31))
#define sp32 (*(u8 *)(g_frame + 0x32))
#define sp33 (*(s32 *)(g_frame + 0x33))
#define sp3A (*(s32 *)(g_frame + 0x3A))
#define sp40 (*(u8 *)(g_frame + 0x40))
#define sp44 (*(s32 *)(g_frame + 0x44))
#define sp48 (*(u8 *)(g_frame + 0x48))
#define sp49 (*(u8 *)(g_frame + 0x49))
#define sp4A (*(u8 *)(g_frame + 0x4A))
    u8 var_v0;
    u8 *temp_s0;

    temp_s0 = (u8 *)&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00);
    sp44 = (*(s32 *)((u8 *)(temp_s0) + 0x5FC));
    memcpy(&sp33, temp_s0 + 0x352, 6);
    sp49 = (*(u8 *)((u8 *)(temp_s0) + 0x11));
    sp32 = (*(u8 *)&g_main_2EF2A8);
    sp40 = (*(u8 *)((u8 *)(temp_s0) + 0x916));
    sp31 = (*(u8 *)((u8 *)&User_data + 0xC23));
    (*(s16 *)((u8 *)(&sp3A) + 0)) = (s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&User_data + 0xC74))) + 0));
    (*(s16 *)((u8 *)(&sp3A) + 2)) = (s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&User_data + 0xC74))) + 2));
    (*(s16 *)((u8 *)(&sp3A) + 4)) = (s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&User_data + 0xC74))) + 4));
    sp30 = Get_weapon_job(&(*(u8 *)((u8 *)&User_data + 0xC74)), (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&User_data + 0xC74))) + 0)), &sp3A);
    if (sp30 == 5) {
        var_v0 = 1;
        goto block_3;
    }
    var_v0 = 4;
    if (sp30 == 6) {
block_3:
        sp30 = var_v0;
    }
    sp48 = (*(u8 *)((u8 *)&User_data + 0x3));
    sp4A = (*(u8 *)((u8 *)&User_data + 0xC7F));
    memcpy(arg0, &sp30, 0x1C);
}

