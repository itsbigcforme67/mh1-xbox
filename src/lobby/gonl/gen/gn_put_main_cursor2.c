/* G online (PC): put_main_cursor2, written from Monster Hunter G's code at lobby 0x579900 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
s32 Put_F(); /* extern */
extern u8 lit_3397;
extern u8 lit_3399;

void put_main_cursor2(s64 arg0, s64 arg1, s32 arg2) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s16 *)(g_frame + 0x10))
#define sp12 (*(s16 *)(g_frame + 0x12))
#define sp14 (*(s16 *)(g_frame + 0x14))
#define sp16 (*(s16 *)(g_frame + 0x16))
#define sp20 (*(s16 *)(g_frame + 0x20))
#define sp22 (*(s16 *)(g_frame + 0x22))
#define sp24 (*(s16 *)(g_frame + 0x24))
#define sp26 (*(s16 *)(g_frame + 0x26))
    s16 temp_a6;
    s64 temp_v1;

    (*(s64 *)((u8 *)(&sp20) + 0)) = (s64) (*(s64 *)((u8 *)(&lit_3397) + 0));
    temp_v1 = ((s16) (arg0));
    temp_a6 = temp_v1 + 4;
    (*(f32 *)((u8 *)(&sp20) + 8)) = (f32) (*(f32 *)((u8 *)(&lit_3397) + 8));
    sp20 = temp_a6;
    sp24 = temp_v1 + 0x190;
    (*(s64 *)((u8 *)(&sp10) + 0)) = (s64) (*(s64 *)((u8 *)(&lit_3399) + 0));
    (*(f32 *)((u8 *)(&sp10) + 8)) = (f32) (*(f32 *)((u8 *)(&lit_3399) + 8));
    sp14 = temp_v1 + 0x193;
    sp10 = temp_v1 + 1;
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        sp22 = (((s16) (arg1))) + 0x3C + (arg2 * 0x16);
        sp26 = sp22 + 0x16;
        sp12 = sp22 - 2;
        sp16 = sp26 + 2;
        Put_F(&sp10, &sp26, &sp22, (*(s64 *)((u8 *)(&lit_3399) + 0)), &lit_3399, temp_a6, (*(s64 *)((u8 *)(&lit_3397) + 0)));
        Put_F(&sp20);
    }
}

