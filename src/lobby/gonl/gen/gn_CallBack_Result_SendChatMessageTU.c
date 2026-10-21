/* G online (PC): CallBack_Result_SendChatMessageTU, written from Monster Hunter G's code at lobby 0x59A4F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 lb_rule_member;
s32 Lb_chat_receipt();                 /* extern */
s32 Plaza_chat_log_add();              /* extern */
s32 sprintf();                    /* extern */
s32 strcpy();                     /* extern */
extern s32 g_lb_636BD8;

void CallBack_Result_SendChatMessageTU(s64 arg0) {
    u8 g_frame[0x140] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
#define sp20 (*(s32 *)(g_frame + 0x20))
#define sp28 (*(s32 *)(g_frame + 0x28))
#define sp3C (*(s32 *)(g_frame + 0x3C))
#define sp13D (*(s8 *)(g_frame + 0x13D))
#define sp13E (*(s8 *)(g_frame + 0x13E))
#define sp13F (*(s8 *)(g_frame + 0x13F))

    sp18 = arg0;
    if ((s8) sp18 == -1) {
        sprintf(&sp3C, g_lb_636BD8);
        strcpy(&sp20, (*(s32 *)((u8 *)&lb_rule_member + 0x14)));
        strcpy(&sp28, (*(s32 *)((u8 *)&lb_rule_member + 0x14)));
        sp13F = 6;
        sp13E = 6;
        sp13D = 6;
        if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
            Lb_chat_receipt(&sp20);
            return;
        }
        Plaza_chat_log_add(&sp20);
    }
}

