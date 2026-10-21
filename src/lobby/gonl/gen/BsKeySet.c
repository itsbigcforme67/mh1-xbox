/* G online (PC): BsKeySet, written from Monster Hunter G's code at lobby 0x5B6FC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 flpad_adr;
s32 _dpfle();                               /* extern */
s32 _dpflt();                           /* extern */
s32 dpdiv();                                /* extern */
s32 dpsub();                                /* extern */
f32 dptofp();                                    /* extern */
s8 dptoli();                                     /* extern */
s32 fptodp(f32);                                    /* extern */
s32 litodp();                                    /* extern */
extern u8 BsPsw;
extern u8 Psw;

/* Warning: Gap in callee-saved word stack region.
 * Saved: [0x0, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c, 0x50, 0x54], gap at: 0x8. */
void BsKeySet(void) {
    s32 *var_s1;
    f32 var_f20;
    s16 temp_v1;
    s32 var_s2;
    u8 *var_s0;

    var_s2 = 0;
    var_s1 = &BsPsw;
    var_s0 = (*(void **)((u8 *)&flpad_adr + 0x4));
    (*(u16 *)((u8 *)(&BsPsw) + 0)) = (u16) (*(u16 *)((u8 *)(&Psw) + 0));
    (*(u16 *)((u8 *)(&BsPsw) + 2)) = (u16) (*(u16 *)((u8 *)(&Psw) + 2));
    (*(u16 *)((u8 *)(&BsPsw) + 4)) = (u16) (*(u16 *)((u8 *)(&Psw) + 4));
    (*(u16 *)((u8 *)(&BsPsw) + 6)) = (u16) (*(u16 *)((u8 *)(&Psw) + 6));
    do {
        temp_v1 = (*(s16 *)((u8 *)(var_s0) + 0x30));
        if (temp_v1 >= 0x7F) {
            (*(u8 *)((u8 *)(var_s1) + 8)) = 3U;
        } else if (temp_v1 >= 0x65) {
            (*(u8 *)((u8 *)(var_s1) + 8)) = 2U;
        } else if (temp_v1 >= 0x47) {
            (*(u8 *)((u8 *)(var_s1) + 8)) = 1U;
        } else {
            (*(u8 *)((u8 *)(var_s1) + 8)) = 0U;
        }
        if ((s32) (*(u8 *)((u8 *)(var_s1) + 8)) > 0) {
            var_f20 = dptofp(dpsub(0x40595000 << 0x20, litodp((*(s16 *)((u8 *)(var_s0) + 0x32)))));
            if (_dpflt(fptodp(var_f20), 0) != 0) {
                var_f20 += 360.0f;
            }
            if (_dpfle(0x40768000 << 0x20, fptodp(var_f20)) != 0) {
                var_f20 -= 360.0f;
            }
            (*(s8 *)((u8 *)(var_s1) + 9)) = dptoli(dpdiv(fptodp(var_f20), 0x40368000 << 0x20));
        } else {
            (*(s8 *)((u8 *)(var_s1) + 9)) = 0;
        }
        var_s2 += 1;
        var_s0 += 0xC;
        var_s1 += 2;
    } while (var_s2 < 2);
}

