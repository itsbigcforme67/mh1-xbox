/* Lobby: commer/trade/status receivers (SLPM_654.95 lobby overlay 0x5C4E60-0x5C5E80). Whole file; matching runs split into lb_aNN.c */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];

void lb_commer_message(s8 id, u8 *src) {
    u8 buf[0x120];
    char *name;
    if (CW8(0x35D5) != 0) {
        name = lbCommer[id].name;
        memcpy(name, src, 0x10);
        memcpy(CWPLAYER(id) + 0x1334, src, 0x10);
        memcpy(&lb_player[id].x04, src, 0x10);
        memset(buf, 0, 0x120);
        buf[0x11F] = 6;
        buf[0x11E] = 6;
        buf[0x11D] = 6;
        if (name != 0) {
            sprintf((char *)buf + 0x1C, lit_238_0065ECF0, name);
            Chat_log_add(0, buf);
        }
    }
}

void lb_trade_start(s8 id, u8 *data) {
    PLW *pl = &player_work[id];
    LBTRADE t;
    if (pl->be_flag != 0) {
        memcpy(&t, data, 0xE);
        pl->work909 = Lb_get_plID(t.plid);
        pl->work904 = t.item;
        pl->work906 = t.num;
        Lb_Pl_act_set2(pl, 0, 0x2D, 0x10);
    }
}

void lb_trade_check(int a0, u8 *data) {
    PLW *pl = &player_work[game_w.master];
    LBTRADE t;
    memcpy(&t, data, 0xE);
    if ((s16)act_ck(pl, 0, 0x2D) != 0 && pl->work904 == t.item && (s16)Pl_item_num_ck(pl, t.item) >= t.num) {
        Ud_item_stack(t.item, -t.num);
        Lb_send_item_result(a0, 0);
        pl->work90A = 0xFF;
        return;
    }
    Lb_send_item_result(a0, 1);
}

void lb_trade_result(u8 *data) {
    PLW *pl = &player_work[game_w.master];
    LBTRADE2 t;
    void Ud_item_stack();
    if ((s16)act_ck(pl, 0, 0x2E) != 0) {
        memcpy(&t, data, 0xE);
        if (t.result == 0) {
            pl->work90A = 1;
            Ud_item_stack(t.item, t.num);
            set01_set(1, 0xD, (s16)t.item);
            return;
        }
        pl->work90A = 2;
    }
}

int lb_check_mini_data(int a0, int a1, u8 *mini) {
    int ret = 0;
    s8 id = a0;
    u8 *m = CWPLAYER(id) + 0x1346;
    if (memcmp(m + 0xE, mini + 0xE, 6) != 0) {
        if ((s8)cw[id + 0x2BFE] == 0) {
            Lb_set_mini_data_to_pl(a0, mini);
            Lb_set_player(a0 & 0xFF, a1, CWPLAYER(id) + 0x1334);
        } else {
            Lb_player_release(&player_work[id]);
            Lb_set_mini_data_to_pl(a0, mini);
            Lb_set_player(a0 & 0xFF, a1, CWPLAYER(id) + 0x1334);
            ret = 1;
        }
        cw[id + 0x2BFE] = 0;
    } else if (memcmp(m + 8, mini + 8, 6) != 0) {
        Lb_set_mini_data_to_pl(a0, mini);
    }
    memcpy(m, mini, 0x40);
    if (id == game_w.master) {
        memcpy(my_user_mini_data, mini, 0x18);
    }
    return ret;
}

void Lb_chat_receipt(u8 *msg) {
    Chat_log_add(Lb_get_plID() & 0xFF, msg);
}

int Lb_get_plID(u8 *mac) {
    int i = 0;
    LBCOMMER *c = lbCommer;
    do {
        if (memcmp(c, mac, 6) == 0) return i;
        i = (i + 1) & 0xFF;
        c++;
    } while (i < 8);
    return 0xFF;
}

void lb_set_pl_status(u8 id, u8 *src) {
    u8 buf[0x10];
    LBSTAT *st;
    PLW *pl;
    pl = &player_work[id];
    flMemcpy(buf, src, 0x10);
    st = (LBSTAT *)buf;
    pl->work800 = st->x;
    pl->work808 = st->z;
    pl->flag14 = st->act14;
    pl->flag15 = st->act15;
    pl->ang_y = st->ang;
    PLU8(pl, 0x8EC) = st->x0D;
    Lb_Pl_adj_calc(pl, 0xA);
    if (st->act15 == 0x4C && pl->flag14 == 0 && st->act15 != pl->flag15) {
        Lb_Pl_act_set2(pl, 0, 0x4C, 0);
        return;
    }
    switch (st->act15) {
    case 0x4D:
        lb_sys.chair_mask = lb_sys.chair_mask & ~(1 << st->chair);
        break;
    case 0x4C:
    case 0x54:
    case 0x59:
    case 0x5A:
    case 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x60:
    case 0x4E:
    case 0x2A:
    case 0x29:
    case 0x2B:
        if (st->chair > 0 && st->chair < 0xB) {
            lb_sys.chair_mask = lb_sys.chair_mask | (1 << st->chair);
        }
        break;
    }
    Lb_act_set(pl, st->act14, st->act15);
}

void lb_set_pl_pos(u8 id, u8 *src, u8 mode) {
    u8 buf[0xC];
    LBPOS *p;
    PLW *pl = &player_work[id];
    flMemcpy(buf, src, 0xC);
    p = (LBPOS *)buf;
    if (mode == 0) {
        pl->work800 = p->x;
        pl->work808 = p->z;
        pl->ang_y = p->ang;
        if (pl->stg != game_w.stage) {
            *(LBV3 *)pl->pos = *(LBV3 *)&pl->work800;
            pl->ang[1] = *(u16 *)&pl->ang_y;
        } else {
            Lb_Pl_adj_calc(pl, 0xA);
        }
        pl->stg = p->stg;
    } else if (mode == 2) {
        pl->work800 = p->x;
        pl->work808 = p->z;
        *(LBV3 *)pl->pos = *(LBV3 *)&pl->work800;
        *(LBV3 *)&pl->work5A0 = *(LBV3 *)pl->pos;
        pl->ang_y = p->ang;
        pl->ang[1] = p->ang;
    }
}

void lb_set_pl_stage(s8 id, u8 *stg) {
    char buf[0x120];
    u8 uid = id;
    PLW *pl = &player_work[uid];
    PLW *me;
    pl->stg = *stg;
    PLU8(pl, 0x8EC) = 0;
    me = &player_work[game_w.master];
    PLU8(pl, 0x90F) = 0;
    if (me->x3B0 != 0 && ((u16 *)me->x3B0)[6] == uid && ((u8 *)me->x3B0)[0x1E] == 0) {
        me->x3B0 = 0;
    }
    if (*stg == 0) {
        if (CW8(0x35D5) != 0) {
            if (lbCommer[uid].name[0] != 0) {
                memset(buf, 0, 0x120);
                buf[0x11F] = 6;
                buf[0x11E] = 6;
                buf[0x11D] = 6;
                sprintf(buf + 0x1C, lit_476_0065ED10, lbCommer[uid].name);
                Chat_log_add(0, buf);
            }
        }
        Lb_clearChatMember(id);
    }
}

void lb_check_chair(int a0, u8 *p) {
    if (memcmp(cw + 0x440, cw + 3, 8) == 0) {
        if ((1 << *p) & lb_sys.chair_mask) {
            Lb_send_chair_status(a0, 0, *p);
            return;
        }
        Lb_send_chair_status(a0, 1, *p);
    }
}

void lb_set_chair(int a0, u8 *p) {
    u8 me;
    if (p[0] == 0) {
        me = game_w.master;
        if (me == (Lb_get_plID(p + 2) & 0xFF) && lb_sys.x68 == 0x18) {
            lb_sys.x68 = 0;
            lb_sys.x6C = 0;
        }
        return;
    }
    lb_sys.chair_mask = lb_sys.chair_mask | (1 << p[1]);
    me = game_w.master;
    if (me == (Lb_get_plID(p + 2) & 0xFF)) {
        if (game_w.stage == 0x4D && lb_sys.x68 == 0x18) {
            Lb_pl_to_chair();
            return;
        }
        Lb_send_chair_release(&player_work[me]);
    }
}

void lb_recv_myChair(u8 *p) {
    lb_sys.chair_mask = lb_sys.chair_mask | (1 << *p);
}

void lb_send_my_status(int a0) {
    Lb_send_stage();
    if (game_w.stage == 0x4D || game_w.stage == 0x4C) {
        Lb_send_pl_status(&player_work[game_w.master]);
        if (D_3E54FB[game_w.master * 0xA00] != 0) {
            Lb_send_trade_startTU(&player_work[game_w.master], a0);
        }
    }
}

void lb_chidori_off(s8 id) {
    PLW *pl = &player_work[id];
    PLU8(pl, 0x8EC) = 0;
    PLU8(pl, 0x90F) = 0;
}
