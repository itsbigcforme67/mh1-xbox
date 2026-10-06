#include "lobby_a.h"
extern char Friend_data[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char tl_member_buff[];
extern char tl_member_buff[];
extern char tl_member_buff[];
s32 getFriendNow(int arg0, u8 arg1, int arg2) {
    s16 temp_a3;
    s32 temp_a2;
    s32 temp_a3_2;
    s32 temp_v0;
    int temp_v1_2;
    u8 temp_a0;
    u8 temp_a3_3;
    u8 temp_a3_4;
    u8 temp_v1;
    u8 temp_v1_3;
    int temp_s0;

    temp_a3 = F(s16, arg0, 0x24);
    temp_v1 = F(u8, arg0, 4);
    temp_a3_2 = temp_a3 * 7;
    temp_a2 = temp_a3 * 0x150;
    temp_s0 = (int)&Friend_data + temp_a2;
    switch (temp_v1) {                              /* irregular */
    case 0:
        F(u8, arg0, 4) = (u8) (temp_v1 + 1);
        F(u8, arg0, 6) = arg1;
        /* fallthrough */
    case 1:
        F(u8, arg0, 4) = (u8) (F(u8, arg0, 4) + 1);
        strcpy((int)&SearchCondition + 4, temp_s0 + (F(u8, arg0, 6) * 0x30), temp_a2, temp_a3_2);
        if (F(s8, &SearchCondition, 4) != 0) {
            temp_a0 = F(u8, arg0, 6);
            if ((temp_a0 + (F(s16, arg0, 0x24) * 7)) >= 0x32) {
                goto block_8;
            }
            F(s8, &SearchCondition, 1) = strlen(temp_s0 + (temp_a0 * 0x30));
            F(s8, &SearchCondition, 0) = 1;
        }
block_8:
        F(u8, arg0, 4) = 0U;
        return 0;
    case 2:
        temp_v0 = Lbc_ConditionSearch(&SearchCondition, 1);
        if ((temp_v0 != 1) && (temp_v0 != 0)) {
            break;
        }
        temp_v1_2 = (int)SearchResult;
        if ((*(u8 *)temp_v1_2) != 0) {
            memcpy(&tl_member_buff[F(u8, arg0, 6) * 0x2FC] + 0x280, temp_v1_2 + 4, 8);
            temp_a3_3 = F(u8, arg0, 6);
            memcpy(&tl_member_buff[temp_a3_3 * 0x2FC] + 0x288, (int)SearchResult + 0xC, 0x11);
            temp_a3_4 = F(u8, arg0, 6);
            memcpy((int)&tl_member_buff + (temp_a3_4 * 0x2FC) + 0x29A, (int)SearchResult + 0x20, 0x40);
        }
        temp_v1_3 = F(u8, arg0, 6) + 1;
        F(u8, arg0, 6) = temp_v1_3;
        if ((temp_v1_3 & 0xFF) >= ((s8)arg2)) {
            F(u8, arg0, 4) = 0U;
            return 0;
        }
        F(u8, arg0, 4) = (u8) (F(u8, arg0, 4) - 1);
        break;
    }
    return 2;
}
