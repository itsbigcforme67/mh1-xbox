/* create_server_table (0x5B5DD0): 16/225 differ: only register naming in the connected-server loop (the original keeps the dead 'found' counter alive, ours folds it into the pointer). Index form in the loops is what fixed the first two loops. Not built. */
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
    s32 found;
    int tot[10];
    int cur[10];
    s32 i;
    s32 j;
    LBSW *w;
    LBSW *w2;
    int free[10];
    s32 l;

    CnetWork.x04 = 0;
    if (mode == 0) {
        memset(LbsInfoWork, 0, 0xC8);
        for (i = 0; i < BsLbsCount; i++) {
            tot[i] = atoi(bsCsvWork + 0x266C + i * 0x21);
            cur[i] = atoi(bsCsvWork + 0x2522 + i * 0x21);
            free[i] = tot[i] - cur[i];
            if (free[i] < 0) {
                free[i] = 0;
            }
        }
        if (BsLbsCount == 1) {
            memcpy(LbsInfoWork, BsLbsInfo, 0xC);
            LbsInfoWork[0].h10 = tot[0];
            LbsInfoWork[0].h0E = cur[0];
            LbsInfoWork[0].h12 = free[0];
        } else {
            for (i = 0; i < BsLbsCount; i++) {
                memcpy(&LbsInfoWork[i], BsLbsInfo + i * 0x102, 0xC);
                LbsInfoWork[i].h10 = tot[i];
                LbsInfoWork[i].h0E = cur[i];
                LbsInfoWork[i].h12 = free[i];
            }
            l = 0;
            w = LbsInfoWork;
            if (0 < BsLbsCount - 1) {
                do {
                    j = l + 1;
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
                    l++;
                    w++;
                } while (l < BsLbsCount - 1);
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
        for (i = 0; i < BsLbsCount; i++) {
            if (strncmp(ConnectLbsId, (char *)&LbsInfoWork[i], 0xC) != 0) {
                memcpy(&tmp[found], &LbsInfoWork[i], 0x14);
                found++;
            }
        }
        memcpy(LbsInfoWork, tmp, 0xC8);
    }
}
