/* G online (PC): plaza_setMyComment, written from Monster Hunter G's code at lobby 0x570BE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 User_data;
extern u8 cw;
extern u8 game_w;
extern u8 pNet;
s32 KinshiYogo_chk();                        /* extern */
s32 cnWrap_SoundRequest();          /* extern */
s32 memcpy();      /* extern */
s32 my_comment_input();                          /* extern */
extern s32 *g_lb_636A00;
extern u8 g_lb_63E128;

s32 plaza_setMyComment(s32 arg0) {
    s32 **var_s0;
    s32 temp_a0;
    s32 var_s1;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;

    temp_v1 = (*(void **)&pNet);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0x23));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0x23)) = (u8) (temp_v1_2 + 1);
        memcpy((*(s32 *)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x62) + 0x724, &(*(u8 *)((u8 *)&User_data + 0xC9C)), 0x62);
        var_s1 = 0;
        var_s0 = &g_lb_636A00;
        do {
            memcpy(*var_s0, &g_lb_63E128, 4);
            var_s1 = (var_s1 + 1) & 0xFFFF;
            var_s0 += 4;
        } while (var_s1 < 7);
block_14:
    default:
        return 2;
    case 1:
        temp_a0 = arg0 & 0xFFFF;
        if (temp_a0 & 0x20) {
            (*(u8 *)((u8 *)(temp_v1) + 0x23)) = (u8) (temp_v1_2 + 1);
            cnWrap_SoundRequest(0, temp_v1 + 0x23);
        case 2:
            if (my_comment_input((*(s32 *)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x62) + 0x724) == 1) {
                KinshiYogo_chk((*(s32 *)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x62) + 0x724);
                temp_v1_3 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_3) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x23)) - 1);
            }
            goto block_14;
        }
        if (temp_a0 & 0x40) {
            memcpy(&(*(u8 *)((u8 *)&User_data + 0xC9C)), (*(s32 *)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x62) + 0x724, 0x62);
            return 3;
        }
        goto block_14;
    }
}

