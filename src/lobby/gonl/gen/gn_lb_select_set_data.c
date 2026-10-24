/* G online (PC): lb_select_set_data, written from Monster Hunter G's code at lobby 0x58A6C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 KinshiYogo_chk();
void * Lb_get_quest_str();
#ifdef __MWERKS__
s32 Lbs_GetClassAdd();
#else
unsigned short Lbs_GetClassAdd();   /* the real return type (tools/check_kr_returns.py) */
#endif
void * Lbs_GetRoomInfo();
s32 han2zen();
s32 lobby_func_00559C30();
void * strcat();
void * strcpy();
extern u8 lb_board_exp;
extern u8 lb_board_message;
extern u8 lb_quest_exp;
extern u8 pNet;
#ifdef __MWERKS__
s32 Lb_get_quest_type();                         /* extern */
#else
signed char Lb_get_quest_type();                         /* extern */   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_num_to_str();              /* extern */
s32 sprintf(); /* extern */
extern u16 g_lb_63574C;
extern u16 g_lb_635750;
extern s32 g_lb_635D9C;
extern s32 g_lb_635E00;
extern s32 g_lb_635E64;
extern s32 g_lb_63E7D8;
extern s32 D_00528A80;
extern s32 lb_num_str;
extern s32 lb_quest_attribute;
extern u16 lb_quest_color_tex;
extern void *lb_rule_msg_etc;
extern s32 lit_427_0065E260;
extern s32 map_name;

void lb_select_set_data(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp60 (*(s32 *)(g_frame + 0x60))
#define sp80 (*(s32 *)(g_frame + 0x80))
    u8 *temp_a5;   /* (s32 *) */
    s16 temp_a4;
    s32 temp_a1;
    s32 temp_s1;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s8 temp_v0_3;
    u32 temp_v1;
    u8 temp_v1_2;
    void **temp_s0;
    u8 *temp_a0;
    u8 *temp_a3;
    u8 *temp_s2;

    temp_s2 = Lbs_GetRoomInfo((s32) (*(u8 *)((u8 *)((*(u8 **)&pNet)) + 0x28)), arg1, arg2, arg3);
    temp_s1 = Lbs_GetClassAdd(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) & 0xFFFF;
    temp_v0 = lobby_func_00559C30(arg0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    temp_v0_2 = Lb_get_quest_type(temp_v0);
    temp_a1 = temp_v0_2 * 8;
    temp_a0 = (u8 *)&lb_quest_color_tex + temp_a1;
    temp_a4 = (*(s16 *)((u8 *)(temp_a0) + 0));
    temp_a3 = (u8 *)&(*(u16 *)((u8 *)&lb_quest_color_tex + 0x4)) + temp_a1;
    (*(s16 *)((u8 *)(&g_lb_63574C) + 0)) = temp_a4;
    (*(s16 *)((u8 *)(&g_lb_63574C) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a0) + 2));
    (*(s16 *)((u8 *)(&g_lb_635750) + 0)) = (s16) (*(s16 *)((u8 *)(temp_a3) + 0));
    (*(s16 *)((u8 *)(&g_lb_635750) + 2)) = (s16) (*(s16 *)((u8 *)(temp_a3) + 2));
    temp_s0 = (*(void ***)((u8 *)(temp_v0) + 0x18));
    sprintf(&(*(s32 *)((u8 *)&lb_board_exp + 0x4)), (*(s32 *)((u8 *)&lb_quest_attribute + (temp_v0_2 * 4))), &g_lb_635750, temp_a3, temp_a4, &g_lb_63574C);
    if ((arg0 & 0xFFFF) >= 0xC350) {
        strcpy(&(*(s32 *)((u8 *)&lb_board_exp + 0x68)), Lb_get_quest_str((s32) (*(u16 *)((u8 *)(temp_v0) + 0x1E)), 0, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)));
    } else {
        strcpy(&(*(s32 *)((u8 *)&lb_board_exp + 0x68)), *temp_s0);
    }
    temp_a5 = (*(u8 **)&pNet);
    sprintf(&(*(s32 *)((u8 *)&lb_board_exp + 0xCC)), &lit_427_0065E260, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&lb_num_str + 0x4)) + ((*(u8 *)((u8 *)(temp_a5) + 0x28)) * 4))), (*(void * *)((u8 *)&lb_num_str + 0x2C)), (s16) (*(s32 *)((u8 *)&lb_num_str + ((temp_s1 & 0xFFFF) * 4))), temp_a5);
    sprintf(&(*(s32 *)((u8 *)&lb_board_exp + 0x130)), &lit_427_0065E260, (*(s32 *)((u8 *)&lb_num_str + ((*(u16 *)((u8 *)(temp_s2) + 6)) * 4))), (*(void * *)((u8 *)&lb_num_str + 0x2C)), (s16) (*(s32 *)((u8 *)&lb_num_str + ((*(u16 *)((u8 *)(temp_s2) + 8)) * 4))));
    if ((*(u8 *)((u8 *)(temp_s2) + 0x17)) != 0) {
        strcpy(&(*(s32 *)((u8 *)&lb_board_exp + 0x194)), (*(void * *)((u8 *)&lb_rule_msg_etc + 0x8)));
    } else {
        strcpy(&(*(s32 *)((u8 *)&lb_board_exp + 0x194)), (*(void * *)((u8 *)&lb_rule_msg_etc + 0x4)));
    }
    temp_v0_3 = (*(s8 *)((u8 *)(temp_s2) + 0x5B));
    if (temp_v0_3 != 0) {
        if (((temp_v0_3 & 0xFF) == 0x81) && ((*(u8 *)((u8 *)(temp_s2) + 0x5C)) == 0x40) && ((*(u8 *)((u8 *)(temp_s2) + 0x5D)) == 0)) {
            goto block_12;
        }
        strcpy(&(*(s32 *)((u8 *)&lb_board_exp + 0x25C)), (*(void * *)((u8 *)&lb_rule_msg_etc + 0x8)));
    } else {
block_12:
        strcpy(&(*(s32 *)((u8 *)&lb_board_exp + 0x25C)), (*(void * *)((u8 *)&lb_rule_msg_etc + 0x4)));
    }
    KinshiYogo_chk(temp_s2 + 0x5B);
    strcpy(&(*(s32 *)((u8 *)&lb_board_exp + 0x2C0)), temp_s2 + 0x5B);
    Lb_num_to_str((*(s32 *)((u8 *)(temp_v0) + 8)), &(*(s32 *)((u8 *)&lb_board_message + 0x24)));
    strcat(&(*(s32 *)((u8 *)&lb_board_message + 0x24)), (*(void * *)((u8 *)&lb_quest_exp + 0x524)));
    Lb_num_to_str((*(s32 *)((u8 *)(temp_v0) + 4)), &(*(s32 *)((u8 *)&lb_board_message + 0x88)));
    strcat(&(*(s32 *)((u8 *)&lb_board_message + 0x88)), (*(void * *)((u8 *)&lb_quest_exp + 0x524)));
    temp_v1 = (u32) (*(u32 *)((u8 *)(temp_v0) + 0x10)) >> 0x1F;
    temp_v0_4 = (*(u32 *)((u8 *)(temp_v0) + 0x10)) / 60;
    sprintf(&sp80, &g_lb_63E7D8, ((temp_v0_4 + temp_v1) / 30) + ((u32) (temp_v0_4 + temp_v1) >> 0x1F), lb_rule_msg_etc, 0x88888889);
    han2zen(&sp80, &sp60);
    strcpy(&g_lb_635D9C, &sp60);
    sprintf(&g_lb_635E00, (*(s32 *)((u8 *)&map_name + ((*(s32 *)((u8 *)(temp_v0) + 0x14)) * 4))));
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v0) + 0x1C));
    if ((s32) temp_v1_2 < 0x3C) {
        strcpy(&g_lb_635E64, (*(s32 *)((u8 *)&D_00528A80 + (temp_v1_2 * 4))));
    }
}

