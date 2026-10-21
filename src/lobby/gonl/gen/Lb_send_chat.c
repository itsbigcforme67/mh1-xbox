/* G online (PC): Lb_send_chat, written from Monster Hunter G's code at lobby 0x59A380 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 Lb_chat_receipt();                 /* extern */
s32 Online_ck();                                    /* extern */
s32 Plaza_chat_log_add();              /* extern */
s32 Visual_main();                           /* extern */
s32 cnLBS_Send_ChatBinaryTU(); /* extern */
s32 cnLBS_Send_ChatMessage();       /* extern */
s32 memcpy();      /* extern */
s32 strcpy();                 /* extern */
extern s8 chatIDList;
extern u8 chat_cnfg_font_color;
extern u8 my_user_handle;
extern u8 my_user_id;

void Lb_send_chat(s32 arg1, s32 arg2) {
    u8 sp18F;
    s8 sp18E;
    s8 sp18D;
    union { s32 v; u8 b[0x104]; } u_sp8C;
#define sp8C (u_sp8C.v)
    union { s32 v; u8 b[0x118]; } u_sp78;
#define sp78 (u_sp78.v)
    union { s32 v; u8 b[0x120]; } u_sp70;
#define sp70 (u_sp70.v)
    s32 temp_v0;
    s32 var_s2;
    s8 *var_s0;

    if ((Online_ck() != 0) && (temp_v0 = Visual_main(arg1), ((temp_v0 < 0x40) != 0))) {
        if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xF74E)) == 0) {
            cnLBS_Send_ChatMessage(arg1, temp_v0 & 0xFFFF);
            return;
        }
        var_s2 = 0;
        var_s0 = &chatIDList;
        do {
            if (*var_s0 != 0) {
                cnLBS_Send_ChatBinaryTU(arg2 & 0xFF, var_s0, arg1, temp_v0 & 0xFFFF);
            }
            var_s2 += 1;
            var_s0 += 8;
        } while (var_s2 < 7);
        memcpy(&sp70, &my_user_id, 8);
        memcpy(&sp78, &my_user_handle, 0x10);
        strcpy(&sp8C, arg1);
        sp18E = 0;
        sp18D = 0;
        sp18F = *(&chat_cnfg_font_color + (arg2 & 0xFF));
        if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
            Lb_chat_receipt(&sp70);
            return;
        }
        Plaza_chat_log_add(&sp70);
    }
}

