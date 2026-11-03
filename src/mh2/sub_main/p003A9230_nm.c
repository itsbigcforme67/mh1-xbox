/* Monster Hunter 2 sub_main. Written from our own disassembly of the owner's disc (docs/mh2.md).
   Runs one step of a quest's event script (8-byte commands, op 0x1A = label). Names are placeholders. */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef struct { s16 op, a, b, c; } QCMD;
typedef struct { int t, x; } QMSG;

extern u8 D_006ADCF0, D_006ADFC3, D_006ADCEC, D_006ADED4[];
extern s8 D_006ADB2E;
extern s8 D_005D8260, D_005D8261, D_005D8266, D_005D826A, D_005D826B, D_005D8282, D_005D82F7, D_005D8397, D_005D8398;
extern s8 D_005D83B2, D_005D83B3, D_005D83B4, D_005D87B1;
extern s16 D_005D827E, D_005D8280, D_005D8394, D_005D87B2, D_005D87B4, D_005D87B6;
extern int D_005D826C, D_005D8270, D_005D838C;
extern QCMD *D_005D82B4;
extern int *D_005D82B8;
extern u8 *D_005D82F8;
extern QMSG D_0044BDD0[];
extern struct { int f, x; } D_005D83C0[];
extern int D_00445558, D_00445560, D_00445564, D_00445568, mission_area;

int Quest_clear_ck(), Quest_time_get(), act_ck(), func_7090C0(), func_901790(), quest_item_ck3(), quest_timer_send();
int set01_set(), set01_set2(), set01_set2_use_mem(), set07_set(), str_gattai();
int sub_main_func_00323C90(), sub_main_func_0036BA50(), sub_main_func_0039E400(), sub_main_func_0039F540();
int sub_main_func_0039F970(), sub_main_func_003A0820(), sub_main_func_003A3C10(), sub_main_func_003A7EC0();
int sub_main_func_003A80F0(), sub_main_func_003A8470(), sub_main_func_003A84C0(), sub_main_func_003A8680();
int sub_main_func_003A87D0(), sub_main_func_003A89C0(), sub_main_func_003A8F60();
int sub_main_func_003AB770();

#define FIND(q, key)                              \
    i = 0;                                        \
    for (;;) {                                    \
        if ((q)->op == 0x1A && (key) == (q)->a) { \
            i++;                                  \
            break;                                \
        }                                         \
        i++;                                      \
        (q)++;                                    \
    }

#define SKIPTO(o)            \
    i++;                     \
    p++;                     \
    for (;;) {               \
        if (p->op == (o)) {  \
            i++;             \
            break;           \
        }                    \
        i++;                 \
        p++;                 \
    }

#define HURT()                                                                  \
    w = D_005D82F8;                                                             \
    if (w != 0 && (sub_main_func_003A0820(w[2]) & 0xFF) != 0xFF) {              \
        D_005D8394 = *(s16 *)(w + 0x2C2);                                       \
        *(u16 *)(&D_005D8260 + w[2] * 2 + 0x48C) = *(u16 *)(w + 0x9A4);         \
    }

static int sub_main_func_003A8990(u16 n) {
    return (D_005D83C0[n].f & 0x31) == 0x31;
}

s16 sub_main_func_003A9230(u8 *pl, s16 i) {
    char buf[32];
    QCMD *q;
    QCMD *p;
    QCMD *tbl;
    u8 *w;
    int r;
    int k;

    r = -1;
    if (D_006ADCF0 == 2 && D_005D8270 >= 0) {
        if (D_005D82F7 > 0 && D_0044BDD0[D_005D82F7].t >= D_005D8270) {
            str_gattai(buf, D_00445558, D_0044BDD0[D_005D82F7].t / 1800);
            buf[31] = 0;
            set01_set2_use_mem(buf);
            D_005D82F7--;
        }
        sub_main_func_003A7EC0(1);
        if (D_005D8270 <= 0) {
            D_005D8270 = 0;
            p = D_005D82B4;
            i = 0;
            quest_timer_send();
            k = D_005D826A;
            D_005D8270 = -1;
            for (;;) {
                if (p->op == 0x1A && k == p->a) {
                    i++;
                    break;
                }
                i++;
                p++;
            }
        } else if (D_005D8270 % 9000 == 0) {
            quest_timer_send();
        }
    }
    for (;;) {
        if (D_005D826B != 0 && sub_main_func_003A8F60() == 1) {
            q = D_005D82B4;
            FIND(q, (s8)D_005D826B);
            D_005D826B = 0;
        }
        if (D_005D8397 != 0 && D_005D8398 != 0) {
            q = D_005D82B4;
            FIND(q, (s16)D_005D8397);
            D_005D8397 = 0;
        }
        tbl = D_005D82B4;
        p = &tbl[i];
        switch (p->op) {
        case 1:
            D_005D83B4 = p->a;
            i++;
            continue;
        case 2:
            sub_main_func_003A80F0(p->a, p->b, p->c);
            i++;
            continue;
        case 3:
            break;
        case 9:
            D_005D826C = p->a;
            i++;
            break;
        case 10:
            if (--D_005D826C <= 0) {
                i++;
            }
            break;
        case 11:
            w = (u8 *)(D_005D82B8 + p->a);
            set01_set2(*(int *)w + mission_area);
            i++;
            break;
        case 5:
            i++;
            continue;
        case 6:
            if (sub_main_func_003A84C0(0) == 1 || sub_main_func_003A8680() == 1) {
                i++;
                continue;
            }
            SKIPTO(7);
            continue;
        case 0x24:
            if ((s16)sub_main_func_003A89C0((u16)p->a) > p->b) {
                break;
            }
            i++;
            continue;
        case 12:
            if (*(u16 *)(pl + 0x680) != p->a) {
                break;
            }
            i++;
            continue;
        case 4:
            D_005D8270 += (u16)p->a;
            i++;
            break;
        case -1:
            HURT();
            D_005D87B2 = 0;
            D_005D87B4 = 0;
            D_006ADFC3 = 0x81;
            D_005D8266 = 3;
            D_006ADCF0 = 3;
            i++;
            D_005D87B6 = 0;
            k = D_006ADCEC;
            D_005D838C |= 1 << (k + 8);
            D_005D87B1 = 2;
            func_901790(6, k, k + 8);
            continue;
        case 0x2D:
            if ((D_005D838C & 0xF00) != 0xF00 && D_006ADED4[D_006ADCEC] == 1) {
                break;
            }
            i++;
            break;
        case 0x1E:
            D_005D8261 = 0;
            i = -1;
            D_005D8260++;
            break;
        case 0x17:
            D_005D827E = i = D_005D8280;
            continue;
        case 0x18:
            if ((s16)act_ck(pl, 4, 0) == 0) {
                break;
            }
            i++;
            continue;
        case -2:
            D_005D87B2 = 0;
            D_005D87B4 = 0;
            D_006ADCF0 = 5;
            i++;
            D_005D8266 = 5;
            D_005D87B6 = 0;
            D_005D87B1 = 4;
            func_901790(6, D_006ADCEC);
            continue;
        case 0x1F:
            D_005D8266 = 6;
            i = -1;
            D_006ADCF0 = 6;
            sub_main_func_003AB770();
            if (sub_main_func_0039E400() == 0) {
                set01_set2(D_00445560);
            }
            if (sub_main_func_0039E400() == 0 && sub_main_func_0036BA50() == 0 && sub_main_func_0039F970() == 0) {
                set01_set(0, 15, 0);
            }
            break;
        case 0x1B:
            D_005D826A = p->a;
            k = Quest_time_get(1);
            str_gattai(buf, D_00445564, k / 1800);
            buf[31] = 0;
            set01_set2_use_mem(buf);
            if (sub_main_func_0039E400() == 0) {
                set07_set();
            }
            i++;
            break;
        case 0x20:
            D_005D8282 = (u16)p->a;
            sub_main_func_003A3C10();
            i++;
            continue;
        case 0x21:
            if (quest_item_ck3(&D_005D8260, p) == 0) {
                break;
            }
            i++;
            continue;
        case 0x23:
            D_005D826B = p->a;
            i++;
            break;
        case 0x2C:
            D_005D8397 = p->a;
            i++;
            break;
        case 0x25:
            D_006ADCF0 = 5;
            i++;
            D_005D8266 = 5;
            set01_set2(D_00445568);
            D_005D87B2 = 0;
            D_005D87B1 = 10;
            D_005D87B4 = 0;
            D_005D87B6 = 0;
            func_901790(6, D_006ADCEC);
            continue;
        case 0x26:
            HURT();
            D_005D87B2 = 0;
            D_005D87B4 = 0;
            D_005D8266 = 3;
            i++;
            D_006ADCF0 = 3;
            D_005D87B6 = 0;
            k = D_006ADCEC;
            D_005D838C |= 1 << (k + 8);
            D_005D87B1 = 11;
            func_901790(6, k, k + 8);
            continue;
        case 0x28: {
            s16 h = *(s16 *)(D_005D82F8 + 0x2C2);
            if (h <= D_005D8394 - p->a) {
                D_005D8394 = h;
                i++;
                continue;
            }
            SKIPTO(0x29);
            continue;
        }
        case 0x2A:
            if ((func_7090C0() & 0xFF) == 1) {
                sub_main_func_003A8470(0, 0x200);
                i++;
                continue;
            }
            SKIPTO(0x2B);
            continue;
        case 0x1C:
            k = p->a;
            FIND(tbl, k);
            continue;
        case 0x27:
            break;
        case 0x1A:
            i++;
            continue;
        case 0x2F:
            D_005D826A = p->a;
            i++;
            break;
        case 0x30:
            if ((s16)sub_main_func_003A89C0((u16)p->a) <= p->b) {
                k = p->c;
                q = D_005D82B4;
                FIND(q, k);
                continue;
            }
            i++;
            continue;
        case 0x31:
            r = i;
            i++;
            continue;
        case 0x32:
            r = -1;
            i++;
            continue;
        case 0x33:
            if (D_006ADB2E == 0) {
                if (sub_main_func_003A8990(0) == 1) {
                    sub_main_func_0039F540(2);
                    i = -1;
                } else if (sub_main_func_003A8990(1) == 1) {
                    sub_main_func_0039F540(1);
                    i = -1;
                }
            } else if (D_005D83B2 <= 0) {
                sub_main_func_0039F540(2);
                i = -1;
            } else if (D_005D83B3 <= 0) {
                sub_main_func_0039F540(1);
                i = -1;
            }
            break;
        case 0x34:
            sub_main_func_003A8470(0, 0);
            i++;
            continue;
        }
        break;
    }
    if (D_006ADFC3 == 0x80) {
        D_006ADFC3 = 0;
        if (sub_main_func_003A87D0(0) == 1 && Quest_clear_ck(0) != 0) {
            sub_main_func_00323C90();
        }
    }
    if ((s16)r != -1) {
        i = r;
    }
    return i;
}
