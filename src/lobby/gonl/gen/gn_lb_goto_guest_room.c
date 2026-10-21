/* G online (PC): lb_goto_guest_room, written from Monster Hunter G's code at lobby 0x599220 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Gold_add();
s32 cnWrap_SoundRequest();
s32 ran_suu();
extern u8 User_data;
extern u8 cw;
s32 Lb_check_hotel();                            /* extern */
s32 Lb_put_set01();                      /* extern */
extern s32 g_lb_5FC59C;
extern s32 lb_sys;

void lb_goto_guest_room(void *arg0, s16 arg1) {
    s32 temp_v0;
    u8 *temp_a0;
    u8 temp_v1;
    u8 *temp_a1;

    temp_v0 = Lb_check_hotel(arg1);
    switch (temp_v0) {                              /* irregular */
    case 1:
        if (arg1 == 0x55) {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA64)) = (s8) (((s32) (ran_suu(1) & 0xFFFF) % 27) * 2);
        }
        Gold_add(-(*(s32 *)((u8 *)&g_lb_5FC59C + (arg1 * 4))));
        cnWrap_SoundRequest(8, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        temp_a0 = &(*(u8 *)((u8 *)&User_data + 0xC3F)) + arg1;
        temp_v1 = *temp_a0;
        if ((s32) temp_v1 < 0x96) {
            *temp_a0 = temp_v1 + 1;
        }
        /* fallthrough */
    case 2:
        (*(s8 *)((u8 *)&lb_sys + 0x7C)) = 0;
        (*(s8 *)((u8 *)&lb_sys + 0x19)) = 5;
        (*(s16 *)((u8 *)(arg0) + 0x73A)) = arg1;
        temp_a1 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a1) + 0xFA63)) = (u8) ((*(u8 *)((u8 *)(temp_a1) + 0xFA63)) | ((1 << (arg1 - 0x51)) & 0xFF));
        lb_sys = 0x16;
        return;
    case 3:
        Lb_put_set01(0xB);
        /* fallthrough */
    case 0:
        return;
    }
}

