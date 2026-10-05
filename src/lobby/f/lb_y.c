/* Lobby: guild quest table generation (key quests, flag quests, per-level quest lists), hand-written from m2c drafts. */
#include "lobby_f.h"
extern u8 *flag_quest_tbl[];
extern u8 *flag_quest_tbl_local[];
extern u8 *quest_lv_tbl[];
extern u8 *quest_local_tbl[];
extern u8 lb_quest_clear[];
extern u8 lb_quest_info[];
extern u8 key_quest;
extern s8 key_quest_num;
extern u8 User_data[];
int Quest_clear_bit_ck();
int lb_key_quest_ck();
int Ex_quest_ck();
int Event_flag_ck();
void Event_flag_set();
int lb_get_quest_level();
u8 get_new_quest();
u8 get_flag_quest();
int lb_guild_check_keyQuest();
int Lb_guild_check_requireF();
s8 lb_set_key_quest_local(void);
u8 get_flag_quest(a)
int a;
{
    u8 *tbl;
    int n;
    int i;
    int k;
    u8 *e;
    int v;
    if (a == -1) {
        return 0;
    }
    if (Online_ck() == 1) {
        tbl = flag_quest_tbl[a];
    } else {
        tbl = flag_quest_tbl_local[a];
    }
    v = 0;
    if (*tbl != 0) {
        do {
            v = (v + 1) & 0xFF;
        } while (tbl[v] != 0);
    }
    n = v & 0xFF;
    i = (ran_suu(1) & 0xFFFF) % n;
    k = 0;
    i = i & 0xFF;
    if (n > 0) {
        do {
            v = i & 0xFF;
            if (v < 0x67 || v >= 0x6B) {
                e = tbl + (i & 0xFF);
                if (Quest_clear_bit_ck(*e) == 0) {
                    if (*e == 0x90) {
                        if (Quest_clear_bit_ck(0x94) == 1) {
                            return *e;
                        }
                    } else {
                        return *e;
                    }
                }
            }
            i = (i + 1) & 0xFF;
            if (i >= n) {
                i = 0;
            }
            k = (k + 1) & 0xFF;
        } while (k < n);
    }
    return 0;
}
u8 get_new_quest(a)
int a;
{
    u8 *tbl;
    int n;
    int i;
    int k;
    u8 *e;
    int v;
    if (a == -1) {
        return 0;
    }
    if (Online_ck() == 1) {
        tbl = quest_lv_tbl[a];
    } else {
        tbl = quest_local_tbl[a];
    }
    v = 0;
    if (*tbl != 0) {
        do {
            v = (v + 1) & 0xFF;
        } while (tbl[v] != 0);
    }
    n = v & 0xFF;
    i = (ran_suu(1) & 0xFFFF) % n;
    k = 0;
    i = i & 0xFF;
    if (n > 0) {
        do {
            e = tbl + (i & 0xFF);
            if (Quest_clear_bit_ck(*e) == 0 && (Online_ck() == 0 || *e < 0x83) && lb_key_quest_ck(*e) == 0) {
                if (*e == 0x90) {
                    if (Quest_clear_bit_ck(0x94) == 1) {
                        return *e;
                    }
                } else {
                    return *e;
                }
            }
            i = (i + 1) & 0xFF;
            if (i >= n) {
                i = 0;
            }
            k = (k + 1) & 0xFF;
        } while (k < n);
    }
    return 0;
}
int lb_guild_check_keyQuest(u32 a) {
    switch (a) {
    case 0:
        if (*(u8 *)0x3C733B >= 4) {
            return 1;
        }
        break;
    case 1:
        if (*(u8 *)0x3C733B >= 8) {
            return 1;
        }
        break;
    case 2:
        if (*(u8 *)0x3C733B >= 0xC) {
            return 1;
        }
        break;
    case 3:
        if (*(u8 *)0x3C733B >= 0x10) {
            return 1;
        }
        break;
    case 4:
        if (*(u8 *)0x3C733B >= 0x12) {
            return 1;
        }
        break;
    case 5:
        if (*(u8 *)0x3C733B >= 0x13) {
            return 1;
        }
        break;
    default:
        return 1;
    }
    return 0;
}
s8 lb_set_key_quest_local(void) {
    s8 n;
    if (Event_flag_ck(0x4C) == 0) {
        return 0;
    }
    n = 0;
    if (Quest_clear_bit_ck(0xAF) == 1 && Quest_clear_bit_ck(0xAE) == 1 && Quest_clear_bit_ck(0xAD) == 1 && Quest_clear_bit_ck(0xAC) == 1) {
        n = 1;
        lb_quest_info[0x19] = 0xAA;
    }
    if (Ex_quest_ck(User_data, 2) == 1) {
        s8 *p = (s8 *)lb_quest_info + 0x19 + n;
        n = n + 1;
        *p = 0xAF;
    }
    if (Ex_quest_ck(User_data, 1) == 1) {
        s8 *p = (s8 *)lb_quest_info + 0x19 + n;
        n = n + 1;
        *p = 0xAE;
    }
    if (Ex_quest_ck(User_data, 0) == 1) {
        s8 *p = (s8 *)lb_quest_info + 0x19 + n;
        n = n + 1;
        *p = 0xAD;
    }
    if (Quest_clear_bit_ck(0xAD) == 1) {
        s8 *p = (s8 *)lb_quest_info + 0x19 + n;
        n = n + 1;
        *p = 0xAC;
    }
    return n;
}
void Lb_make_quest_tbl_local(void) {
    u32 mask;                       /* never initialised in the original (stale stack) */
    int lvl;
    int has;
    u8 flagq;
    u8 cleared;
    u8 **tp;
    u8 *cp;
    u8 *row;
    u8 *src;
    int i;
    int j;
    int k;
    int cnt;
    u8 nq;
    u8 *e;
    u8 *t;
    u8 key;
    key_quest = 0;
    key_quest_num = 0;
    has = 0;
    flagq = 0;
    cleared = 0;
    lvl = (s8)lb_get_quest_level(1);
    for (i = 0, tp = quest_local_tbl, cp = lb_quest_clear, row = lb_quest_info; i < 5; i++, tp++, cp++, row += 5) {
        src = *tp;
        *cp = 0;
        row[0] = src[0];
        row[1] = src[1];
        row[2] = src[2];
        row[3] = src[3];
        row[4] = src[4];
    }
    lb_quest_info[0x19] = 0;
    lb_quest_info[0x1A] = 0;
    lb_quest_info[0x1B] = 0;
    lb_quest_info[0x1C] = 0;
    lb_quest_info[0x1D] = 0;
    if (lvl == -1) {
        key_quest = 0x83;
    } else {
        tp = quest_local_tbl;
        cp = lb_quest_clear;
        row = lb_quest_info;
        for (i = 0; i < 5; i++, tp++, cp++, row += 5) {
            src = *tp;
            nq = get_new_quest(i);
            if (nq == 0) {
                *cp = 1;
            }
            if (lvl == 5) {
                key_quest = 0xAA;
            } else if (lvl == i) {
                has = 1;
                flagq = get_flag_quest(lvl);
                if (flagq == 0) {
                    has = 0;
                    switch (lvl) {
                    case 0:
                        key_quest = 0x88;
                        break;
                    case 1:
                        key_quest = 0x89;
                        break;
                    case 2:
                        key_quest = 0x9A;
                        break;
                    case 3:
                        key_quest = 0x8B;
                        break;
                    case 4:
                        key_quest = 0xAB;
                        break;
                    }
                } else if (i != 0) {
                    j = 0;
                    if (i != 1) {
                        do {
                            t = row + j;
                            if (flagq == *t) {
                                *t = row[4];
                                row[4] = flagq;
                            } else {
                                j += 1;
                                if (j < 5) {
                                    continue;
                                }
                            }
                            break;
                        } while (1);
                        if (j == 5) {
                            row[4] = flagq;
                        }
                    }
                }
            }
            if (nq != 0) {
                if (i != 0 && i != 1) {
                    has = 1;
                    j = 0;
                    do {
                        t = row + j;
                        if (nq == *t) {
                            *t = row[0];
                            row[0] = nq;
                        } else {
                            j += 1;
                            if (j < 5) {
                                continue;
                            }
                        }
                        break;
                    } while (1);
                    if (j == 5) {
                        row[0] = nq;
                    }
                }
            } else {
                cleared += 1;
            }
            if (i != 0 && i != 1) {
                k = 5;
                if (src[5] != 0) {
                    cnt = 5 - has;
                    do {
                        if (mask == 0) {
                            mask = ran_suu(1) & 0xFFFF;
                        }
                        e = src + k;
                        if ((*e != 0xAB || Quest_clear_bit_ck(0xAB) == 1) && ((mask >> (k % 32)) & 1)) {
                            u8 c = *e;
                            if (flagq != c && nq != c) {
                                int h = (u32)(ran_suu(1) & 0xFFFF) % cnt;
                                u8 cc = *e;
                                int pos = has + h;
                                if (cc != 0x90 || Quest_clear_bit_ck(0x94) == 1) {
                                    t = row + pos;
                                    if (flagq != *t && (pos != 0 || nq == 0)) {
                                        *t = *e;
                                    }
                                }
                            }
                        }
                        k += 1;
                    } while (src[k] != 0);
                }
            }
        }
    }
    if (cleared == 5 && Event_flag_ck(0x4C) == 0) {
        Event_flag_set(0x4C);
    }
    key = key_quest;
    if (key == 0xAA) {
        key_quest_num = lb_set_key_quest_local();
        return;
    }
    if (key != 0) {
        lb_quest_info[0x19] = key;
        key_quest_num = 1;
    }
}
void Lb_make_quest_tbl(void) {
    u32 lvl;
    int has;
    u8 flagq;
    u8 cleared;
    u8 **tp;
    u8 *cp;
    u8 *row;
    u8 *src;
    int i;
    int j;
    int k;
    int cnt;
    u8 nq;
    u8 *e;
    u8 *t;
    u8 key;
    s8 kn;
    flagq = 0;
    cleared = 0;
    key_quest = 0;
    key_quest_num = 0;
    lvl = (s8)lb_get_quest_level(1);
    for (i = 0, tp = quest_lv_tbl, cp = lb_quest_clear, row = lb_quest_info; i < 6; i++, tp++, cp++, row += 5) {
        src = *tp;
        *cp = 0;
        row[0] = src[0];
        row[1] = src[1];
        row[2] = src[2];
        row[3] = src[3];
        row[4] = src[4];
    }
    lb_quest_info[0x1E] = 0;
    lb_quest_info[0x23] = 0;
    lb_quest_info[0x1F] = 0;
    lb_quest_info[0x24] = 0;
    lb_quest_info[0x20] = 0;
    lb_quest_info[0x25] = 0;
    lb_quest_info[0x21] = 0;
    lb_quest_info[0x26] = 0;
    lb_quest_info[0x22] = 0;
    lb_quest_info[0x27] = 0;
    tp = quest_lv_tbl;
    cp = lb_quest_clear;
    row = lb_quest_info;
    for (i = 0; i < 6; i++, tp++, cp++, row += 5) {
        src = *tp;
        nq = get_new_quest(i);
        if (nq == 0) {
            *cp = 1;
            cleared = (cleared + 1) & 0xFF;
        }
        if (cleared == 6 && Event_flag_ck(0x4E) == 0) {
            Event_flag_set(0x4E);
        }
        if (lvl == i) {
            flagq = get_flag_quest(lvl);
            if (flagq == 0 && lb_guild_check_keyQuest(lvl) == 1) {
                switch (lvl) {
                case 0:
                    if (Quest_clear_bit_ck(0x27) == 0) {
                        key_quest = 0x27;
                    }
                    break;
                case 1:
                    if (Quest_clear_bit_ck(9) == 0) {
                        key_quest = 9;
                    }
                    break;
                case 2:
                    if (Quest_clear_bit_ck(0x65) == 0) {
                        key_quest = 0x65;
                    }
                    break;
                case 3:
                    if (Quest_clear_bit_ck(0x4F) == 0) {
                        key_quest = 0x4F;
                    }
                    break;
                case 4:
                    if (Quest_clear_bit_ck(0x61) == 0) {
                        key_quest = 0x61;
                    }
                    break;
                case 5:
                    if (Lb_guild_check_requireF() == 1) {
                        key_quest = 0x67;
                    } else {
                        key_quest = 0x6B;
                    }
                    break;
                }
            } else {
                j = 0;
                do {
                    t = row + j;
                    if (flagq == *t) {
                        *t = row[4];
                        row[4] = flagq;
                    } else {
                        j += 1;
                        if (j < 5) {
                            continue;
                        }
                    }
                    break;
                } while (1);
                if (j == 5 && flagq != 0) {
                    row[4] = flagq;
                }
            }
        }
        if (nq != 0) {
            j = 0;
            has = 1;
            do {
                t = row + j;
                if (nq == *t) {
                    *t = row[0];
                    row[0] = nq;
                } else {
                    j += 1;
                    if (j < 5) {
                        continue;
                    }
                }
                break;
            } while (1);
            if (j == 5 && nq != 0) {
                row[0] = nq;
            }
        } else {
            has = 0;
        }
        if (nq == 0 && i == 0 && Event_flag_ck(0x33) == 0) {
            Event_flag_set(0x33);
        }
        if (i == 0) {
            u8 pick = (!((ran_suu(1) & 0xFFFF) & 1) ? 3 : 1) & 0xFF;
            j = 1;
            has += 1;
            do {
                t = row + j;
                if (pick == *t) {
                    *t = row[0];
                    row[0] = pick;
                } else {
                    j += 1;
                    if (j < 5) {
                        continue;
                    }
                }
                break;
            } while (1);
            if (j == 5) {
                u8 c0 = row[0];
                if (pick != c0) {
                    row[1] = c0;
                    row[0] = pick;
                }
            }
        }
        k = 5;
        if (src[5] != 0) {
            cnt = 5 - has;
            do {
                e = src + k;
                if (((u32)(ran_suu(1) & 0xFFFF) >> (k % 16)) & 1) {
                    u8 c = *e;
                    if (flagq != c && nq != c && c < 0x83 && c < 0x67 && c != 0x65) {
                        int h = (u32)(ran_suu(1) & 0xFFFF) % cnt;
                        int pos = has + h;
                        t = row + pos;
                        if (flagq != *t) {
                            if (nq != 0 ? nq != *t : 1) {
                                if (pos != 0 || nq == 0) {
                                    if (lb_key_quest_ck(*e) != 0) {
                                        if (Quest_clear_bit_ck(*e) == 1) {
                                            *t = *e;
                                        }
                                    } else {
                                        *t = *e;
                                    }
                                }
                            }
                        }
                    }
                }
                k += 1;
            } while (src[k] != 0);
        }
    }
    key = key_quest;
    if (key != 0) {
        if (key != 0x67) {
            kn = 1;
            lb_quest_info[0x1E] = key;
        } else {
            lb_quest_info[0x1E] = key;
            lb_quest_info[0x1F] = key + 1;
            lb_quest_info[0x20] = key + 2;
            lb_quest_info[0x21] = key + 3;
            kn = 5;
            lb_quest_info[0x22] = 0x6B;
        }
        key_quest_num = kn;
    }
}
