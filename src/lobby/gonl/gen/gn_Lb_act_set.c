/* G online (PC): Lb_act_set, written from Monster Hunter G's code at lobby 0x568540 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 game_w;
extern u8 lb_sys;
s64 Lb_act_ck();                                    /* extern */
s32 Lb_pl_to_normal(); /* extern */
s32 check_sender0();                      /* extern */
s32 ck_pl_send();                             /* extern */
s32 lb_action_timer_calc();      /* extern */
s16 ran_suu();                               /* extern */

void Lb_act_set(void *arg0, u8 arg1, u8 arg2) {
    if ((*(u8 *)((u8 *)(arg0) + 0x10)) == 0) {
        (*(u8 *)((u8 *)(arg0) + 0x16)) = (u8) (*(u8 *)((u8 *)(arg0) + 0x14));
        (*(u8 *)((u8 *)(arg0) + 0x17)) = (u8) (*(u8 *)((u8 *)(arg0) + 0x15));
    }
    (*(u8 *)((u8 *)(arg0) + 0x14)) = arg1;
    (*(u8 *)((u8 *)(arg0) + 0x15)) = arg2;
    (*(s8 *)((u8 *)(arg0) + 5)) = 0;
    (*(s8 *)((u8 *)(arg0) + 6)) = 0;
    (*(s8 *)((u8 *)(arg0) + 7)) = 0;
    if ((*(u8 *)((u8 *)(arg0) + 0x10)) == 0) {
        (*(s16 *)((u8 *)(arg0) + 0x39A)) = ran_suu(1);
    }
    if ((*(u8 *)((u8 *)(arg0) + 0x10)) != 0) {
        (*(s8 *)((u8 *)(arg0) + 0x720)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x724)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x72C)) = 0;
        (*(s8 *)((u8 *)(arg0) + 0x721)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x726)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x72E)) = 0;
        (*(s8 *)((u8 *)(arg0) + 0x722)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x728)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x730)) = 0;
        (*(s8 *)((u8 *)(arg0) + 0x723)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x72A)) = 0;
        (*(s16 *)((u8 *)(arg0) + 0x732)) = 0;
    }
    if (((*(u8 *)((u8 *)(arg0) + 0x10)) == 0) && ((*(u8 *)((u8 *)(arg0) + 0x14)) == 2)) {
        (*(s8 *)((u8 *)(arg0) + 0x615)) = 0;
    }
    lb_action_timer_calc(arg0, 0);
    (*(s32 *)((u8 *)(arg0) + 0x390)) = (s32) ((*(s32 *)((u8 *)(arg0) + 0x390)) & 0x100100);
    (*(s32 *)((u8 *)(arg0) + 0x394)) = 0;
    (*(s32 *)((u8 *)(arg0) + 0x1A0)) = 0x40000000;
    (*(s32 *)((u8 *)(arg0) + 0x1F0)) = 0x40000000;
    if (((*(u8 *)((u8 *)(arg0) + 0x1E)) == 0) && ((*(u16 *)((u8 *)(arg0) + 0xC)) != (*(u8 *)((u8 *)&game_w + 0x3A)))) {
        if ((((s16) (Lb_act_ck()))) != 0) {
            if ((*(u16 *)((u8 *)(arg0) + 0x2DC)) != 1) {
                Lb_pl_to_normal(arg0, 0, 4, 0);
            }
            goto block_14;
        }
    } else {
block_14:
        if (ck_pl_send(arg0) != 0) {
            if (((*(u8 *)((u8 *)(arg0) + 0x15)) == 0x4D) && ((*(u8 *)((u8 *)&game_w + 0x14)) == 0x4D)) {
                (*(u16 *)((u8 *)&lb_sys + 0x14)) &= ~(1 << *(*(u16 **)((u8 *)(arg0) + 0x878)));
            }
            check_sender0(arg0);
        }
    }
}

