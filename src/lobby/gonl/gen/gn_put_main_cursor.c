/* G online (PC): put_main_cursor, written from Monster Hunter G's code at lobby 0x579840 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_F();
extern u8 pNet;
extern u8 lit_3380;
extern u8 lit_3382;

void put_main_cursor(s32 arg0) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s32 *)(g_frame + 0x10))
#define sp12 (*(s16 *)(g_frame + 0x12))
#define sp16 (*(s16 *)(g_frame + 0x16))
#define sp20 (*(s32 *)(g_frame + 0x20))
#define sp22 (*(s16 *)(g_frame + 0x22))
#define sp26 (*(s16 *)(g_frame + 0x26))

    (*(s64 *)((u8 *)(&sp20) + 0)) = (s64) (*(s64 *)((u8 *)(&lit_3380) + 0));
    (*(f32 *)((u8 *)(&sp20) + 8)) = (f32) (*(f32 *)((u8 *)(&lit_3380) + 8));
    (*(s64 *)((u8 *)(&sp10) + 0)) = (s64) (*(s64 *)((u8 *)(&lit_3382) + 0));
    (*(f32 *)((u8 *)(&sp10) + 8)) = (f32) (*(f32 *)((u8 *)(&lit_3382) + 8));
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        sp22 = (arg0 * 0x16) + 0x8C;
        sp26 = sp22 + 0x16;
        sp12 = sp22 - 2;
        sp16 = sp26 + 2;
        Put_F(&sp10);
        Put_F(&sp20);
    }
}

