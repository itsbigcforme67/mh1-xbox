/* lb_v16 - offline guild quest table 0x005CA3D0-0x005CA8C0: Lb_make_quest_tbl_local (fills lb_quest_info rows from the offline tables, key quest, shuffle). */
#include "types.h"
int ran_suu();
extern u8 *quest_local_tbl[];
extern u8 lb_quest_info[];
extern u8 lb_quest_clear[8];
extern u8 key_quest;
extern s8 key_quest_num;
int lb_get_quest_level();
int lb_set_key_quest_local();
int get_new_quest();
int get_flag_quest();
int Quest_clear_bit_ck();
int Event_flag_ck();
void Event_flag_set();
void Lb_make_quest_tbl_local(void) {
    int lv;
    u32 rnd;
    int flag;
    u8 sel;
    u8 cnt;
    int m;
    u8 *t;
    u8 *tp;
    u8 *cp;
    u8 *p;
    int n;
    u8 v;
    u8 *row;
    u8 *e;
    int i;
    int k;
    int j;
    key_quest = 0;
    key_quest_num = 0;
    flag = 0;
    sel = 0;
    cnt = 0;
    lv = (s8)lb_get_quest_level(1);
    m = 0;
    tp = (u8 *)quest_local_tbl;
    cp = lb_quest_clear;
    p = lb_quest_info;
    do {
        t = *(u8 **)tp;
        m++;
        *cp = 0;
        tp += 4;
        cp++;
        p[0] = t[0];
        p[1] = t[1];
        p[2] = t[2];
        p[3] = t[3];
        p[4] = t[4];
        p += 5;
    } while (m < 5);
    lb_quest_info[0x19] = 0;
    lb_quest_info[0x1A] = 0;
    lb_quest_info[0x1B] = 0;
    lb_quest_info[0x1C] = 0;
    lb_quest_info[0x1D] = 0;
    if (lv == -1) {
        key_quest = 0x83;
    } else {
        tp = (u8 *)quest_local_tbl;
        cp = lb_quest_clear;
        i = 0;
        row = lb_quest_info;
        do {
            t = *(u8 **)tp;
            v = get_new_quest(i);
            if (v == 0) {
                *cp = 1;
            }
            if (lv == 5) {
                key_quest = 0xAA;
            } else if (lv == i) {
                flag = 1;
                sel = get_flag_quest(lv);
                if (sel == 0) {
                    flag = 0;
                    switch (lv) {
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
                } else if (i != 0 && i != 1) {
                    j = 0;
                    do {
                        if (sel == row[j]) {
                            row[j] = row[4];
                            row[4] = sel;
                            break;
                        }
                        j++;
                    } while (j < 5);
                    if (j == 5) {
                        row[4] = sel;
                    }
                }
            }
            if (v != 0) {
                if (i != 0 && i != 1) {
                    flag = 1;
                    j = 0;
                    do {
                        if (v == row[j]) {
                            row[j] = row[0];
                            row[0] = v;
                            break;
                        }
                        j++;
                    } while (j < 5);
                    if (j == 5) {
                        row[0] = v;
                    }
                }
            } else {
                cnt++;
            }
            if (i != 0 && i != 1 && (k = 5, t[5] != 0)) {
                do {
                    if (rnd == 0) {
                        rnd = ran_suu(1) & 0xFFFF;
                    }
                    e = t + k;
                    if ((*e != 0xAB || Quest_clear_bit_ck(0xAB) == 1) && ((rnd >> (k % 32)) & 1) && sel != *e && v != *e) {
                        n = flag + (u16)ran_suu(1) % (u32)(5 - flag);
                        if (*e != 0x90 || Quest_clear_bit_ck(0x94) == 1) {
                            if (sel != row[n] && (n != 0 || v == 0)) {
                                row[n] = *e;
                            }
                        }
                    }
                    k++;
                } while (t[k] != 0);
            }
            tp += 4;
            cp++;
            row += 5;
            i++;
        } while (i < 5);
    }
    if (cnt == 5 && Event_flag_ck(0x4C) == 0) {
        Event_flag_set(0x4C);
    }
    if (key_quest == 0xAA) {
        key_quest_num = lb_set_key_quest_local();
    } else if (key_quest != 0) {
        lb_quest_info[0x19] = key_quest;
        key_quest_num = 1;
    }
}
