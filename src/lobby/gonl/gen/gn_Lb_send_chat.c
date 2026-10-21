/* G online (PC): Lb_send_chat, written from Monster Hunter G's code at lobby 0x59A380 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_SendChatMessageTU();
s32 Online_ck();
s32 Plaza_chat_log_add();
s32 cnLBS_Send_ChatBinaryTU();
s32 cnLBS_Send_ChatMessage();
void * memcpy();
void * strcpy();
extern u8 cw;
s32 Lb_chat_receipt();                 /* extern */
s32 Visual_main();                               /* extern */
extern s8 chatIDList;
extern u8 chat_cnfg_font_color;
extern u8 my_user_handle;
extern u8 my_user_id;

void Lb_send_chat(s32 unused_a0, s32 arg1, s32 arg2) {
    u8 g_frame[0x190] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s32 *)(g_frame + 0x70))
#define sp78 (*(s32 *)(g_frame + 0x78))
#define sp8C (*(s32 *)(g_frame + 0x8C))
#define sp18D (*(s8 *)(g_frame + 0x18D))
#define sp18E (*(s8 *)(g_frame + 0x18E))
#define sp18F (*(u8 *)(g_frame + 0x18F))
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
                cnLBS_Send_ChatBinaryTU(arg2 & 0xFF, var_s0, (void *) arg1, temp_v0 & 0xFFFF, (s32) CallBack_Result_SendChatMessageTU);
            }
            var_s2 += 1;
            var_s0 += 8;
        } while (var_s2 < 7);
        memcpy(&sp70, &my_user_id, 8);
        memcpy(&sp78, &my_user_handle, 0x10);
        strcpy(&sp8C, (void *) arg1);
        sp18E = 0;
        sp18D = 0;
        sp18F = *(&chat_cnfg_font_color + (arg2 & 0xFF));
        if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
            Lb_chat_receipt(&sp70);
            return;
        }
        Plaza_chat_log_add((s32) &sp70);
    }
}

