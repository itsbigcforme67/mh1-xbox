/* G online (PC): Lb_put_icon_free, written from Monster Hunter G's code at lobby 0x5932B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flps0008();
extern u8 lb_icon_tbl;

void Lb_put_icon_free(s32 arg0, s16 arg1, s16 arg2, s32 arg3, s64 arg4) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s16 *)(g_frame + 0x10))
#define sp12 (*(s16 *)(g_frame + 0x12))
#define sp14 (*(s16 *)(g_frame + 0x14))
#define sp16 (*(s16 *)(g_frame + 0x16))
#define sp18 (*(s32 *)(g_frame + 0x18))
#define sp1C (*(s32 *)(g_frame + 0x1C))
#define sp20 (*(s32 *)(g_frame + 0x20))
    s16 temp_v0;
    s64 temp_a4;
    u8 *temp_a4_2;

    temp_a4 = ((s16) (arg4));
    if (temp_a4 != 0x30) {
        temp_a4_2 = (u8 *)&lb_icon_tbl + (temp_a4 * 8);
        sp12 = arg1;
        sp10 = M2C_BITWISE(s16, (0.8f * (f32) arg0));
        sp14 = M2C_BITWISE(s16, (0.8f * (f32) arg2));
        temp_v0 = sp14;
        if (temp_v0 < 0) {
            sp14 = -temp_v0;
        }
        sp16 = arg2;
        sp18 = arg3;
        (*(s16 *)((u8 *)(&sp1C) + 0)) = (s16) (*(s16 *)((u8 *)(temp_a4_2) + 0));
        (*(s16 *)((u8 *)(&sp1C) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a4_2) + 2));
        (*(s16 *)((u8 *)(&sp20) + 0)) = (s16) (*(s16 *)((u8 *)(temp_a4_2) + 4));
        (*(s16 *)((u8 *)(&sp20) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a4_2) + 6));
        flps0008(&sp10);
    }
}

