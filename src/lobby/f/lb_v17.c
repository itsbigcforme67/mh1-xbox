/* lb_v17 - online guild quest table 0x005CAAF0-0x005CB0E0: Lb_make_quest_tbl (fills lb_quest_info rows from the online tables, key quest, shuffle). */
#include "types.h"
int ran_suu();
extern u8 *quest_lv_tbl[];
extern u8 lb_quest_info[];
extern u8 lb_quest_clear[8];
extern u8 key_quest;
extern s8 key_quest_num;
int lb_get_quest_level();
int get_new_quest();
int get_flag_quest();
int Quest_clear_bit_ck();
int Event_flag_ck();
void Event_flag_set();
int lb_guild_check_keyQuest();
int Lb_guild_check_requireF();
int lb_key_quest_ck();
void Lb_make_quest_tbl(void) {
    int lv;
    int flag;
    u8 sel;
    u8 cnt;
    int m;
    u8 *t;
    u8 *tp;
    u8 *cp;
    u8 *p;
    u8 v;
    u8 *row;
    u8 *e;
    int i;
    int k;
    int j;
    int n;
    u8 x;
    u32 r;
    key_quest = 0;
    key_quest_num = 0;
    sel = 0;
    cnt = 0;
    lv = (s8)lb_get_quest_level(1);
    m = 0;
    tp = (u8 *)quest_lv_tbl;
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
    } while (m < 6);
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
    tp = (u8 *)quest_lv_tbl;
    cp = lb_quest_clear;
    i = 0;
    row = lb_quest_info;
    do {
        t = *(u8 **)tp;
        v = get_new_quest(i);
        if (v == 0) {
            cnt++;
            *cp = 1;
        }
        if (cnt == 6 && Event_flag_ck(0x4E) == 0) {
            Event_flag_set(0x4E);
        }
        if (lv == i) {
            sel = get_flag_quest(lv);
            if (sel == 0 && lb_guild_check_keyQuest(lv) == 1) {
                switch (lv) {
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
                    if (sel == row[j]) {
                        row[j] = row[4];
                        row[4] = sel;
                        break;
                    }
                    j++;
                } while (j < 5);
                if (j == 5 && sel != 0) {
                    row[4] = sel;
                }
            }
        }
        if (v != 0) {
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
            if (j == 5 && v != 0) {
                row[0] = v;
            }
        } else {
            flag = 0;
        }
        if (v == 0 && i == 0 && Event_flag_ck(0x33) == 0) {
            Event_flag_set(0x33);
        }
        if (i == 0) {
            r = ran_suu(1) & 0xFFFF;
            x = (r & 1) ? 1 : 3;
            flag++;
            j = 1;
            do {
                if (x == row[j]) {
                    row[j] = row[0];
                    row[0] = x;
                    break;
                }
                j++;
            } while (j < 5);
            if (j == 5 && x != row[0]) {
                row[1] = row[0];
                row[0] = x;
            }
        }
        k = 5;
        if (t[5] != 0) {
            do {
                r = ran_suu(1) & 0xFFFF;
                if ((r >> (k % 16)) & 1) {
                    e = t + k;
                    if (sel != *e && v != *e && *e < 0x83 && *e < 0x67 && *e != 0x65) {
                        n = flag + (u16)ran_suu(1) % (u32)(5 - flag);
                        p = row + n;
                        if (sel != *p && (v == 0 || v != *p) && (n != 0 || v == 0)) {
                            if (lb_key_quest_ck(*e) == 0 || Quest_clear_bit_ck(*e) == 1) {
                                *p = *e;
                            }
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
    } while (i < 6);
    if (key_quest != 0) {
        if (key_quest != 0x67) {
            lb_quest_info[0x1E] = key_quest;
            key_quest_num = 1;
        } else {
            lb_quest_info[0x1E] = key_quest;
            lb_quest_info[0x1F] = key_quest + 1;
            lb_quest_info[0x20] = key_quest + 2;
            lb_quest_info[0x21] = key_quest + 3;
            lb_quest_info[0x22] = 0x6B;
            key_quest_num = 5;
        }
    }
}
