/* lb_gad05 - near-match fixes 0x005D7790-0x005D78C0: Lb_npc_set. Whole file in lb_ad.c. */
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

void Lb_npc_set(int a) {
    int i;
    int j;
    u8 *e;
    u8 *p;
    int v;
    int n;
    u8 *tb;
    tb = npc_dialog_table + 0x174;
    p = tb + a;
    n = *p;
    switch (a) {
    case 0x57:
        if (Quest_clear_bit_ck(0x88) == 1) {
            n += 1;
        }
        break;
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
        v = (s16)(game_w.stage - 0x51);
        if (v < 0 || v > 0x55) {
            v = 0;
        }
        if ((&lb_sys.x87)[1 + (s16)v] == 1) {
            n -= 1;
        }
        break;
    }
    i = 0;
    j = 0;
    for (; i < n; i++, j++) {
        e = pull_enemy_work();
        if (e != 0) {
            *(s8 *)(e + 0x1E) = 1;
            *(s8 *)(e + 0x34F) = 0;
            *(s16 *)(e + 0xC) = j;
            *(s8 *)(e + 0x1B) = j;
            e[0x736] = game_w.stage;
        }
    }
}
