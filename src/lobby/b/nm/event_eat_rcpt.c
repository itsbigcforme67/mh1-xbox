#include "lobby_f.h"
extern s8 eatResult;
extern u8 * pRes;
extern char eat_data_type[];
extern char eat_data_type[];
extern char eat_result[];
extern char Status_add_tbl[];
extern char Status_add_tbl[];
extern char Status_add_tbl[];
extern char Status_add_tbl[];
extern char Status_add_tbl[];
typedef struct { u8 pad0000[0x2]; u8 x0002; u8 pad0003[0x3]; u8 x0006; u8 pad0007[0x1]; u8 x0008; u8 x0009; u8 x000A; } ARG_event_eat_rcpt_arg0;
s32 event_eat_rcpt(ARG_event_eat_rcpt_arg0 *arg0) {
    s32 temp_a2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a2;
    s32 var_v0_3;
    int temp_s0;
    s8 temp_a0_2;
    s8 temp_v1_5;
    u16 var_v0_4;
    u8 temp_a0;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 temp_v0_4;
    u8 temp_v1_6;
    u8 var_v0;
    u8 var_v0_2;
    void *temp_t0;

    temp_s0 =  ((game_w.stage - 0x51) << 0x38) >> 0x38;
    temp_a0 = arg0->x0002;
    temp_v1 = Get_sw2(0) & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        temp_v1_2 = temp_v1 & 0xFFFF;
        if (temp_v1_2 & 0x20) {
            arg0->x0002 = (u8) (temp_a0 + 1);
            arg0->x0009 = (u8) arg0->x0008;
            cnWrap_SoundRequest(0);
            break;
        }
        if (temp_v1_2 & 0x40) {
            cnWrap_SoundRequest(3);
            return 1;
        }
        if (temp_v1_2 & 0x2000) {
            temp_v0 = arg0->x0008;
            if (temp_v0 == 0) {
                var_v0 = 9;
            } else {
                var_v0 = temp_v0 - 1;
            }
            arg0->x0008 = var_v0;
            cnWrap_SoundRequest(1);
        } else if (temp_v1_2 & 0x1000) {
            temp_v0_2 = arg0->x0008 + 1;
            arg0->x0008 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) >= 0xA) {
                arg0->x0008 = 0U;
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 1:
        temp_v1_3 = temp_v1 & 0xFFFF;
        if (temp_v1_3 & 0x20) {
            if (arg0->x0009 == arg0->x0008) {
                cnWrap_SoundRequest(7);
            } else {
                arg0->x0002 = (u8) (temp_a0 + 1);
                cnWrap_SoundRequest(0);
            }
        } else if (temp_v1_3 & 0x40) {
            arg0->x0002 = (u8) (temp_a0 - 1);
            cnWrap_SoundRequest(3);
        } else if (temp_v1_3 & 0x2000) {
            temp_v0_3 = arg0->x0009;
            if (temp_v0_3 == 0) {
                var_v0_2 = 9;
            } else {
                var_v0_2 = temp_v0_3 - 1;
            }
            arg0->x0009 = var_v0_2;
            cnWrap_SoundRequest(1);
        } else if (temp_v1_3 & 0x1000) {
            temp_v0_4 = arg0->x0009 + 1;
            arg0->x0009 = temp_v0_4;
            if ((temp_v0_4 & 0xFF) >= 0xA) {
                arg0->x0009 = 0U;
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 2:
        temp_v1_4 = temp_v1 & 0xFFFF;
        if (temp_v1_4 & 0x20) {
            if (arg0->x000A == 1) {
                cnWrap_SoundRequest(3);
                return 1;
            }
            cnWrap_SoundRequest(8);
            temp_a0_2 = *((u8 *)&eat_data_type + arg0->x0009);
            temp_v1_5 = *((u8 *)&eat_data_type + arg0->x0008);
            if (temp_v1_5 < temp_a0_2) {
                var_v0_3 = (temp_v1_5 << 8) | temp_a0_2;
            } else {
                var_v0_3 = (temp_a0_2 << 8) | temp_v1_5;
            }
            pRes = (u8 *) *((u8 *)&eat_result + (((s8)temp_s0) * 4));
            var_v0_4 = F(u16, pRes, 0);
            var_a2 = 0;
            if (var_v0_4 != 0xFF) {
loop_44:
                if (var_v0_4 == (var_v0_3 & 0xFFFF & 0xFFFF & 0xFFFF)) {
                    arg0->x0006 = (u8) F(u16, pRes, 2);
                } else {
                    var_a2 = (var_a2 + 1) & 0xFFFF;
                    if (var_a2 >= 0x33) {
                        arg0->x0006 = 0U;
                    } else {
                        pRes = (u8 *) (pRes + 8);
                        var_v0_4 = F(u16, pRes, 0);
                        if (var_v0_4 == 0xFF) {

                        } else {
                            goto loop_44;
                        }
                    }
                }
            }
            temp_v1_6 = arg0->x0006;
            if (temp_v1_6 != 0) {
                if (temp_v1_6 == 0xFF) {
                    goto block_55;
                }
                temp_a2 = F(u16, pRes, 2) * 8;
                if (!(((f32) *((u8 *)&Status_add_tbl + 6 + temp_a2) + ((f32) *((u8 *)&Status_add_tbl + 4 + temp_a2) + (f32) (*((u8 *)&Status_add_tbl + temp_a2) + *((u8 *)&Status_add_tbl + 2 + temp_a2)))) <= 0.0f)) {
                    eatResult = 2;
                } else {
                    eatResult = 0;
                }
            } else {
block_55:
                eatResult = 1;
            }
            temp_t0 = pRes;
            *(s8 *)0x3F3603 = (s8) *((u8 *)&Status_add_tbl + 4 + (F(u16, temp_t0, 2) * 8));
            *(s8 *)0x3F3604 = (s8) *((u8 *)&Status_add_tbl + 6 + (F(u16, temp_t0, 2) * 8));
            *(s8 *)0x3F3605 = (s8) *((u8 *)&Status_add_tbl + (F(u16, temp_t0, 2) * 8));
            *(s16 *)0x3F3606 = *((u8 *)&Status_add_tbl + 2 + (F(u16, temp_t0, 2) * 8));
            return 0;
        }
        if (temp_v1_4 & 0x40) {
            arg0->x0002 = (u8) (temp_a0 - 1);
            cnWrap_SoundRequest(3);
        } else if (temp_v1_4 & 0x3000) {
            arg0->x000A = (u8) (arg0->x000A ^ 1);
            cnWrap_SoundRequest(1);
        }
        break;
    }
    return 2;
}
