/* Monster Hunter 2 sub_main. Written from our own disassembly of the owner's disc (docs/mh2.md). The quest's per-frame state machine,
   called from the game task (0x326E64): returns 0 running, 1 and -1, -2 for how it ended. Struct and global names are placeholders. */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

extern u8 D_006ADCB0[], D_006ADCF0, D_006ADCEC, D_006ADCEE, D_006ADED4[];
extern u8 player_work[];
extern u8 quest_w[];
#define D_005D8260 (*(s8 *)(quest_w + 0x0))
#define D_005D8261 (*(s8 *)(quest_w + 0x1))
#define D_005D8266 (*(s8 *)(quest_w + 0x6))
#define D_005D87B1 (*(s8 *)(quest_w + 0x551))
#define D_005D8268 (*(u16 *)(quest_w + 0x8))
#define D_005D827E (*(s16 *)(quest_w + 0x1E))
#define D_005D83B6 (*(s16 *)(quest_w + 0x156))
#define D_005D87B2 (*(s16 *)(quest_w + 0x552))
#define D_005D87B4 (*(s16 *)(quest_w + 0x554))
#define D_005D87B6 (*(s16 *)(quest_w + 0x556))
#define D_005D8270 (*(int *)(quest_w + 0x10))
#define D_005D8274 (*(int *)(quest_w + 0x14))
#define D_005D8288 (*(int *)(quest_w + 0x28))
#define D_005D838C (*(int *)(quest_w + 0x12C))
#define D_005D8388 (*(int *)(quest_w + 0x128))
#define D_005D8390 (*(int *)(quest_w + 0x130))
#define D_005D878C (*(int *)(quest_w + 0x52C))
typedef struct { u8 r, x; } QRANK;
#define D_005D8300 ((QRANK *)(quest_w + 0xA0))
extern int D_00445540, D_00445544, D_00445548, D_0044554C, D_00445550, D_00445598, D_0044559C;
extern u8 D_005D4A60;

int Get_hunter_rank(), Online_ck(), Quest_error_set(), Quest_time_get(), func_6ED360(), func_901790();
int set01_set(), set01_set2();
int sub_main_func_0036BA50(), sub_main_func_0039E400(), sub_main_func_0039F970(), sub_main_func_0039F990();
int sub_main_func_003A0760(), sub_main_func_003A3A60(), sub_main_func_003A3C50(), sub_main_func_003A5880();
int sub_main_func_003A7EC0(), sub_main_func_003A84C0(), sub_main_func_003A87D0(), sub_main_func_003A8CB0();
int sub_main_func_003A9230(), sub_main_func_003B5A40();

#pragma divbyzerocheck off
int sub_main_func_003A2E30(void) {
    int i;
    u8 *g;
    u8 *me;
    int t;
    u8 *pl;
    int x;
    int y;

    me = player_work + D_006ADCEC * 0x970;
    g = D_006ADCB0;
    for (i = 0; i < 4; i++) {
        if (g[0x224] != 1) {
            D_005D838C |= 1 << (i + 8);
        }
        g++;
    }
    if (D_005D83B6 > 0) {
        D_005D83B6--;
    }
    sub_main_func_003A5880(); /* a0 still holds 1 from the loop */
    if (D_005D8268 == 0) {
        return 0;
    }
    if (D_005D8266 == 7) {
        return -2;
    }
    if (D_005D8266 == 8) {
        return -2;
    }
    switch (D_005D8260) {
    case 0:
        if (D_005D8266 == 4) {
            return 1;
        }
        if (D_005D8266 == 6) {
            return -1;
        }
        if (D_005D827E == -1) {
            return 1;
        }
        if (sub_main_func_0039E400() == 1) {
            pl = player_work;
            g = D_006ADCB0;
            for (i = 0; i < D_006ADCEE; i++) {
                if (g[0x224] != 1 && pl[0x409] < 3) {
                    Quest_error_set();
                }
                g++;
                pl += 0x970;
            }
        }
        sub_main_func_003A8CB0();
        sub_main_func_003A84C0(1);
        sub_main_func_003A0760();
        D_005D827E = sub_main_func_003A9230(me, D_005D827E, &y);
        sub_main_func_003A3A60();
        sub_main_func_003A3C50();
        if (sub_main_func_0039F990() == 1) {
            func_6ED360();
        }
        return 0;
    case 1:
        switch (D_005D8261) {
        case 0:
            D_005D8261++;
            t = Quest_time_get(1);
            x = t - Quest_time_get(0);
            D_005D87B2 = 0;
            D_005D878C = x;
            D_005D87B1 = 15;
            D_005D87B6 = D_005D878C;
            D_005D87B4 = (u32)D_005D878C >> 16;
            func_901790(6, D_006ADCEC);
            D_005D8270 = 150;
            break;
        case 1:
            sub_main_func_003A7EC0(1);
            if (D_005D8270 <= 0) {
                D_005D8261++;
                if (sub_main_func_003A87D0(0) == 1) {
                    if (D_005D8288 & 0x20000006) {
                        D_005D8270 = 600;
                        if (Online_ck() == 1) {
                            set01_set2(D_00445540);
                        } else {
                            set01_set2(D_00445544);
                        }
                    } else {
                        D_005D8270 = 1800;
                        if (Online_ck() == 1) {
                            set01_set2(D_00445548);
                        } else {
                            set01_set2(D_0044554C);
                        }
                    }
                } else {
                    D_005D8270 = 300;
                    if (Online_ck() == 1) {
                        set01_set2(D_00445598);
                    } else {
                        set01_set2(D_0044559C);
                    }
                }
            }
            break;
        case 2:
            sub_main_func_003A7EC0(1);
            if (D_005D8270 <= 0) {
                D_005D8261 = 0;
                D_005D8260++;
                set01_set2(D_00445550);
                if (sub_main_func_0039E400() == 0 && sub_main_func_0036BA50() == 0 && sub_main_func_0039F970() == 0) {
                    set01_set(0, 15, 0);
                }
            }
            break;
        }
        break;
    case 2:
        switch (D_005D8261) {
        case 3:
            break;
        case 0: {
            u8 *r;
            int w;
            D_005D8261++;
            D_005D8266 = 4;
            D_005D8274 = D_005D8274 / D_006ADCEE;
            w = D_006ADCEC;
            r = &D_005D8300[w].r;
            *r = Get_hunter_rank(&D_005D4A60, D_005D8274);
            D_005D87B2 = 0;
            D_005D87B1 = 12;
            D_005D87B4 = *r;
            D_005D87B6 = 0;
            func_901790(6, w);
            D_005D8390 = 5400;
            D_005D838C = D_005D838C | 1 << D_006ADCEC;
            break;
        }
        case 1:
            if (D_005D8390 > 0) {
                D_005D8390--;
            }
            g = D_006ADCB0;
            for (i = 0; i < 4; i++) {
                if (g[0x224] != 1) {
                    D_005D838C |= 1 << i;
                }
                g++;
            }
            if ((D_005D838C & 0xF) == 0xF || D_005D8390 <= 0 || D_006ADED4[D_006ADCEC] != 1) {
                D_005D87B2 = 0;
                D_005D87B1 = 13;
                D_005D8261++;
                D_005D87B6 = D_005D8388;
                D_005D87B4 = (u32)D_005D8388 >> 16;
                func_901790(6, D_006ADCEC);
                D_005D8390 = 1800;
                D_005D838C |= 1 << (D_006ADCEC + 4);
            }
            break;
        case 2:
            if (D_005D8390 > 0) {
                D_005D8390--;
            }
            g = D_006ADCB0;
            for (i = 0; i < 4; i++) {
                if (g[0x224] != 1) {
                    D_005D838C |= 1 << (i + 4);
                }
                g++;
            }
            if ((u8)D_005D838C == 0xFF || D_005D8390 <= 0 || D_006ADED4[D_006ADCEC] != 1) {
                D_006ADCF0 = 4;
                D_005D8261++;
            }
            break;
        }
        break;
    case 3:
        if (D_005D8261 == 0) {
            if (D_005D8390 > 0) {
                D_005D8390--;
            }
            g = D_006ADCB0;
            for (i = 0; i < 4; i++) {
                if (g[0x224] != 1) {
                    D_005D838C |= 1 << i;
                }
                g++;
            }
            if ((D_005D838C & 0xF) == 0xF || D_005D8390 <= 0 || D_006ADED4[D_006ADCEC] != 1) {
                D_005D8261++;
                t = Quest_time_get(1);
                D_005D878C = t - Quest_time_get(0);
                if (D_005D8266 == 4) {
                    D_006ADCF0 = 4;
                    pl = player_work;
                    for (i = 0; i < D_006ADCEE; i++) {
                        if (pl[0x409] < 3 && sub_main_func_003B5A40(pl[0x409], (player_work + D_006ADCEC * 0x970)[0x409], 1) == 1) {
                            break;
                        }
                        pl += 0x970;
                    }
                    set01_set(3, 3, (s16)i);
                } else {
                    D_006ADCF0 = 6;
                    pl = player_work;
                    for (i = 0; i < D_006ADCEE; i++) {
                        if (pl[0x409] < 3 && sub_main_func_003B5A40(pl[0x409], (player_work + D_006ADCEC * 0x970)[0x409], 1) == 0) {
                            break;
                        }
                        pl += 0x970;
                    }
                    set01_set(3, 3, (s16)i);
                }
            }
        }
        break;
    }
    return 0;
}
#pragma divbyzerocheck reset
