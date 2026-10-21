/* G online (PC): Plaza_chatlog_mv, written from Monster Hunter G's code at lobby 0x58F100 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Name_ID_change();
s32 Plaza_get_chat_line_num();
s32 plaza_chat_log_disp_line();
s32 se_req();
extern u8 PitMenu;
void Plaza_chatlog_mv(s32 arg0) {
    s32 temp_s0;
    u8 temp_a0;
    u8 temp_v1;

    temp_s0 = arg0 & 0xFFFF;
    if (temp_s0 & 0x200) {
        Name_ID_change();
    }
    (*(u8 *)((u8 *)&PitMenu + 0x29)) = 0;
    if (Plaza_get_chat_line_num() >= 0xAU) {
        if ((*(u8 *)((u8 *)&PitMenu + 0x27)) != 0) {
            (*(u8 *)((u8 *)&PitMenu + 0x29)) = (u8) ((*(u8 *)((u8 *)&PitMenu + 0x29)) | 2);
        }
        if ((plaza_chat_log_disp_line((s32) (*(u8 *)((u8 *)&PitMenu + 0x27))) & 0xFF) >= 0xA) {
            (*(u8 *)((u8 *)&PitMenu + 0x29)) = (u8) ((*(u8 *)((u8 *)&PitMenu + 0x29)) | 1);
            if (temp_s0 & 0x2000) {
                temp_a0 = (*(u8 *)((u8 *)&PitMenu + 0x27));
                if ((s32) temp_a0 < ((*(u8 *)((u8 *)&PitMenu + 0x26)) - 1)) {
                    (*(s8 *)((u8 *)&PitMenu + 0x28)) = 0;
                    (*(u8 *)((u8 *)&PitMenu + 0x27)) = (u8) (temp_a0 + 1);
                    (*(u8 *)((u8 *)&PitMenu + 0x29)) = (u8) ((*(u8 *)((u8 *)&PitMenu + 0x29)) | 4);
                    se_req(7, 0x17, 0);
                    if ((plaza_chat_log_disp_line((s32) (*(u8 *)((u8 *)&PitMenu + 0x27))) & 0xFF) < 0xA) {
                        (*(u8 *)((u8 *)&PitMenu + 0x28)) = 1;
                    }
                }
            }
        } else {
            (*(u8 *)((u8 *)&PitMenu + 0x28)) = 1;
        }
        if (temp_s0 & 0x1000) {
            temp_v1 = (*(u8 *)((u8 *)&PitMenu + 0x27));
            if ((s32) temp_v1 > 0) {
                (*(u8 *)((u8 *)&PitMenu + 0x27)) = (u8) (temp_v1 - 1);
                (*(u8 *)((u8 *)&PitMenu + 0x29)) = (u8) ((*(u8 *)((u8 *)&PitMenu + 0x29)) | 8);
                se_req(7, 0x17, 0);
                (*(u8 *)((u8 *)&PitMenu + 0x28)) = 0;
            }
        }
    }
}

