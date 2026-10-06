/* create_server_table (0x5B5DD0): logic complete (server list: counts, free slots, sort by free desc, move the connected server first); 51/225 differ in register naming and load/store scheduling. Not built. */
#include "lobby_a.h"
extern u8 BsLbsCount;
extern char bsCsvWork[];
extern char BsLbsInfo[];
extern char ConnectLbsId[];
typedef struct { char name[0xC]; u8 pad0C[2]; u16 h0E; u16 h10; u16 h12; } LBSW;
extern LBSW LbsInfoWork[10];
typedef struct { u8 pad[4]; u8 x04; u8 padEND[0x2B]; } CNW4;
extern CNW4 CnetWork;
void create_server_table(mode, unused)
u8 mode;
int unused;
{
    LBSW tmp[10];
    int tot[10];
    int cur[10];
    int free[10];
    s32 i;
    s32 j;
    s32 found;
    char *p;
    LBSW *w;
    LBSW *w2;
    char *q;
    int *a;
    int *b;
    int *c;

    CnetWork.x04 = 0;
    if (mode == 0) {
        memset(LbsInfoWork, 0, 0xC8);
        i = 0;
        if (0 < BsLbsCount) {
            p = bsCsvWork;
            a = tot;
            b = cur;
            c = free;
            do {
                *a = atoi(p + 0x266C);
                *b = atoi(p + 0x2522);
                *c = *a - *b;
                if (*c < 0) {
                    *c = 0;
                }
                i++;
                p += 0x21;
                a++;
                b++;
                c++;
            } while (i < BsLbsCount);
        }
        if (BsLbsCount == 1) {
            memcpy(LbsInfoWork, BsLbsInfo, 0xC);
            *(s16 *)0x3A39A0 = tot[0];
            *(s16 *)0x3A399E = cur[0];
            *(s16 *)0x3A39A2 = free[0];
        } else {
            i = 0;
            if (0 < BsLbsCount) {
                q = BsLbsInfo;
                w = LbsInfoWork;
                a = tot;
                b = cur;
                c = free;
                do {
                    memcpy(w, q, 0xC);
                    i++;
                    q += 0x102;
                    w->h10 = *a;
                    a++;
                    w->h0E = *b;
                    b++;
                    w->h12 = *c;
                    c++;
                    w++;
                } while (i < BsLbsCount);
            }
            i = 0;
            w = LbsInfoWork;
            if (0 < BsLbsCount - 1) {
                do {
                    j = i + 1;
                    if (j < BsLbsCount) {
                        w2 = &LbsInfoWork[j];
                        do {
                            if (w->h12 < w2->h12) {
                                memcpy(tmp, w2, 0x14);
                                memcpy(w2, w, 0x14);
                                memcpy(w, tmp, 0x14);
                            }
                            j++;
                            w2++;
                        } while (j < BsLbsCount);
                    }
                    i++;
                    w++;
                } while (i < BsLbsCount - 1);
            }
        }
    } else if (mode == 1) {
        memset(tmp, 0, 0xC8);
        found = 0;
        i = 0;
        if (0 < BsLbsCount) {
            w = LbsInfoWork;
            do {
                if (strncmp(ConnectLbsId, (char *)w, 0xC) == 0) {
                    memcpy(tmp, &LbsInfoWork[i], 0x14);
                    found = 1;
                    break;
                }
                i++;
                w++;
            } while (i < BsLbsCount);
        }
        i = 0;
        if (0 < BsLbsCount) {
            w2 = LbsInfoWork;
            w = &tmp[found];
            do {
                if (strncmp(ConnectLbsId, (char *)w2, 0xC) != 0) {
                    memcpy(w, w2, 0x14);
                    w++;
                    found++;
                }
                i++;
                w2++;
            } while (i < BsLbsCount);
        }
        memcpy(LbsInfoWork, tmp, 0xC8);
    }
}
