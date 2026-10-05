/* lb_talk - lobby.bin 0x00533A00-0x00535100. NPC talk start in the lobby
 * town (Lb_talk) and the talk-script chooser helpers. */
#include "lobby.h"
#include "pl.h"
#include "ud.h"
#include "em.h"

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
    u32 v = *(u32 *)(cw + 0xBF3C);

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
        lb_pit.x08 = *(u8 *)(cw + 0x35D8);
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

void Lb_reset_talk_count(void) {
    EMW *em = em_work;
    s16 i;

    for (i = 0; i < 20; i++, em++) {
        if (em->be_flag != 0) ((LB_NPCW *)em->ex)->x2E = 0;
    }
}

int Get_sw2();
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
