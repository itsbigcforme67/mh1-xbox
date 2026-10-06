#include "lobby_f.h"
extern s8 chatListFlag;
extern char chatLogBuff[];
extern char chatLogBuff[];
extern char chatLogBuff[];
typedef struct { s16 x0000; u8 pad0002[0x1]; u8 x0003; u8 pad0004[0x6]; u8 x000A; u8 pad000B[0x1]; s8 x000C; u8 pad000D[0x5]; u8 x0012; u8 x0013; u8 pad0014[0x10]; s16 x0024; s16 x0026; s16 x0028; } ARG_plaza_setChatMode_arg0;
void plaza_setChatMode(ARG_plaza_setChatMode_arg0 *arg0, int arg1, int arg2) {
    s16 temp_a1;
    s16 temp_v0_4;
    s16 temp_v1;
    s32 temp_a0_2;
    s32 temp_hi;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_v0_6;
    s32 temp_v1_3;
    int temp_v0;
    int temp_v0_2;
    int temp_v0_5;
    u8 temp_a0;
    u8 temp_a1_2;
    u8 temp_v0_3;
    u8 temp_v0_7;
    u8 temp_v1_2;

    temp_a0 = arg0->x0003;
    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        arg0->x0003 = (u8) (temp_a0 + 1);
        arg0->x000A = 0U;
        temp_v0 = Plaza_log_id_chk(&chatLogBuff);
        arg0->x0012 = (u8) temp_v0;
        arg0->x0024 = 0;
        arg0->x0026 = get_page_num( (temp_v0 << 0x30) >> 0x30, 7);
        return;
    case 1:
        temp_v0_2 = Plaza_log_id_chk(&chatLogBuff);
        arg0->x0012 = (u8) temp_v0_2;
        arg0->x0026 = get_page_num( (temp_v0_2 << 0x30) >> 0x30, 7, temp_v0_2);
        temp_v1 = arg0->x0026;
        if (arg0->x0024 >= temp_v1) {
            arg0->x0024 = (s16) (temp_v1 - 1);
            arg0->x000A = 0U;
        } else {
            temp_v1_2 = arg0->x000A;
            if ((arg2 % 7) < temp_v1_2) {
                arg0->x000A = (u8) (temp_v1_2 - 1);
            }
        }
        temp_a0_2 = temp_s0 & 0xFFFF;
        arg0->x0028 = Get_sw_on2(0);
        if (temp_a0_2 & 0x100) {
            cnWrap_SoundRequest(6);
            arg0->x0003 = 3U;
            arg0->x0013 = (u8) arg0->x000A;
            arg0->x000A = 0U;
            chatListFlag = 0;
            arg0->x0000 = 0;
            return;
        }
        if (temp_a0_2 & 0x20) {
            cnWrap_SoundRequest(6);
            temp_v0_3 = arg0->x000A;
            if (temp_v0_3 == 0) {
                Lb_clearChatList();
                return;
            }
            temp_a1 = arg0->x0024;
            temp_s0_2 = *((u8 *)&chatLogBuff + ((temp_v0_3 - 1 + (temp_a1 * 7)) * 4));
            if (Lb_checkChatID(temp_s0_2 + 0x44, temp_a1) == 1) {
                Lb_clearChatID(temp_s0_2 + 0x44);
                return;
            }
            if (Lb_addChatMember(temp_s0_2 + 0x44, temp_s0_2 + 0x4C) == -1) {
                arg0->x0003 = 2U;
                SetDialogData(0x2F, 3);
                return;
            }
        } else {
            if (temp_a0_2 & 0x40) {
                tl_exit_sub_menu(0);
                return;
            }
            if (temp_a0_2 & 0x2000) {
                if (arg0->x0012 == 0) {
                    cnWrap_SoundRequest(7);
                } else if (arg0->x000A == 0) {
                    cnWrap_SoundRequest(1);
                    if (arg0->x0024 == (arg0->x0026 - 1)) {
                        temp_hi = arg0->x0012 % 7;
                        if (temp_hi == 0) {
                            arg0->x000A = 7U;
                        } else {
                            arg0->x000A = (u8) temp_hi;
                        }
                    } else {
                        arg0->x000A = 7U;
                    }
                } else {
                    cnWrap_SoundRequest(1);
                    arg0->x000A = (u8) (arg0->x000A - 1);
                }
                cnWrap_SoundRequest(1);
                return;
            }
            if (temp_a0_2 & 0x1000) {
                if (arg0->x0012 == 0) {
                    cnWrap_SoundRequest(7);
                } else {
                    cnWrap_SoundRequest(1);
                    arg0->x000A = (u8) (arg0->x000A + 1);
                    temp_a1_2 = arg0->x000A;
                    if ((temp_a1_2 >= 8) || ((temp_a1_2 + (arg0->x0024 * 7)) >= (arg0->x0012 + 1))) {
                        arg0->x000A = 0U;
                    }
                }
                cnWrap_SoundRequest(1);
                return;
            }
            if (temp_a0_2 & 0x800) {
                if (arg0->x0026 >= 2) {
                    arg0->x000A = 0U;
                    temp_v0_4 = arg0->x0024 - 1;
                    arg0->x0024 = temp_v0_4;
                    if (((s16)temp_v0_4) < 0) {
                        arg0->x0024 = (s16) (arg0->x0026 - 1);
                    }
                    cnWrap_SoundRequest(1);
                    return;
                }
            } else {
                if ((temp_a0_2 & 0x400) && (arg0->x0026 >= 2)) {
                    arg0->x000A = 0U;
                    temp_v0_5 = arg0->x0024 + 1;
                    arg0->x0024 = (s16) temp_v0_5;
                    if (( (temp_v0_5 << 0x30) >> 0x30) >= arg0->x0026) {
                        arg0->x0024 = 0;
                    }
                    cnWrap_SoundRequest(1);
                    return;
                }
                return;
            }
        }
        break;
    case 2:
        arg0->x000C = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            arg0->x0003 = 1U;
            cnWrap_SoundRequest(0);
            return;
        }
        break;
    case 3:
        temp_v0_6 = lb_chatMemberCheck(temp_a0);
        if ((temp_v0_6 != 3) && (temp_v0_6 != 0)) {
            return;
        }
        arg0->x0003 = (u8) (arg0->x0003 + 1);
        return;
    case 4:
        temp_v1_3 = temp_s0 & 0xFFFF;
        if (temp_v1_3 & 0x40) {
            arg0->x000A = (u8) arg0->x0013;
            arg0->x0003 = 1U;
            cnWrap_SoundRequest(3);
            return;
        }
        if (temp_v1_3 & 0x20) {
            cnWrap_SoundRequest(6);
            temp_v0_7 = arg0->x000A;
            if (temp_v0_7 == 0) {
                Lb_clearChatList();
                return;
            }
            Lb_clearChatMember( ((temp_v0_7 - 1) << 0x38) >> 0x38);
            arg0->x0003 = 3U;
            chatListFlag = 0;
            arg0->x0000 = 0;
            arg0->x000A = (u8) (arg0->x000A - 1);
            return;
        }
        arg0->x000A = Lb_cursorUD(arg0->x000A, F(u8, (u8 *)cw, 0x32BE) + 1);
        break;
    }
}
