/* lb_gad02 - near-match fixes 0x005D7D60-0x005D7E34: Lb_check_hotel. Whole file in lb_ad.c. */
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

int Lb_check_hotel(int a) {
    int s0;
    int rank;
    int v;
    s0 = a - 0x51;
    rank = (s8)Lb_get_lb_rank(*(u8 *)0x3C733B);
    if (Event_flag_ck(4) == 0) {
        return 0;
    }
    v = 1;
    if (cw[0x35D7] & (1 << s0)) {
        return 2;
    }
    if ((s8)rank >= s0) {
        if (*(s32 *)0x3C6FE0 >= ((s32 *)room_price)[s0]) {
            return v;
        } else {
            return 3;
        }
    }
    return 0;
}
