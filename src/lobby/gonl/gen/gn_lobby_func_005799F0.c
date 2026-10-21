/* G online (PC): lobby_func_005799F0, written from Monster Hunter G's code at lobby 0x5799F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_F();
extern u8 pNet;
void lobby_func_005799F0(s64 arg0, s64 arg1, s32 arg2, s32 arg3, s32 arg4) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s16 *)(g_frame + 0x10))
#define sp12 (*(s16 *)(g_frame + 0x12))
#define sp14 (*(s16 *)(g_frame + 0x14))
#define sp16 (*(s16 *)(g_frame + 0x16))
#define sp18 (*(s32 *)(g_frame + 0x18))
#define sp20 (*(s16 *)(g_frame + 0x20))
#define sp22 (*(s16 *)(g_frame + 0x22))
#define sp24 (*(s16 *)(g_frame + 0x24))
#define sp26 (*(s16 *)(g_frame + 0x26))
#define sp28 (*(s32 *)(g_frame + 0x28))
    u8 *var_a0;   /* (s16 *) */
    u8 *var_a5;   /* (s16 *) */
    s32 var_v1;
    s32 var_v1_2;
    s64 temp_a3;

    var_v1 = 0xC;
    var_a5 = &sp20;
    if (var_a5 != NULL) {
        do {
            (*(s16 *)var_a5) = 0;
            var_v1 -= 1;
            var_a5 += 1;
        } while (var_v1 != 0);
    }
    sp28 = arg3;
    temp_a3 = ((s16) (arg0));
    sp20 = temp_a3 + 4;
    sp24 = temp_a3 + 0x190;
    var_a0 = &sp10;
    var_v1_2 = 0xC;
    if (var_a0 != NULL) {
        do {
            (*(s16 *)var_a0) = 0;
            var_v1_2 -= 1;
            var_a0 += 1;
        } while (var_v1_2 != 0);
    }
    sp18 = arg4;
    sp10 = temp_a3 + 1;
    sp14 = temp_a3 + 0x193;
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        sp22 = (((s16) (arg1))) + 0x3C + (arg2 * 0x16);
        sp26 = sp22 + 0x16;
        sp12 = sp22 - 2;
        sp16 = sp26 + 2;
        Put_F(&sp10);
        Put_F(&sp20);
    }
}

