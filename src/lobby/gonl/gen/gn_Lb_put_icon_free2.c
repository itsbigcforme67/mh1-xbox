/* G online (PC): Lb_put_icon_free2, written from Monster Hunter G's code at lobby 0x593380 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flps0008();
extern u16 lb_icon_tbl;

void Lb_put_icon_free2(s32 arg0, s16 arg1, s16 arg2, s32 arg3, s32 arg4) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s16 *)(g_frame + 0x10))
#define sp12 (*(s16 *)(g_frame + 0x12))
#define sp14 (*(s16 *)(g_frame + 0x14))
#define sp16 (*(s16 *)(g_frame + 0x16))
#define sp18 (*(s32 *)(g_frame + 0x18))
#define sp1C (*(s32 *)(g_frame + 0x1C))
#define sp20 (*(s32 *)(g_frame + 0x20))
    s32 temp_a4;
    u8 *temp_a3;

    temp_a4 = ((s16) (arg4));
    if (temp_a4 != 0x30) {
        sp12 = arg1;
        sp18 = arg3;
        temp_a3 = (u8 *)&lb_icon_tbl + (temp_a4 * 8);
        sp14 = arg2;
        sp16 = arg2;
        sp10 = M2C_BITWISE(s16, (0.8f * (f32) arg0));
        (*(s16 *)((u8 *)(&sp1C) + 0)) = (s16) (*(s16 *)((u8 *)(temp_a3) + 0));
        (*(s16 *)((u8 *)(&sp1C) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a3) + 2));
        (*(s16 *)((u8 *)(&sp20) + 0)) = (s16) (*(s16 *)((u8 *)(temp_a3) + 4));
        (*(s16 *)((u8 *)(&sp20) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a3) + 6));
        flps0008(&sp10);
    }
}

