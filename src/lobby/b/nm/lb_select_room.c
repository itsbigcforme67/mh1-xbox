#include "lobby_a.h"
extern char lb_board_exp[];
extern char lit_428_0065E270[];
s32 lb_select_room(void) {
    s32 temp_s0;
    s32 temp_v1_2;
    u32 temp_a0;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 temp_v1;
    int temp_s1;
    int temp_v0;
    int temp_v1_3;

    temp_s1 = Lbs_GetRoomInfo(F(u8, pNet, 7));
    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_v0 = (int)pNet;
    temp_v0_2 = F(u8, temp_v0, 2);
    if (temp_v0_2 != 0) {

    } else {
        F(u8, temp_v0, 2) = (u8) (temp_v0_2 + 1);
        temp_a0 = (u32) (F(s32, temp_s1, 0x158) & 0x1FE) >> 1;
        F(s8, pNet, 0x12) = (s8) temp_a0;
        if (F(u8, temp_s1, 0x10) != 3) {
            F(s8, pNet, 0xE) = 1;
        } else {
            F(s8, pNet, 0xE) = 0;
        }
        lb_select_set_data(temp_a0 & 0xFF);
    }
    Lbs_GetClassAdd();
    temp_v1 = F(u8, temp_s1, 0x10);
    switch (temp_v1) {                              /* irregular */
    case 4:
        Lb_put_set01(7);
block_12:
        cnWrap_SoundRequest(3);
        return 3;
    default:
        Lb_put_set01(6);
        goto block_12;
    case 3:
        temp_v1_2 = temp_s0 & 0xFFFF;
        if (temp_v1_2 & 0x20) {
            F(u8, &lb_sys, 0x73) = (u8) F(u8, pNet, 7);
            Lbs_GetRoomInfo(F(u8, &lb_sys, 0x73));
            cnWrap_SoundRequest(0);
            return 0;
        }
        if (temp_v1_2 & 0x40) {
            F(s8, &lb_sys, 6) = 0xE;
            cnWrap_SoundRequest(3);
            return 3;
        }
        if (temp_v1_2 & 0x200) {
            temp_v1_3 = (int)pNet;
            temp_v0_3 = F(u8, temp_v1_3, 8) + 1;
            F(u8, temp_v1_3, 8) = temp_v0_3;
            if ((temp_v0_3 & 0xFF) >= 3) {
                F(u8, pNet, 8) = 0U;
            }
            cnWrap_SoundRequest(6);
        }
        sprintf((int)&lb_board_exp + 0x130, &lit_428_0065E270, ((int *)&lb_num_str)[F(u16, temp_s1, 2)], F(s32, &lb_num_str, 0x2C));
        return 2;
    }
}
