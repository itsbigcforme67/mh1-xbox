/* G online (PC): lobby_func_005799F0, written from Monster Hunter G's code at lobby 0x5799F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
s32 Put_F();                 /* extern */

void lobby_func_005799F0(s64 arg0, s64 arg1, s32 arg2, s32 arg3) {
    s32 sp28;
    union { s16 v; u8 b[0xA]; } u_sp26;
#define sp26 (u_sp26.v)
    s16 sp24;
    union { s16 v; u8 b[0xE]; } u_sp22;
#define sp22 (u_sp22.v)
    union { s16 v; u8 b[0x10]; } u_sp20;
#define sp20 (u_sp20.v)
    s32 sp18;
    s16 sp16;
    s16 sp14;
    s16 sp12;
    union { s16 v; u8 b[0x20]; } u_sp10;
#define sp10 (u_sp10.v)
    s16 *var_a0;
    s16 *var_t1;
    s32 var_v1;
    s32 var_v1_2;
    s64 temp_a3;

    var_v1 = 0xC;
    var_t1 = &sp20;
    if (var_t1 != NULL) {
        do {
            *var_t1 = 0;
            var_v1 -= 1;
            var_t1 += 1;
        } while (var_v1 != 0);
    }
    sp28 = arg3;
    temp_a3 = (s64) (arg0 << 0x30) >> 0x30;
    sp20 = temp_a3 + 4;
    sp24 = temp_a3 + 0x190;
    var_a0 = &sp10;
    var_v1_2 = 0xC;
    if (var_a0 != NULL) {
        do {
            *var_a0 = 0;
            var_v1_2 -= 1;
            var_a0 += 1;
        } while (var_v1_2 != 0);
    }
    sp18 = M2C_ERROR(/* Read from unset register $t0 */);
    sp10 = temp_a3 + 1;
    sp14 = temp_a3 + 0x193;
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        sp22 = ((s64) (arg1 << 0x30) >> 0x30) + 0x3C + (arg2 * 0x16);
        sp26 = sp22 + 0x16;
        sp12 = sp22 - 2;
        sp16 = sp26 + 2;
        Put_F(&sp10, &sp26, &sp22);
        Put_F(&sp20);
    }
}

