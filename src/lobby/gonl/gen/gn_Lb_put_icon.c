/* G online (PC): Lb_put_icon, written from Monster Hunter G's code at lobby 0x592F70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flps0008();
extern u16 lb_icon_tbl;

void Lb_put_icon(s32 arg0, s16 arg1, u32 arg2, s32 arg3) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s16 *)(g_frame + 0x10))
#define sp12 (*(s16 *)(g_frame + 0x12))
#define sp14 (*(s16 *)(g_frame + 0x14))
#define sp16 (*(s16 *)(g_frame + 0x16))
#define sp18 (*(s32 *)(g_frame + 0x18))
#define sp1C (*(s32 *)(g_frame + 0x1C))
#define sp20 (*(s32 *)(g_frame + 0x20))
    s16 temp_a0;
    u8 *temp_a4;

    if (arg2 != 0x30) {
        temp_a4 = (u8 *)&lb_icon_tbl + (arg2 * 8);
        sp10 = (s16) (s32) (0.8f * (f32) arg0);
        sp12 = arg1;
        sp14 = (s16) (s32) (0.8f * (f32) ((*(s16 *)((u8 *)(temp_a4) + 4)) - (*(s16 *)((u8 *)(temp_a4) + 0))));
        temp_a0 = sp14;
        if (temp_a0 < 0) {
            sp14 = -temp_a0;
        }
        sp16 = (*(s16 *)((u8 *)(temp_a4) + 6)) - (*(s16 *)((u8 *)(temp_a4) + 2));
        switch (arg2) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            sp14 = 0x14;
            sp16 = 0x14;
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            sp10 -= 4;
            sp12 -= 2;
            sp14 = 0x14;
            sp16 = 0x16;
            break;
        case 12:
            sp10 -= 4;
            sp12 -= 4;
            sp14 = 0x18;
            sp16 = 0x18;
            break;
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
            sp14 = 0x18;
            sp16 = 0x1C;
            break;
        }
        sp18 = arg3;
        (*(s16 *)((u8 *)(&sp1C) + 0)) = (s16) (*(s16 *)((u8 *)(temp_a4) + 0));
        (*(s16 *)((u8 *)(&sp1C) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a4) + 2));
        (*(s16 *)((u8 *)(&sp20) + 0)) = (s16) (*(s16 *)((u8 *)(temp_a4) + 4));
        (*(s16 *)((u8 *)(&sp20) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a4) + 6));
        flps0008(&sp10);
    }
}

