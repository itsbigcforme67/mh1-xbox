/* lb_gad01 - near-match fixes 0x005D5B10-0x005D5B9C: CallBack_Result_SendChatMessageTU. Whole file in lb_ad.c. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 *St_unique_tbl[];
extern u8 chatIDList[];
extern u8 npc_dialog_table[];
extern u8 room_price[];
extern char lit_220_00664EA0[];
extern char lit_221_00664ED0[];
extern char lit_1026_00665D68[];
extern char *lb_set01_msg[];
extern u8 lb_pit[0xC];
extern u8 User_data[];
f32 flSqrt(f32);
void flvecCopy(f32 *, f32 *);
void Lb_Pl_adj_calc();
void Lb_send_pl_warp();
int Lb_get_lb_rank();
int Quest_clear_bit_ck();
int Event_flag_ck();
int Get_sw2();
u8 *pull_enemy_work();
void cnLBS_Send_ChatMessage();
void cnLBS_Send_ChatMessageTU();
void CallBack_Result_SendChatMessageTU();
void Lb_chat_receipt();
void Plaza_chat_log_add();
void Chat_log_add(int, void *);
char *strcpy();
int strlen();
int sprintf(char *, const char *, ...);

void CallBack_Result_SendChatMessageTU(a)
long a;
{
    char buf[0x120];
    if (*(s8 *)&a == -1) {
        sprintf(buf + 0x1C, lit_220_00664EA0);
        strcpy(buf, lit_221_00664ED0);
        strcpy(buf + 8, lit_221_00664ED0);
        buf[0x11F] = 6;
        buf[0x11E] = 6;
        buf[0x11D] = 6;
        if (cw[0x35D5] != 0) {
            Lb_chat_receipt(buf);
            return;
        }
        Plaza_chat_log_add(buf);
    }
}
