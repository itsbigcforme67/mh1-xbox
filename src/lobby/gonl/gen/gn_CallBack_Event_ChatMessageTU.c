/* G online (PC): CallBack_Event_ChatMessageTU (G lobby 0x586AB0; HAND-FIXED as CallBack_Event_ChatMessage): a line
 * to this hunter alone; its third colour 0xFF becomes the chat settings' colour of the kind at +0x11C. */
#include "../gonl.h"
extern u8 *cw;
extern u8 chat_cnfg_font_color[];

s32 Lb_chat_receipt();
s32 Plaza_chat_log_add();
s32 cnLBS_Get_ChatMessage();
#ifdef __MWERKS__
s32 get_font_col();
#else
unsigned char get_font_col();   /* the real return type (tools/check_kr_returns.py) */
#endif

void CallBack_Event_ChatMessageTU(void)
{
    u8 m[0x120];
    int k;
    cnLBS_Get_ChatMessage(m);
    for (k = 0x11D; k <= 0x11F; k++)
        m[k] = (u8)get_font_col(m[k]);
    if (m[0x11D] == 0xFF)
        m[0x11D] = 0;
    if (m[0x11E] == 0xFF)
        m[0x11E] = 0;
    if (m[0x11F] == 0xFF)
        m[0x11F] = chat_cnfg_font_color[m[0x11C]];
    if (cw[0xFA61] != 0)
        Lb_chat_receipt(m);
    else
        Plaza_chat_log_add(m);
}
