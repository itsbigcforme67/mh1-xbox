/* G online (PC): lb_pl_chat, written from Monster Hunter G's code at lobby 0x59A070 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 system_error();
s32 lb_pl_chat00();                      /* extern */
s32 lb_pl_chat01();                      /* extern */
s32 lb_pl_chat02();                      /* extern */
s32 lb_pl_chat03();                      /* extern */
s32 lb_pl_chat04();                      /* extern */
s32 lb_pl_chat05();                      /* extern */
s32 lb_pl_chat06();                      /* extern */
s32 lb_pl_chat07();                      /* extern */
s32 lb_pl_chat08();                      /* extern */
s32 lb_pl_chat09();                      /* extern */
s32 lb_pl_chat10();                      /* extern */
s32 lb_pl_chat11();                      /* extern */
s32 lb_pl_chat12();                      /* extern */
s32 lb_pl_chat16();                      /* extern */
s32 lobby_func_0059A060();               /* extern */
extern s32 lit_1807_00664E40;

void lb_pl_chat(void *arg0) {
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 0x15));
    switch (temp_a1) {
    case 0:
        lb_pl_chat00(0);
        return;
    case 1:
        lb_pl_chat01(0);
        return;
    case 2:
        lb_pl_chat02(0);
        return;
    case 3:
        lb_pl_chat03(0);
        return;
    case 4:
        lb_pl_chat04(0);
        return;
    case 5:
        lb_pl_chat05(0);
        return;
    case 6:
        lb_pl_chat06(0);
        return;
    case 7:
        lb_pl_chat07(0);
        return;
    case 8:
        lb_pl_chat08(0);
        return;
    case 9:
        lb_pl_chat09(0);
        return;
    case 10:
        lb_pl_chat10(0);
        return;
    case 11:
        lb_pl_chat11(0);
        return;
    case 12:
        lb_pl_chat12(0);
        return;
    case 13:
        lb_pl_chat09(1);
        return;
    case 14:
        lb_pl_chat09(2);
        return;
    case 15:
        lb_pl_chat09(3);
        return;
    case 16:
        lb_pl_chat16(0);
        return;
    case 17:
        lobby_func_0059A060(0);
        return;
    default:
        system_error(&lit_1807_00664E40, (s32) (*(u8 *)((u8 *)(arg0) + 0x14)), temp_a1 & 0xFF, (s32) (*(u8 *)((u8 *)(arg0) + 0x17)));
        return;
    }
}

