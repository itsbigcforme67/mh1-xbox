/* G online (PC): Lb_put_chat, written from Monster Hunter G's code at lobby 0x59D800 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 lb_rule_member;
s32 Chat_log_add();           /* extern */
s32 Plaza_chat_log_add();              /* extern */
s32 memset();        /* extern */
s32 sprintf();               /* extern */
s32 strcpy();                     /* extern */
extern u8 g_lb_636BC0;

void Lb_put_chat(s32 arg0, s32 arg1) {
    u8 g_frame[0x150] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s32 *)(g_frame + 0x30))
#define sp38 (*(s32 *)(g_frame + 0x38))
#define sp4C (*(s32 *)(g_frame + 0x4C))
#define sp14D (*(s8 *)(g_frame + 0x14D))
#define sp14E (*(s8 *)(g_frame + 0x14E))
#define sp14F (*(s8 *)(g_frame + 0x14F))

    memset(&sp30, 0, 0x120);
    strcpy(&sp30, (*(u8 *)((u8 *)&lb_rule_member + 0x14)));
    strcpy(&sp38, (*(u8 *)((u8 *)&lb_rule_member + 0x14)));
    if (arg1 == 0) {
        strcpy(&sp4C, (*(s32 *)((u8 *)&g_lb_636BC0 + (arg0 * 4))));
    } else {
        sprintf(&sp4C, (*(s32 *)((u8 *)&g_lb_636BC0 + (arg0 * 4))), arg1);
    }
    sp14E = 6;
    sp14D = 6;
    sp14F = 6;
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
        Chat_log_add(0xFF, &sp30);
        return;
    }
    Plaza_chat_log_add(&sp30);
}

