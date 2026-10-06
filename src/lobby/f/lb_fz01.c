/* lb_fz01 0x005D2FF0-0x005D31E0: Lb_pl_to_chair */
/* Lobby: turn, chair, chat send, npc spawn, talk checks, hand-written from m2c drafts. */
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
void Lb_pl_to_chair(void) {
    f32 v[3];
    u8 *e;
    PLW *pl;
    u16 ang;
    u16 i;
    e = St_unique_tbl[0x4D];
    ang = 0;
    pl = &player_work[game_w.master];
    if (game_w.stage == 0x4D) {
        if (lb_sys.x68 != 0x18) {
            lb_sys.x6C = 0;
            Lb_send_chair_release(pl);
            lb_sys.x68 = 0;
            return;
        }
        lb_sys.x6C = 0;
        i = 0;
        lb_sys.x68 = 0;
        while (0 < *(u16 *)(e + 2) && e != 0) {
            if (*(u16 *)(e + 2) == 1 && *(u16 *)e == lb_sys.x66) {
                break;
            }
            i++;
            e += 0x18;
            if (i >= 0x14 || e == 0 || *(u16 *)(e + 2) == 0) {
                return;
            }
        }
        Lb_send_pl_warp(pl);
    }
    if (game_w.stage != 0x4D) {
        v[0] = *(f32 *)((u8 *)pl->fish878 + 4);
        v[1] = *(f32 *)((u8 *)pl->fish878 + 8);
        v[2] = *(f32 *)((u8 *)pl->fish878 + 0xC);
        ang = *(u16 *)((u8 *)pl->fish878 + 0x14);
    } else {
        f32 dx = pl->pos[0] - *(f32 *)(e + 4);
        f32 dz = pl->pos[2] - *(f32 *)(e + 0xC);
        if (flSqrt(dx * dx + dz * dz) <= *(f32 *)(e + 0x10)) {
            v[0] = *(f32 *)(e + 4);
            v[1] = *(f32 *)(e + 8);
            v[2] = *(f32 *)(e + 0xC);
            ang = *(u16 *)(e + 0x14);
        }
    }
    flvecCopy(&pl->work800, v);
    Lb_Pl_adj_calc(pl, 0x14);
    pl->ang_y = ang;
    Lb_Pl_act_set2(pl, 0, 0x4C, 0);
}
