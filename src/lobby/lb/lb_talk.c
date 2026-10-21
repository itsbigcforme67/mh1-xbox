/* lb_talk - lobby.bin 0x00533A00-0x00535100. NPC talk start in the lobby
 * town (Lb_talk) and the talk-script chooser helpers. */
#include "lobby.h"
#include "pl.h"
#include "ud.h"
#include "em.h"
#include "lbsys_g.h"
#ifndef __MWERKS__
unsigned short Get_sw2();   /* (declared before its first use) */
#endif

int Lb_put_npc_default();
void lb_talk_init();

int Lb_talk(void) {
    PLW *p = &player_work[*(u8 *)0x3F34C1];
    void *pl = p->x3B0;

    if (pl == 0) {
        lb_sys.x68 = 0;
        return 3;
    }
    switch (lb_pit.step) {
    case 0:
        lb_talk_init(pl);
        lb_pit.step++;
        break;
    case 1:
        return Lb_put_npc_default(pl);
    }
    return 2;
}

int Event_flag_ck();
int Quest_clear_bit_ck();
int Lb_check_existF();
int Lb_guild_check_requireF();

#ifdef VERSION_MHG
int Online_ck();
int get_questLevelNum(int n);
/* G (lobby 0x543E20): the talk records' condition numbers are story levels (get_questLevelNum): 0..9 in the online town, 10..18
 * in the offline village (lobby 0x543C50 / 0x543D40: two jump-table switches) */
int ck_talkEvent(u8 id) {
    static const u8 on_tbl[10] = { 1, 4, 6, 8, 10, 12, 0x16, 0x1D, 0x1E, 0x1F };
    static const u8 off_tbl[9] = { 0x23, 0x24, 0x27, 0x29, 0x2B, 0x2D, 0x2E, 0x32, 0x3A };
    if (Online_ck() == 1)
        return id < 10 ? get_questLevelNum(on_tbl[id]) : 0;
    return id >= 10 && id < 19 ? get_questLevelNum(off_tbl[id - 10]) : 0;
}
#else
int ck_talkEvent(u8 id) {
    switch (id) {
    case 0:
        if (Event_flag_ck(4) == 1) return 1;
        break;
    case 1:
        if (Quest_clear_bit_ck(0x27) == 1) return 1;
        break;
    case 2:
        if (Quest_clear_bit_ck(9) == 1) return 1;
        break;
    case 3:
        if (Quest_clear_bit_ck(0x65) == 1) return 1;
        break;
    case 4:
        if (Quest_clear_bit_ck(0x4F) == 1) return 1;
        break;
    case 5:
        if (Quest_clear_bit_ck(0x61) == 1) return 1;
        break;
    case 6:
        if (Quest_clear_bit_ck(0x6B) == 1) return 1;
        break;
    case 7:
        if (Lb_guild_check_requireF() == 1) return 1;
        break;
    case 8:
        if (Lb_guild_check_requireF() == 1 && Lb_check_existF() == 0) return 1;
        break;
    case 9:
        if (Event_flag_ck(2) == 1) return 1;
        break;
    case 10:
        if (Event_flag_ck(0) == 1) return 1;
        break;
    case 11:
        if (Quest_clear_bit_ck(0x83) == 1) return 1;
        break;
    case 12:
        if (Quest_clear_bit_ck(0x88) == 1) return 1;
        break;
    case 13:
        if (Quest_clear_bit_ck(0x89) == 1) return 1;
        break;
    case 14:
        if (Quest_clear_bit_ck(0x8B) == 1) return 1;
        break;
    case 15:
        if (Event_flag_ck(3) == 1) return 1;
        break;
    case 16:
        if (Event_flag_ck(1) == 1) return 1;
        break;
    case 17:
        if (Quest_clear_bit_ck(0xAB) == 1) return 1;
        break;
    case 18:
        if (Event_flag_ck(0x15) == 1) return 1;
        break;
    }
    return 0;
}
#endif

typedef struct LB_TALKREC {
    u8 id;
    u8 first;
    u8 again;
} LB_TALKREC;

void lb_talk_setDefault(LB_NPCW *npc, LB_TALKREC *rec, int unused) {
    s8 i = 0;

    do {
        if (rec->id == 0xFF || ck_talkEvent(rec->id) == 1) break;
        i++;
        rec++;
    } while (i < 100);
    if (npc->x2E == 0) {
        lb_pit.pos += rec->first * 8;
    } else {
        lb_pit.pos += rec->again * 8;
    }
}

extern s16 hotel_count[6];

/* checks the hotel-stay count of kind `kind` (byte 0x3E8+kind of User_data)
 * against hotel_count[i] for i = 0..5 while evflag decrements; queues the
 * matching reward talk (item 0xAE/0xAF, count from i) */
int lb_check_hotel_num(LB_NPCW *npc, u8 flag, s8 kind) {
    u8 i;
    s16 *cnt = hotel_count;
    u8 *ud = (u8 *)&User_data[0] + kind + 0x3E8;

    for (i = 0; i < 6; flag--, i++, cnt++) {
        if (*ud >= *cnt && Event_flag_ck(flag) == 0) {
            lb_sys.x68 = 0x2C;
            lb_pit.pos += 0x198;
            switch (kind) {
            case 4:
                npc->item = 0xAF;
                npc->num = 12 - i * 2;
                break;
            case 3:
                npc->item = 0xAF;
                npc->num = 6 - i;
                break;
            case 2:
                npc->item = 0xAE;
                npc->num = 12 - i * 2;
                break;
            case 1:
                npc->item = 0xAE;
                npc->num = 6 - i;
                break;
            }
            npc->flag = flag;
            return 1;
        }
    }
    return 0;
}

int Lb_get_lb_rank();
void Event_flag_set();

/* guild hall receptionist: pick the talk by guild rank and which hotel
 * rewards / guild requests are pending */
void lb_talk_setGHRcpt(LB_NPCW *npc) {
    int rank = Lb_get_lb_rank(User_data[0].rank) & 0xFF;

    if (Event_flag_ck(4) == 0) {
        if (npc->x2E != 0) lb_pit.pos += 0x18;
        return;
    }
    switch (rank & 0xFF) {
    case 4:
        if (lb_check_hotel_num(npc, 0x39, 4) != 1 && lb_check_hotel_num(npc, 0x3F, 3) != 1 &&
            lb_check_hotel_num(npc, 0x45, 2) != 1 && lb_check_hotel_num(npc, 0x4B, 1) != 1) {
            if (ck_talkEvent(8) == 1) {
                if (npc->x2E != 0) lb_pit.pos += 0x190;
                else lb_pit.pos += 0x178;
            } else if (ck_talkEvent(7) == 1) {
                if (npc->x2E != 0) lb_pit.pos += 0x170;
                else lb_pit.pos += 0x158;
            } else if (ck_talkEvent(6) == 1) {
                if (npc->x2E != 0) lb_pit.pos += 0x1B8;
                else lb_pit.pos += 0x1A0;
            } else if (ck_talkEvent(5) == 1) {
                if (npc->x2E != 0) lb_pit.pos += 0x150;
                else lb_pit.pos += 0x140;
            } else if (Event_flag_ck(9) == 0) {
                Event_flag_set(9);
                lb_pit.pos += 0x120;
            } else {
                lb_pit.pos += 0x130;
            }
            return;
        }
        break;
    case 3:
        if (lb_check_hotel_num(npc, 0x3F, 3) != 1 && lb_check_hotel_num(npc, 0x45, 2) != 1 &&
            lb_check_hotel_num(npc, 0x4B, 1) != 1) {
            if (Event_flag_ck(8) == 0) {
                Event_flag_set(8);
                lb_pit.pos += 0xE8;
            } else {
                lb_pit.pos += 0x108;
            }
            return;
        }
        break;
    case 2:
        if (lb_check_hotel_num(npc, 0x45, 2) != 1 && lb_check_hotel_num(npc, 0x4B, 1) != 1) {
            if (Event_flag_ck(7) == 0) {
                Event_flag_set(7);
                lb_pit.pos += 0xB0;
            } else {
                lb_pit.pos += 0xD0;
            }
            return;
        }
        break;
    case 1:
        if (lb_check_hotel_num(npc, 0x4B, 1) != 1) {
            if (Event_flag_ck(6) == 0) {
                Event_flag_set(6);
                lb_pit.pos += 0x58;
                return;
            }
            lb_pit.pos += 0x98;
            return;
        }
        break;
    case 0:
        if (Event_flag_ck(5) == 0) {
            Event_flag_set(5);
            lb_pit.pos += 0x20;
            return;
        }
        lb_pit.pos += 0x50;
        break;
    }
}

void NPCZoomInCameraCancel();
void cnWrap_SoundRequest();
short Ud_item_num_ck3();
void Ud_item_stack();
void set01_set();
void set01_set2();
void Omake_flag_set();
int Omake_flag_ck();
extern void *lb_talk_tbl[];

/* NPC talk with a reward: the market keeper hands out item 0xAC */
void Lb_event_market(void) {
    void *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    int r;

    if (pl == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        NPCZoomInCameraCancel();
        cnWrap_SoundRequest(3);
        return;
    }
    switch (lb_sys.step) {
    case 0:
        lb_talk_init(pl);
        lb_sys.step++;
        return;
    case 1:
        r = Lb_put_npc_default(pl);
        switch (r) {
        case 0:
        case 3:
            lb_sys.x68 = 0;
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            cnWrap_SoundRequest(3);
            if (Ud_item_num_ck3(0xAC) > 0) {
                Ud_item_stack(0xAC, 1);
                set01_set(1, 0xD, 0xAC);
                Event_flag_set(0x51);
                if (Omake_flag_ck(5) == 0) {
                    set01_set2(lb_talk_tbl[0]);
                    Omake_flag_set(5);
                }
            } else {
                set01_set2(lb_talk_tbl[1]);
            }
            lb_sys.step = 0;
            break;
        }
    }
}

void Lb_put_set01();

/* guild hall: hands over the requested item (npc->item x npc->num) */
void Lb_event_GH(void) {
    EMW *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    LB_NPCW *npc = (LB_NPCW *)pl->ex;
    int r;

    if (pl == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        NPCZoomInCameraCancel();
        cnWrap_SoundRequest(3);
        return;
    }
    switch (lb_sys.step) {
    case 0:
        lb_talk_init(pl);
        lb_sys.step++;
        return;
    case 1:
        r = Lb_put_npc_default(pl);
        switch (r) {
        case 0:
        case 3:
            lb_sys.x68 = 0;
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            cnWrap_SoundRequest(3);
            if (Ud_item_num_ck3(npc->item) >= npc->num) {
                Ud_item_stack(npc->item, (s16)npc->num);
                set01_set(1, 0xD, (s16)npc->item);
                Event_flag_set(npc->flag);
            } else {
                set01_set2(lb_talk_tbl[1]);
            }
            lb_sys.step = 0;
            break;
        }
    }
}

#ifdef VERSION_MHG
void Omake_flag_set();
void Lb_reset_talk_count(void);   /* defined below; the Xbox clang rejects the implicit declaration */
int Omake_flag_ck();
void Lbc_SendMiniData();
extern char *lb_set01_msg[];
u8 Warehouse_search_space(void);
u8 Warehouse_equip_stack(UDW *, s8, s16, s16);
/* lobby 0x59D7B0: message `i` of the set01 list (the reward lines), shown for 90 ticks */
static void lb_msg_g(int i)
{
    if (i < 0x18) {
        set01_set2(lb_set01_msg[i]);
        *((u8 *)&lb_sys + 0x7D) = 0x5A;
    }
}
/* lobby 0x58DC30: the banner's prompt, drawn from its prim once the 90 ticks are over */
extern char *lb_banner_close_str;
void Lb_put_button(int, int, int);
static s16 banner_t = -1;
static void banner_prim(void)
{
    if (banner_t == 0) {
        flfntSetSize(0x12, 0x12);
        reload_tex(1, 0x180);
        SetTextureStage(0x180);
        Lb_put_button(0xBC, 0x132, 1);
        font_set_palette(0);
        flfntLocate(0xD2, 0x134);
        font_print(lb_banner_close_str);
    }
}
/* G's story events (lobby 0x544760, mode 49 on the PS2, 47 here): the elder and the village's story NPCs talk (the row chosen in
 * lb_talk_init by lb_sys+4) and the talk ends with the reward of that story point: zenny, items, stored equipment, event / extra
 * flags. The mode 48 banner (the hunter's registration card shown for 90 ticks, lobby 0x58DCC0) is shortened to its result. */
void Lb_event_story(void) {
    EMW *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    LB_NPCW *npc;
    s32 sub;
    int r, k;

    if (pl == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        NPCZoomInCameraCancel();
        cnWrap_SoundRequest(3);
        return;
    }
    npc = (LB_NPCW *)pl->ex;
    switch (lb_sys.step) {
    case 0:
        lb_talk_init(pl);
        lb_sys.step++;
        return;
    case 1:
        r = Lb_put_npc_default(pl);
        if (r != 0 && r != 3)
            return;
        sub = *(s32 *)((u8 *)&lb_sys + 4);
        switch (sub) {
        case 0:
            Gold_add(1500);
            cnWrap_SoundRequest(8);
            Lb_put_set01(2);
            Event_flag_set(0);
            Lb_reset_talk_count();
            break;
        case 1:
            if (Ud_item_num_ck3(0x1C1) > 0) {
                Ud_item_stack(0x1C1, 1);
                set01_set(1, 0xD, 0x1C1);
                Event_flag_set(0x50);
            } else {
                lb_msg_g(0x14);
            }
            if (Omake_flag_ck(8) == 0) {
                lb_msg_g(0x16);
                Omake_flag_set(8);
            }
            if (Omake_flag_ck(0xC) == 0) {
                lb_msg_g(0x15);
                Omake_flag_set(0xC);
            }
            Event_flag_set(0);      /* PORT CHOICE (a guess): G's asm sets event 0 only in event 0 (a kind 0 NPC, the town); without it the elder's
                                     * greeting would repeat for ever and the counter (mode 2, "event 0 set") would never open offline */
            break;
        case 2:
            if (Ud_item_num_ck3(0x1CD) > 0) {
                Ud_item_stack(0x1CD, 1);
                set01_set(1, 0xD, 0x1CD);
                Event_flag_set(0x52);
            }
            for (k = 2; k <= 0xC; k++)
                if (Omake_flag_ck(k) == 0) {
                    lb_msg_g(0x17);
                    for (k = 2; k <= 0xC; k++)
                        Omake_flag_set(k);
                    break;
                }
            break;
        case 3:
            if ((Warehouse_search_space() & 0xFF) == 0xFF) {       /* the box of stored equipment is full */
                Lb_put_set01(0xA);
            } else {
                Warehouse_equip_stack(User_data, 2, 0xEA, 0);
                Lb_put_set01(0x12);
                Event_flag_set(0x5A);
            }
            break;
        case 4:
            if (Ud_item_num_ck3(6) > 0) {
                Ud_item_stack(6, 1);
                set01_set(1, 0xD, 6);
                Event_flag_set(0x5B);
            } else {
                lb_msg_g(0x14);
            }
            break;
        case 5: {
            if (((u8 *)&User_data)[0xC23] == 0) {       /* sub_main 0x4C50B0: the registration is done once (User_data+0xC23) */
                ((u8 *)&User_data)[0xC23] = 1;
                Lbc_SendMiniData();
            }
            /* the banner of mode 48 (lobby 0x58DCC0): 90 ticks, then the "close" button until key 0x40. G draws the registration card
             * and its sparkles only on stage 0x4D (not the village), so in Kokoto the banner is just this wait and prompt */
            if (banner_t < 0) {
                banner_t = 0x5A;
            } else if (banner_t > 0) {
                banner_t--;
            }
            Lbc_set_prim(banner_prim, 0, 0);
            if (banner_t != 0 || !(Get_sw2(0) & 0x40))
                return;
            banner_t = -1;
            npc->x2E = 0;
            Event_flag_set(4);
            Lb_put_set01(0xF);
            lb_sys.x68 = 0;
            lb_sys.x6C = 0;
            NPCZoomInCameraCancel();
            lb_sys.x87 = 0x14;
            cnWrap_SoundRequest(3);
            *(s32 *)((u8 *)&lb_sys + 4) = 0;
            lb_sys.step = 0;
            return;
        }
        case 6:
            if (Ud_item_num_ck3(0x1BB) > 0) {
                Ud_item_stack(0x1BB, 1);
                set01_set(1, 0xD, 0x1BB);
                Event_flag_set(0x51);
                if (Omake_flag_ck(5) == 0) {
                    lb_msg_g(0x13);
                    Omake_flag_set(5);
                }
            } else {
                lb_msg_g(0x14);
            }
            break;
        case 7:
            Event_flag_set(0x59);
            if (Omake_flag_ck(8) == 0) {
                lb_msg_g(0x16);
                Omake_flag_set(8);
            }
            if (Omake_flag_ck(0xC) == 0) {
                lb_msg_g(0x15);
                Omake_flag_set(0xC);
            }
            break;
        case 8:
            if (Ud_item_num_ck3(0x1BC) >= 5) {
                Ud_item_stack(0x1BC, 5);
                set01_set(1, 0xD, 0x1BC);
                Event_flag_set(0x5D);
            } else {
                lb_msg_g(0x14);
            }
            break;
        }
        npc->x2E = 0;
        lb_sys.x68 = 0;
        *(s32 *)((u8 *)&lb_sys + 4) = 0;
        NPCZoomInCameraCancel();
        lb_sys.x6C = 0;
        lb_sys.x87 = 0x14;
        cnWrap_SoundRequest(3);
        lb_sys.step = 0;
        return;
    }
}
#endif

void Lb_event_localLv5End(void) {
    void *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    int r;

    if (pl == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        NPCZoomInCameraCancel();
        cnWrap_SoundRequest(3);
        return;
    }
    switch (lb_sys.step) {
    case 0:
        lb_talk_init(pl);
        lb_sys.step++;
        return;
    case 1:
        r = Lb_put_npc_default(pl);
        switch (r) {
        case 0:
        case 3:
            lb_sys.x68 = 0;
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            cnWrap_SoundRequest(3);
            Event_flag_set(0x50);
            if (Ud_item_num_ck3(0xB1) > 0) {
                Ud_item_stack(0xB1, 1);
                set01_set(1, 0xD, 0xB1);
                Event_flag_set(0x4D);
            } else {
                set01_set2(lb_talk_tbl[1]);
            }
            if (Omake_flag_ck(8) == 0) {
                set01_set2(lb_talk_tbl[2]);
                Omake_flag_set(8);
            }
            lb_sys.step = 0;
            break;
        }
    }
}

void Lb_event_guildstart(void) {
    void *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    int r;

    if (pl == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        NPCZoomInCameraCancel();
        cnWrap_SoundRequest(3);
        return;
    }
    switch (lb_sys.step) {
    case 0:
        lb_talk_init(pl);
        lb_sys.step++;
        return;
    case 1:
        r = Lb_put_npc_default(pl);
        switch (r) {
        case 0:
        case 3:
            lb_sys.x68 = 0;
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            cnWrap_SoundRequest(3);
            Event_flag_set(4);
            Lb_put_set01(0xF);
            lb_sys.step = 0;
            break;
        }
    }
}

void Lb_event_guild(void) {
    void *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    int r;

    if (pl == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        NPCZoomInCameraCancel();
        cnWrap_SoundRequest(3);
        return;
    }
    switch (lb_sys.step) {
    case 0:
        lb_talk_init(pl);
        lb_sys.step++;
        return;
    case 1:
        r = Lb_put_npc_default(pl);
        switch (r) {
        case 0:
        case 3:
            lb_sys.x68 = 0;
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            cnWrap_SoundRequest(3);
            if (Ud_item_num_ck3(0xAD) >= 5) {
                Ud_item_stack(0xAD, 5);
                set01_set(1, 0xD, 0xAD);
                Event_flag_set(0x4F);
            } else {
                set01_set2(lb_talk_tbl[1]);
            }
            lb_sys.step = 0;
            break;
        }
    }
}

void Lb_event_localLast(void) {
    void *pl = player_work[*(u8 *)0x3F34C1].x3B0;
    int r;

    if (pl == 0) {
        lb_sys.x68 = 0;
        lb_sys.x6C = 0;
        NPCZoomInCameraCancel();
        cnWrap_SoundRequest(3);
        return;
    }
    switch (lb_sys.step) {
    case 0:
        lb_talk_init(pl);
        lb_sys.step++;
        return;
    case 1:
        r = Lb_put_npc_default(pl);
        switch (r) {
        case 0:
        case 3:
            lb_sys.x68 = 0;
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            cnWrap_SoundRequest(3);
            lb_sys.step = 0;
            Event_flag_set(0x52);
            if (Omake_flag_ck(2) == 0 || Omake_flag_ck(3) == 0 || Omake_flag_ck(4) == 0 ||
                Omake_flag_ck(5) == 0 || Omake_flag_ck(6) == 0 || Omake_flag_ck(7) == 0 ||
                Omake_flag_ck(8) == 0) {
                set01_set2(lb_talk_tbl[3]);
                Omake_flag_set(2);
                Omake_flag_set(3);
                Omake_flag_set(4);
                Omake_flag_set(5);
                Omake_flag_set(6);
                Omake_flag_set(7);
                Omake_flag_set(8);
            }
            break;
        }
    }
}

void lb_talk_setLocalShop(void) {
    if (Event_flag_ck(0x32) == 0) {
        Event_flag_set(0x32);
        lb_pit.pos += 0x28;
    }
}

void lb_talk_setGondola(void) {
    u32 v = *(u32 *)(cw + CW_OFS(0xBF3C));

    if (v >= 4) {
        lb_pit.x08 = 0x10;
    } else {
        switch (v & 3) {
        case 1:
            lb_pit.x08 = 4;
            break;
        case 2:
            lb_pit.x08 = 8;
            break;
        case 3:
            lb_pit.x08 = 0xC;
            break;
        default:
        case 0:
            lb_pit.x08 = 0;
            break;
        }
    }
    lb_pit.x0 = 0;
    lb_pit.pos += lb_pit.x08 * 8;
}

typedef struct LB_NPCTALK {
    s32 id;             /* 0x00 talk handler id (0 = end of table) */
    s32 kind;           /* 0x04 npc kind it applies to */
} LB_NPCTALK;

extern LB_NPCTALK lb_npc_talk_tbl[];
extern u8 *npc_dialog_table[];

#ifdef VERSION_MHG
#define lbshop_raw ((u8 *)&lbShop)   /* lbShop (LB_SHOP) as bytes; not an asm label: __asm__ skips the "_" of win32 (Xbox) names */
/* G: the talk rows of the shop NPCs depend on the story: the first block whose condition holds (a story level or a talk event of
 * ck_talkEvent) and whose first flag is not set yet sets its flags (the lines already heard) and picks the row (lobby 0x544CD0,
 * 0x544EF0, 0x545010, 0x545180: local shop 0x38, kind 0x2E, kind 1, kind 2). */
typedef struct GTALK { u8 lvl; u8 arg; u8 row; u8 n; u8 flags[6]; } GTALK;
static int gtalk_pick(const GTALK *t, int n)
{
    int i, k;
    for (i = 0; i < n; i++) {
        int c = t[i].lvl ? get_questLevelNum(t[i].arg) : ck_talkEvent(t[i].arg);
        if (c == 1 && Event_flag_ck(t[i].flags[0]) == 0) {
            for (k = 0; k < t[i].n; k++)
                Event_flag_set(t[i].flags[k]);
            return t[i].row;
        }
    }
    return 0;
}
static const GTALK gt_localshop[5] = {
    { 0, 17, 15, 5, { 95, 94, 86, 87, 88 } }, { 0, 16, 18, 4, { 94, 86, 87, 88 } }, { 0, 14, 21, 3, { 86, 87, 88 } },
    { 0, 13, 12, 2, { 87, 88 } }, { 0, 12, 9, 1, { 88 } } };
static const GTALK gt_kind2E[3] = { { 0, 17, 12, 3, { 34, 33, 32 } }, { 0, 16, 9, 2, { 33, 32 } }, { 0, 14, 6, 1, { 32 } } };
static const GTALK gt_kind1[4] = {
    { 0, 5, 11, 4, { 96, 83, 84, 85 } }, { 0, 3, 14, 3, { 83, 84, 85 } }, { 0, 2, 8, 2, { 84, 85 } }, { 0, 1, 4, 1, { 85 } } };
static const GTALK gt_kind2[6] = {
    { 1, 34, 26, 6, { 42, 41, 40, 39, 38, 37 } }, { 0, 6, 22, 5, { 41, 40, 39, 38, 37 } }, { 0, 5, 18, 4, { 40, 39, 38, 37 } },
    { 0, 4, 14, 3, { 39, 38, 37 } }, { 0, 2, 10, 2, { 38, 37 } }, { 0, 1, 6, 1, { 37 } } };
/* G's town level (cw+0x11AB0, MH1 cw+0xBF3C: the gondola / room level) tests of lobby 0x588B70 / 0x588B90 */
static int lobby_func_00588B70(void) { return *(u32 *)(cw + 0x11AB0) >= 4; }
static int lobby_func_00588B90(int kind)
{
    u32 v = *(u32 *)(cw + 0x11AB0);
    return kind == 3 ? v == 2 : kind == 5 ? v == 3 : kind == 0x4C ? v == 1 : 0;
}

void lb_talk_init(EMW *pl) {
    LB_NPCW *npc = (LB_NPCW *)pl->ex;
    LB_NPCTALK *t = lb_npc_talk_tbl;
    int i, row;
    s32 sub = *(s32 *)((u8 *)&lb_sys + 4);      /* G lb_sys+4: the story event number (mode 47 here = G's 49) */
    int story = lb_sys.x68 == 47;

    lb_pit.x0 = 0;
    lb_pit.x09 = 0;
    lb_pit.x08 = 0;
    lb_pit.pos = npc_dialog_table[npc->kind];
    cnWrap_SoundRequest(0xC);
#define ROW(r) do { lb_pit.x08 = (r); lb_pit.pos += lb_pit.x08 * 8; } while (0)
    switch (npc->kind) {
    case 0x56:
        lb_pit.x08 = *(u8 *)(cw + 0xFA64);
        lb_pit.x0 = 0;
        lb_pit.pos += lb_pit.x08 * 8;
        break;
    case 4:
        lb_talk_setGHRcpt(npc);
        break;
    case 0x38:
        if (story && sub == 5) { lb_pit.x0 = 0; lb_pit.x08 = 6; } else lb_pit.x08 = gtalk_pick(gt_localshop, 5);
        lb_pit.pos += lb_pit.x08 * 8;
        break;
    case 1:
        ROW(gtalk_pick(gt_kind1, 4));
        break;
    case 2:
        ROW(gtalk_pick(gt_kind2, 6));
        break;
    case 0x2E:
        ROW(gtalk_pick(gt_kind2E, 3));
        break;
    case 5:
        if (story && sub == 7) { ROW(6); return; }
        row = lobby_func_00588B70() == 1 ? 4 : lobby_func_00588B90(npc->kind) == 1 ? 5 : lb_pit.x08;
        ROW(row);
        break;
    case 3:
    case 0x4C:
        row = lobby_func_00588B70() == 1 ? 5 : lobby_func_00588B90(npc->kind) == 1 ? 4 : 0;
        ROW(row);
        break;
    case 0x17:
        lb_talk_setGondola();
        break;
    case 0x49:
        if (story && sub == 5) { ROW(5); return; }
        ROW(lobby_func_00588B70() == 1 ? 4 : 0);
        break;
    case 0x47:
        ROW(sub == 1 ? 12 : sub == 2 ? 26 : sub == 3 ? 31 : sub == 4 ? 24 : 0);
        break;
    case 0:
        lb_pit.x0 = 0;
        ROW(13);
        break;
    case 6:
        ROW(sub == 0 ? 13 : sub == 8 ? 17 : 0);
        break;
    case 0x48:
        if (story) {
            ROW(sub == 4 ? 79 : sub == 9 ? 70 : 0);
            return;
        }
        /* fall through */
    default:
        i = 0;
        do {
            s32 id = t->id;
            if (id == 0)
                break;
            if (t->kind == npc->kind) {
                lb_talk_setDefault(npc, (LB_TALKREC *)id, i);
                break;
            }
            i++;
            t++;
        } while (i < 100);
        break;
    }
    if (npc->x2E == 0) npc->x2E++;
#undef ROW
}
#else
void lb_talk_init(EMW *pl) {
    LB_NPCW *npc = (LB_NPCW *)pl->ex;
    LB_NPCTALK *t = lb_npc_talk_tbl;
    int i;

    lb_pit.x0 = 0;
    lb_pit.x09 = 0;
    lb_pit.x08 = 0;
    lb_pit.pos = npc_dialog_table[npc->kind];
    cnWrap_SoundRequest(0xC);
    switch (npc->kind) {
    case 0x56:
        lb_pit.x08 = *(u8 *)(cw + CW_OFS(0x35D8));
        lb_pit.x0 = 0;
        lb_pit.pos += lb_pit.x08 * 8;
        break;
    case 4:
        lb_talk_setGHRcpt(npc);
        break;
    case 0x38:
        lb_talk_setLocalShop();
        break;
    case 5:
        lb_pit.x0 = 0;
        lb_pit.x08 = 7;
        lb_pit.pos += lb_pit.x08 * 8;
        break;
    case 0x17:
        lb_talk_setGondola();
        break;
    case 0x47:
        if (lb_sys.x68 == 0x31) lb_pit.x08 = 0x1F;
        else lb_pit.x08 = 0x1A;
        lb_pit.x0 = 0;
        lb_pit.pos += lb_pit.x08 * 8;
        break;
    case 0:
        lb_pit.x0 = 0;
        lb_pit.x08 = 0xD;
        lb_pit.pos += lb_pit.x08 * 8;
        break;
    case 0x48:
        if (Event_flag_ck(4) == 0) {
            lb_sys.x68 = 0x2F;
            return;
        }
        if (Event_flag_ck(0x4E) == 1 && Event_flag_ck(0x4F) == 0) {
            lb_pit.x0 = 0;
            lb_sys.x68 = 0x2D;
            lb_pit.x08 = 0x3D;
            lb_pit.pos += lb_pit.x08 * 8;
            return;
        }
    default:
        i = 0;
        do {
            s32 id = t->id;
            if (id != 0) {
                if (id != 0 && t->kind == npc->kind) {
                    lb_talk_setDefault(npc, (LB_TALKREC *)id, i);
                } else {
                    i++;
                    t++;
                    continue;
                }
            }
            break;
        } while (i < 100);
        break;
    }
    if (npc->x2E == 0) npc->x2E++;
}

#endif

void Lb_reset_talk_count(void) {
    EMW *em = em_work;
    s16 i;

    for (i = 0; i < 20; i++, em++) {
        if (em->be_flag != 0) ((LB_NPCW *)em->ex)->x2E = 0;
    }
}

#ifdef __MWERKS__
int Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
int NPC_Message();

typedef struct LB_TALKENT {
    u16 type;           /* 0x00 entry type (2 = text then advance) */
    u8 _pad02[2];
    s32 text;           /* 0x04 message id */
} LB_TALKENT;

/* draws the current talk entry; returns 2 while talking, 0 when finished */
int Lb_put_npc_default(void) {
    u16 sw = Get_sw2(0);
    LB_TALKENT *e;

    if (lb_pit.x0 >= 0) lb_pit.x0++;
    e = (LB_TALKENT *)lb_pit.pos;
    if (NPC_Message(e->text, lb_pit.x0, e->type, lb_pit.x09) == 0) {
        if (sw & 0x60) {
            e = (LB_TALKENT *)lb_pit.pos;
            if (e->type == 2) {
                lb_pit.x0 = 0;
                lb_pit.pos += 8;
                lb_pit.x09 = 0;
                lb_pit.x08++;
            } else {
                return 0;
            }
        }
    } else if (sw & 0x60) {
        lb_pit.x0 = -1;
    }
    return 2;
}
