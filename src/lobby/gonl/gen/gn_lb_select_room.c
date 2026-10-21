/* G online (PC): lb_select_room, written from Monster Hunter G's code at lobby 0x58AA60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 cnWrap_SoundRequest();
extern u8 g_main_2EFABC;
extern u8 g_main_2EFABC;
extern u8 lb_board_exp;
extern u8 lb_sys;
extern u8 pNet;
s32 Lb_put_set01();                  /* extern */
s32 Lbs_GetClassAdd();                          /* extern */
void *Lbs_GetRoomInfo();                      /* extern */
s32 lb_select_set_data();                    /* extern */
s32 sprintf(); /* extern */
extern s32 lb_num_str;
extern s32 lit_427_0065E260;

s32 lb_select_room(void) {
    s32 temp_s0;
    s32 temp_v1;
    u16 temp_a0;
    u8 temp_a1;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 *temp_s1;
    u8 *temp_v0;
    u8 *temp_v1_2;

    temp_s1 = Lbs_GetRoomInfo((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)));
    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_v0 = (*(void **)&pNet);
    temp_v0_2 = (*(u8 *)((u8 *)(temp_v0) + 0x22));
    if (temp_v0_2 != 0) {

    } else {
        (*(u8 *)((u8 *)(temp_v0) + 0x22)) = (u8) (temp_v0_2 + 1);
        temp_a0 = (*(u16 *)((u8 *)(temp_s1) + 0));
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x32)) = (s8) temp_a0;
        if (((*(u8 *)((u8 *)(temp_s1) + 0x16)) == 3) && ((s32) (*(u16 *)((u8 *)(temp_s1) + 6)) < (s32) (*(u16 *)((u8 *)(temp_s1) + 8)))) {
            if ((*(s8 *)&g_main_2EFABC) != 0) {
                goto block_6;
            }
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 0;
        } else {
block_6:
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) = 1;
        }
        lb_select_set_data(temp_a0);
    }
    Lbs_GetClassAdd();
    temp_a1 = (*(u8 *)((u8 *)(temp_s1) + 0x16));
    switch (temp_a1) {                              /* irregular */
    case 3:
        if (((s32) (*(u16 *)((u8 *)(temp_s1) + 6)) >= (s32) (*(u16 *)((u8 *)(temp_s1) + 8))) || ((*(s8 *)&g_main_2EFABC) != 0)) {
            if (temp_a1 != 4) {
            default:
                Lb_put_set01(8, temp_a1);
            } else {
            case 4:
                Lb_put_set01(9, temp_a1);
            }
            cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 3;
        }
        temp_v1 = temp_s0 & 0xFFFF;
        if (temp_v1 & 0x20) {
            (*(u8 *)((u8 *)&lb_sys + 0x7E)) = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
            Lbs_GetRoomInfo((*(u8 *)((u8 *)&lb_sys + 0x7E)), temp_a1);
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 0;
        }
        if (temp_v1 & 0x40) {
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0xF;
            cnWrap_SoundRequest(3, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 3;
        }
        if (temp_v1 & 0x200) {
            temp_v1_2 = (*(void **)&pNet);
            temp_v0_3 = (*(u8 *)((u8 *)(temp_v1_2) + 0x29)) + 1;
            (*(u8 *)((u8 *)(temp_v1_2) + 0x29)) = temp_v0_3;
            if ((temp_v0_3 & 0xFF) >= 3) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) = 0U;
            }
            cnWrap_SoundRequest(6, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        sprintf(&(*(s32 *)((u8 *)&lb_board_exp + 0x130)), &lit_427_0065E260, (*(s32 *)((u8 *)&lb_num_str + ((*(u16 *)((u8 *)(temp_s1) + 6)) * 4))), (*(s32 *)((u8 *)&lb_num_str + 0x2C)), (*(s32 *)((u8 *)&lb_num_str + ((*(u16 *)((u8 *)(temp_s1) + 8)) * 4))));
        return 2;
    }
}

