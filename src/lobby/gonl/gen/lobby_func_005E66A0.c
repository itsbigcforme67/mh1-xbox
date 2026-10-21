/* G online (PC): lobby_func_005E66A0, written from Monster Hunter G's code at lobby 0x5E66A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetSendCommand();       /* extern */
s32 SetSendCommandLen();               /* extern */
s32 SetSendData16();              /* extern */
s32 SetSendData8();                /* extern */
s32 Write_Socket();                    /* extern */
extern u8 send_work;

s32 lobby_func_005E66A0(void *arg0) {
    u8 sp51;
    union { u8 v; u8 b[0x30]; } u_sp50;
#define sp50 (u_sp50.v)
    s16 temp_v0;
    s32 temp_s3;
    s32 var_a1;
    s32 var_s2;
    u8 *var_a2;
    u8 *var_s1;
    u8 temp_s0;
    u8 *var_a0;

    var_a0 = arg0;
    var_a1 = 9;
    var_a2 = &sp50;
    do {
        var_a1 -= 1;
        temp_v0 = (*(s16 *)((u8 *)(var_a0) + 2));
        (*(s16 *)((u8 *)(var_a2) + 0)) = (s16) (*(s16 *)((u8 *)(var_a0) + 0));
        var_a0 += 4;
        (*(s16 *)((u8 *)(var_a2) + 2)) = temp_v0;
        var_a2 += 4;
    } while (var_a1 > 0);
    temp_s3 = SetSendCommand(&send_work, 0x112, var_a2) & 0xFFFF;
    SetSendData8(&send_work, sp50);
    SetSendData16(&send_work, sp52);
    SetSendData8(&send_work, sp51);
    temp_s0 = sp51;
    var_s2 = 0;
    if ((s32) temp_s0 > 0) {
        var_s1 = &sp50;
        do {
            SetSendData8(&send_work, (*(u8 *)((u8 *)(var_s1) + 4)));
            SetSendData8(&send_work, (*(u8 *)((u8 *)(var_s1) + 5)));
            SetSendData8(&send_work, (*(u8 *)((u8 *)(var_s1) + 6)));
            var_s2 += 1;
            var_s1 += 4;
        } while (var_s2 < (s32) temp_s0);
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return temp_s3;
}

