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
void cnLBS_Send_ChatMessage();
void cnLBS_Send_ChatMessageTU();
void CallBack_Result_SendChatMessageTU();
void Lb_chat_receipt();
void Plaza_chat_log_add();
void Chat_log_add(int, void *);
char *strcpy();
int strlen();
int sprintf(char *, const char *, ...);
void lb_pl_turn_sub(PLW *pl) {
    int a2;
    int a1;
    int t1;
    int v1;
    u16 a3;
    u16 a0;
    u32 t0;
    if ((s16)Lb_act_ck(pl, 0, 1) != 0 || (s16)Lb_act_ck(pl, 0, 0x24) != 0) {
        t1 = 0x71C;
    } else {
        t1 = 0x5B0;
        if ((s16)Lb_act_ck(pl, 0, 0x1F) != 0) {
        } else {
            t1 = 0;
            if ((s16)Lb_act_ck(pl, 0, 0x3F) != 0) {
            } else {
                t1 = 0xFA4;
            }
        }
    }
    a2 = pl->ang[1];
    a3 = pl->ang_y;
    a0 = *(u16 *)((u8 *)pl + 0x2DC);
    t0 = (a3 - (a2 & 0xFFFF)) & 0xFFFF;
    switch (a0) {
    case 39:
        t1 = 0x71C;
        a1 = 0x80;
        break;
    case 3:
        a1 = 0x80;
        break;
    case 4:
        a1 = 0xA0;
        break;
    default:
        a1 = 0;
        break;
    }
    if ((u32)((t0 + t1) & 0xFFFF) < (u32)(t1 * 2)) {
        pl->ang[1] = a3;
        *(s8 *)((u8 *)pl + 0x2F8) = 0;
        a0 = *(u16 *)((u8 *)pl + 0x750);
        v1 = a0 + 0x300;
        if (v1 < 0x601) {
            *(u16 *)((u8 *)pl + 0x750) = 0;
        } else {
            if (a0 < 0x8000) {
                v1 = a0 - 0x300;
            }
            *(u16 *)((u8 *)pl + 0x750) = v1;
        }
    } else {
        if (t0 < 0x8000U) {
            pl->ang[1] = (a2 + t1) & 0xFFFF;
            v1 = *(u16 *)((u8 *)pl + 0x750) - a1;
        } else {
            pl->ang[1] = (a2 - t1) & 0xFFFF;
            v1 = *(u16 *)((u8 *)pl + 0x750) + a1;
        }
        *(u16 *)((u8 *)pl + 0x750) = v1;
    }
    a0 = *(u16 *)((u8 *)pl + 0x750);
    if (a0 < 0xF601 && a0 >= 0xA00) {
        u16 v2 = 0xF600;
        if (a0 >= 0x8000) {
        } else {
            v2 = 0xA00;
        }
        *(u16 *)((u8 *)pl + 0x750) = v2;
    }
}
void Lb_pl_to_chair(void) {
    f32 v[3];
    u16 ang;
    u16 i;
    u8 *e;
    PLW *pl;
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
        goto first;
        for (;;) {
            if (*(u16 *)(e + 2) == 1 && *(u16 *)e != lb_sys.x66) {
                i = (i + 1) & 0xFFFF;
                e += 0x18;
                if (i >= 0x14 || e == 0 || *(u16 *)(e + 2) == 0) {
                    return;
                }
                goto first;
            }
            break;
first:
            if ((s16)*(u16 *)(e + 2) <= 0 || e == 0) {
                break;
            }
            if (*(u16 *)(e + 2) != 1) {
                i = (i + 1) & 0xFFFF;
                e += 0x18;
                if (i >= 0x14 || e == 0 || *(u16 *)(e + 2) == 0) {
                    return;
                }
                goto first;
            }
            if (*(u16 *)e != lb_sys.x66) {
                i = (i + 1) & 0xFFFF;
                e += 0x18;
                if (i >= 0x14 || e == 0 || *(u16 *)(e + 2) == 0) {
                    return;
                }
                goto first;
            }
            break;
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
            cnLBS_Send_ChatMessage(s, len & 0xFFFF);
            return;
        }
        i = 0;
        id = chatIDList;
        do {
            if (*(s8 *)id != 0) {
                cnLBS_Send_ChatMessageTU(id, s, len & 0xFFFF, CallBack_Result_SendChatMessageTU);
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
void CallBack_Result_SendChatMessageTU(a)
int a;
{
    char buf[0x120];
    if ((s8)a == -1) {
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
void Lb_put_chat(int a) {
    char buf[0x120];
    strcpy(buf, lit_1026_00665D68);
    strcpy(buf + 8, lit_1026_00665D68);
    strcpy(buf + 0x1C, lb_set01_msg[a]);
    buf[0x11E] = 6;
    buf[0x11D] = 6;
    buf[0x11F] = 6;
    if (cw[0x35D5] != 0) {
        Chat_log_add(0, buf);
        return;
    }
    Plaza_chat_log_add(buf);
}
void Lb_npc_set(int a) {
    s16 j;
    int i;
    u8 *e;
    u8 *p;
    int v;
    u8 n;
    p = npc_dialog_table + 0x174 + a;
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
        if (v >= 0) {
            if (v > 0x55) {
                v = 0;
            }
        } else {
            v = 0;
        }
        if ((&lb_sys.x87)[1 + (s16)v] == 1) {
            n -= 1;
        }
        break;
    }
    i = 0;
    j = 0;
    if (n > 0) {
        do {
            e = pull_enemy_work();
            if (e != 0) {
                *(s8 *)(e + 0x1E) = 1;
                *(s8 *)(e + 0x34F) = 0;
                *(s16 *)(e + 0xC) = j;
                *(s8 *)(e + 0x1B) = j;
                e[0x736] = game_w.stage;
            }
            i += 1;
            j += 1;
        } while (i < n);
    }
}
int Lb_check_hotel(int a) {
    int s0;
    int rank;
    int v;
    s0 = a - 0x51;
    rank = (s8)Lb_get_lb_rank();
    if (Event_flag_ck(4) == 0) {
        return 0;
    }
    v = 1;
    if (cw[0x35D7] & (1 << s0)) {
        return 2;
    }
    if ((s8)rank >= s0) {
        if (*(s32 *)0x3C6FE0 >= ((s32 *)room_price)[s0]) {
        } else {
            v = 3;
        }
        return v;
    }
    return 0;
}
s8 Lb_talk_check_default(a)
int a;
{
    int pad;
    int t;
    int m;
    pad = Get_sw2(0) & 0xFFFF;
    if (*(s32 *)lb_pit >= 0) {
        *(s32 *)lb_pit = *(s32 *)lb_pit + 1;
    }
    if (*(s8 *)(lb_pit + 0xB) == 0) {
        m = (s8)a;
        t = pad & 0xFFFF;
        if (!(m & 1)) {
            if (t & 0x20) {
                *(s32 *)lb_pit = 0;
                if (*(s8 *)(lb_pit + 9) != 1) {
                    if (!(m & 2)) {
                        cnWrap_SoundRequest(0);
                    }
                } else if (!(m & 2)) {
                    cnWrap_SoundRequest(3);
                }
                return 1;
            }
            if (t & 0x40) {
                if (*(s32 *)(*(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8) == 1) {
                    if (*(s8 *)(lb_pit + 9) != 1) {
                        *(s8 *)(lb_pit + 9) = 1;
                        cnWrap_SoundRequest(3);
                        return 0;
                    }
                    *(s32 *)lb_pit = 0;
                    if (!(m & 2)) {
                        cnWrap_SoundRequest(3);
                    }
                    return 1;
                }
                *(s32 *)lb_pit = 0;
                if (!(m & 2)) {
                    cnWrap_SoundRequest(3);
                }
                return 1;
            }
            if (*(s32 *)(*(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8) == 1) {
                if (t & 0x800) {
                    if (*(s8 *)(lb_pit + 9) != 0) {
                        cnWrap_SoundRequest(1);
                        *(s8 *)(lb_pit + 9) = 0;
                    }
                } else if ((t & 0x400) && *(s8 *)(lb_pit + 9) != 1) {
                    cnWrap_SoundRequest(1);
                    *(s8 *)(lb_pit + 9) = 1;
                }
            }
            return 0;
        }
        return 0;
    }
    if (!((s8)a & 1) && (pad & 0xFFFF & 0x60)) {
        *(s32 *)lb_pit = -1;
        return 0;
    }
    return 0;
}
