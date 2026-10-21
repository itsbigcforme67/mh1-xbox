/* G online (PC): SetDialogData, written from Monster Hunter G's code at lobby 0x59F430 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 set_dialog_square();                /* extern */
s8 *strchr();                          /* extern */
s32 strlen_sp();                                /* extern */
extern u8 netDialogMessage;
extern u8 g_lb_636F24;
extern u8 dialogData;

void SetDialogData(s32 arg0) {
    s32 temp_a0;
    s32 var_s0;
    s32 var_v0;
    s8 *temp_v0;
    s8 *var_s2;
    s8 *var_s3;
    s8 var_s1;
    u16 temp_v1;

    dialogData = arg0;
    temp_a0 = arg0 * 8;
    (*(u8 *)((u8 *)&dialogData + 0xC)) = (*(s32 *)((u8 *)&g_lb_636F24 + temp_a0));
    temp_v1 = (*(s32 *)((u8 *)&netDialogMessage + temp_a0));
    var_s0 = 0;
    if (temp_v1 & 0x80) {
        (*(u8 *)((u8 *)&dialogData + 0x14)) = 0;
    } else {
        (*(u8 *)((u8 *)&dialogData + 0x14)) = 1;
    }
    (*(u8 *)((u8 *)&dialogData + 0x4)) = 0;
    (*(u8 *)((u8 *)&dialogData + 0x6)) = 0;
    var_s1 = 0;
    (*(u8 *)((u8 *)&dialogData + 0x12)) = temp_v1 & ~0x80;
    (*(u8 *)((u8 *)&dialogData + 0x15)) = 1;
    var_s3 = (*(u8 *)((u8 *)&dialogData + 0xC));
    var_s2 = var_s3;
    if (*var_s3 != 0) {
loop_6:
        temp_v0 = strchr(var_s3, 0xA);
        var_s1 += 1;
        if (temp_v0 != NULL) {
            var_v0 = temp_v0 - var_s2;
            var_s2 = temp_v0;
        } else {
            var_v0 = strlen_sp(var_s2);
        }
        if (var_v0 < 0) {
            var_v0 = -var_v0;
        }
        if (var_s0 < var_v0) {
            var_s0 = var_v0;
        }
        if (temp_v0 != NULL) {
            var_s3 = temp_v0 + 1;
            if ((*(s8 *)((u8 *)(temp_v0) + 1)) != 0) {
                goto loop_6;
            }
        }
    }
    switch ((*(u8 *)((u8 *)&dialogData + 0x12))) {                           /* irregular */
    case 5:
    case 3:
        (*(u8 *)((u8 *)&dialogData + 0x13)) = 0;
        /* fallthrough */
    case 4:
        var_s1 += 2;
        break;
    }
    (*(u8 *)((u8 *)&dialogData + 0x10)) = var_s1;
    set_dialog_square((var_s0 * 0xA) + 0x3C, (var_s1 * 0x14) + 0x3C);
}

