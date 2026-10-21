/* G online (PC): BsToolMenuSprTrans, written from Monster Hunter G's code at lobby 0x5B2A50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 wpushCtr;
s32 BsStrtblGet();                  /* extern */
s32 BsWorkPush();                               /* extern */
s32 drawBlank();                                /* extern */
s32 drawDialogMenu(f32, f32);                   /* extern */
s32 drawRect(f32, f32, s32);                /* extern */
s32 drawString(f32, s32, s32, s32, s32); /* extern */
s32 flfntStackReset();                          /* extern */

/* Warning: Gap in callee-saved word stack region.
 * Saved: [0x0, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x40, 0x44], gap at: 0x8. */
void BsToolMenuSprTrans(void *arg0) {
    f32 temp_f12;
    f32 temp_f20;
    f32 temp_f5;
    s32 var_s1;
    s32 var_s2;
    s8 temp_v1;

    temp_v1 = (*(s8 *)((u8 *)(arg0) + 6));
    switch (temp_v1) {                              /* irregular */
    case 0x1:
    case 0x0:
        drawBlank();
        if ((*(s8 *)((u8 *)(arg0) + 6)) == 0) {
            return;
        }
    default:
        (*(f32 *)((u8 *)(arg0) + 0x44)) = 110.0f;
        (*(f32 *)((u8 *)(arg0) + 0x40)) = 200.0f;
        temp_f12 = (*(f32 *)((u8 *)(arg0) + 0x34));
        drawDialogMenu(temp_f12, temp_f12 + (*(f32 *)((u8 *)(arg0) + 0x40)));
        var_s2 = 0;
        var_s1 = 0;
        do {
            temp_f20 = 20.0f + (*(f32 *)((u8 *)(arg0) + 0x38)) + (f32) var_s1;
            BsStrtblGet(3, var_s2 & 0xFF);
            drawString(40.0f + (*(f32 *)((u8 *)(arg0) + 0x34)), 0x14, -1, 0, 0x14);
            var_s2 = (var_s2 + 1) & 0xFF;
            var_s1 += 0x14;
        } while (var_s2 < 3);
        flfntStackReset();
        temp_f5 = (*(f32 *)((u8 *)(arg0) + 0x34));
        drawRect((40.0f + temp_f5) - 2.0f, 2.0f + (160.0f + temp_f5), -1);
        return;
    case 0x63:
        BsWorkPush();
        (*(u8 *)&wpushCtr) = (u8) ((*(u8 *)&wpushCtr) + 1);
        return;
    }
}

