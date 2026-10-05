/* lb_gad04 - near-match fixes 0x005D59E0-0x005D5B0C: Lb_send_chat. Whole file in lb_ad.c. */
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
void cnLBS_Send_ChatMessage(char *, u16);
void cnLBS_Send_ChatMessageTU(u8 *, char *, u16, void *);
void CallBack_Result_SendChatMessageTU();
void Lb_chat_receipt();
void Plaza_chat_log_add();
void Chat_log_add(int, void *);
char *strcpy();
int strlen();
int sprintf(char *, const char *, ...);

void Lb_send_chat(a, s)
int a;
char *s;
{
    char buf[0x120];
    int len;
    int i;
    u8 *id;
    if (Online_ck() != 0) {
        len = strlen(s);
        if (len >= 0x40) {
            len = 0x3F;
        }
        if (cw[0x32BE] == 0) {
            cnLBS_Send_ChatMessage(s, len);
            return;
        }
        i = 0;
        id = chatIDList;
        do {
            if (*(s8 *)id != 0) {
                cnLBS_Send_ChatMessageTU(id, s, len, CallBack_Result_SendChatMessageTU);
            }
            i += 1;
            id += 8;
        } while (i < 7);
        strcpy(buf, (char *)cw + 0x440);
        strcpy(buf + 8, (char *)cw + 0x448);
        strcpy(buf + 0x1C, s);
        buf[0x11E] = 0;
        buf[0x11F] = 5;
        buf[0x11D] = 0;
        if (cw[0x35D5] != 0) {
            Lb_chat_receipt(buf);
            return;
        }
        Plaza_chat_log_add(buf);
    }
}
