/* G online (PC): Lb_check_receipt, written from Monster Hunter G's code at lobby 0x558B80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_pl_to_normal();
s32 lb_check_chair();
s32 lb_commer_message();
s32 lb_send_my_status();
s32 lb_set_chair();
s32 lb_set_pl_pos();
s32 lb_set_pl_stage();
s32 lb_set_pl_status();
void * memcpy();
extern u8 cw;
extern u8 jtbl_218_0065ECA0;
extern u8 lb_sys;
s32 Lb_get_plID();                                  /* extern */
s32 Lbc_SendNetComment();         /* extern */
s32 lb_chidori_off();             /* extern */
s32 lb_recv_myChair();         /* extern */
s32 lb_trade_check();                /* extern */
s32 lb_trade_result();         /* extern */
s32 lb_trade_start();                /* extern */
extern u8 player_work;

void Lb_check_receipt(s32 arg0, void *arg1) {
    s32 temp_a1;
    s32 temp_s2;
    s32 var_a0;
    u32 temp_v1;
    u8 temp_s0;
    u8 *temp_a0;
    u8 *temp_a3;
    u8 *temp_s1;

    var_a0 = arg0;
    temp_s2 = var_a0;
    temp_s0 = (*(u8 *)((u8 *)(arg1) + 0));
    temp_s1 = arg1 + 1;
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
        var_a0 = Lb_get_plID() & 0xFF;
        if (var_a0 == 0xFF) {
            return;
        }
        goto block_8;
    }
    temp_a1 = temp_s0 & 0xFF;
    if ((temp_a1 != 8) && (temp_a1 != 7) && (temp_a1 != 0xB)) {
        return;
    }
block_8:
    temp_v1 = temp_s0 & 0xFF;
    switch (temp_v1) {
    case 0:
        lb_set_pl_pos(var_a0, temp_s1, (s32) temp_s0);
        return;
    case 1:
    case 10:
        lb_set_pl_status(var_a0, temp_s1);
        return;
    case 2:
        lb_set_pl_pos(var_a0, temp_s1, (s32) temp_s0);
        return;
    case 3:
        lb_set_pl_stage(var_a0, temp_s1);
        return;
    case 4:
        lb_check_chair(temp_s2, temp_s1);
        return;
    case 9:
        (*(u16 *)((u8 *)&lb_sys + 0x14)) &= ~(1 << (*(s8 *)((u8 *)(arg1) + 1)));
        return;
    case 20:
        temp_a0 = (u8 *)&player_work + ((var_a0 & 0xFF) * 0xA00);
        if (((*(u8 *)((u8 *)(temp_a0) + 0x14)) == 0) && ((*(u8 *)((u8 *)(temp_a0) + 0x15)) == 0x2E)) {
            Lb_pl_to_normal(temp_a0, 0, 2, 0);
            return;
        }
    default:
        return;
    case 5:
        lb_set_chair((s32) (((s8) ((s64) var_a0))), temp_s1);
        return;
    case 17:
        lb_recv_myChair(temp_s1, &jtbl_218_0065ECA0);
        return;
    case 6:
        lb_chidori_off(((s8) ((s64) var_a0)), &jtbl_218_0065ECA0);
        return;
    case 7:
        Lbc_SendNetComment(temp_s2, &jtbl_218_0065ECA0);
        return;
    case 8:
        temp_a3 = (*(void **)&cw);
        if ((*(u8 *)((u8 *)(temp_a3) + 0xFA61)) != 0) {
            memcpy(temp_a3 + ((var_a0 & 0xFF) * 0x62) + 0x724, temp_s1, 0x62);
            memcpy((*(void **)&cw) + 0xAF8, temp_s1, 0x62);
            return;
        }
        memcpy(temp_a3 + 0x724, temp_s1, 0x62);
        memcpy((*(void **)&cw) + 0xAF8, temp_s1, 0x62);
        return;
    case 12:
        lb_trade_start(((s8) ((s64) var_a0)), temp_s1);
        return;
    case 13:
        lb_trade_check(temp_s2, temp_s1);
        return;
    case 14:
        lb_trade_result(temp_s1, &jtbl_218_0065ECA0);
        return;
    case 15:
        lb_send_my_status(temp_s2);
        return;
    case 16:
        lb_commer_message(((s8) ((s64) var_a0)));
        break;
    }
}

