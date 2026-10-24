/* G online (PC): Lb_Pl_adj_calc, written from Monster Hunter G's code at lobby 0x567B20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

f32 flvecCalcDistance();                    /* extern */

void Lb_Pl_adj_calc(void *arg0, s16 arg1) {
    f32 temp_f2;
    s32 temp_v1;

    if (!(flvecCalcDistance(arg0 + 0xAC, arg0 + 0x800) < 800.0f)) {
        (*(s16 *)((u8 *)(arg0) + 0x818)) = 0;
        (*(f32 *)((u8 *)(arg0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(arg0) + 0x800));
        (*(f32 *)((u8 *)(arg0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(arg0) + 0x804));
        (*(f32 *)((u8 *)(arg0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(arg0) + 0x808));
        return;
    }
    (*(s16 *)((u8 *)(arg0) + 0x818)) = arg1;
    temp_v1 = ((s16) ((s32) arg1));
    temp_f2 = (f32) arg1;
    (*(f32 *)((u8 *)(arg0) + 0x80C)) = (f32) (((*(f32 *)((u8 *)(arg0) + 0x800)) - (*(f32 *)((u8 *)(arg0) + 0xAC))) / temp_f2);
    (*(f32 *)((u8 *)(arg0) + 0x810)) = (f32) (((*(f32 *)((u8 *)(arg0) + 0x804)) - (*(f32 *)((u8 *)(arg0) + 0xB0))) / temp_f2);
    (*(f32 *)((u8 *)(arg0) + 0x814)) = (f32) (((*(f32 *)((u8 *)(arg0) + 0x808)) - (*(f32 *)((u8 *)(arg0) + 0xB4))) / temp_f2);
    (*(s16 *)((u8 *)(arg0) + 0x87C)) = arg1;
    if (temp_v1 == 0) {
        M2C_BREAK(0);
    }
    (*(s16 *)((u8 *)(arg0) + 0x90C)) = (s16) ((s32) (((s16) (((*(u16 *)((u8 *)(arg0) + 0xE)) - (*(s32 *)((u8 *)(arg0) + 0xA4)))))) / temp_v1);
}

