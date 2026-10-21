/* HAND-FIXED (memcpy length). G online (PC): lobby_func_0059A210, written from Monster Hunter G's code at lobby 0x59A210 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EF390;
extern u8 g_main_2EFAD4;
extern u8 g_main_2EFAD8;
extern u8 g_main_2EFADC;
extern u8 g_main_2EFAE0;
extern u8 g_main_2EF390;
extern u8 g_main_2EFAD4;
extern u8 g_main_2EFAD8;
extern u8 g_main_2EFADC;
extern u8 g_main_2EFAE0;
s32 memcpy();                  /* extern */
extern s32 g_sub_541E40;

void lobby_func_0059A210(void) {
    u16 temp_v0_2;
    u16 temp_v0_3;
    u16 temp_v0_4;
    u16 temp_v0_5;
    u8 *temp_v0;
    u8 *temp_v1;

    temp_v0 = (*(void **)&g_main_2EF390);
    temp_v0_2 = (*(u16 *)((u8 *)(temp_v0) + 0x2D9C));
    temp_v1 = temp_v0 + 0x23F4 + 0x20;
    if (temp_v0_2 != 0) {
        (*(u16 *)&g_main_2EFAE0) = temp_v0_2;
    }
    temp_v0_3 = (*(u16 *)((u8 *)(temp_v1) + 0x98A));
    if (temp_v0_3 != 0) {
        (*(u16 *)&g_main_2EFAD4) = temp_v0_3;
    }
    temp_v0_4 = (*(u16 *)((u8 *)(temp_v1) + 0x98E));
    if (temp_v0_4 != 0) {
        (*(u16 *)&g_main_2EFAD8) = temp_v0_4;
    }
    temp_v0_5 = (*(u16 *)((u8 *)(temp_v1) + 0x992));
    if (temp_v0_5 != 0) {
        (*(u16 *)&g_main_2EFADC) = temp_v0_5;
    }
    memcpy(&g_sub_541E40, temp_v1 + 0x998, 0xA);   /* HAND-FIXED: the length is set in the tail jump's delay slot */
}

