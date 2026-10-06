/* lb_by104 - agent B promoted near-match 0x005BA3B0-0x005BA47C: CallBack_Result_Plaza_ReadAllocation2 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int cnLBS_Get_PlazaStatus(u16 id, void *p);
int cnLBS_Get_mhPlazaJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_PlazaName(u16 id, void *p);
typedef struct { s8 x0; u8 pad1; u16 x2; u8 pad4[0x1C]; } CLSI;
extern CLSI ClassInfo;
typedef struct { s16 x0; u8 pad2[0x15A]; } PLZ;
extern PLZ PlazaInfo[];

void CallBack_Result_Plaza_ReadAllocation2(CNET_RES res) {
    int i;
    PLZ *p;

    if ((F(u8, (u8 *)cw, 0x2C31) != 5) && (res.val != 2) && (res.val == 0)) {
        cnLBS_Get_PlazaCount(&ClassInfo.x2);
        i = 0;
        if (0 < ClassInfo.x2) {
            p = PlazaInfo;
            do {
                p->x0 = i + 1;
                cnLBS_Get_PlazaStatus(i + 1, (u8 *)p + 0x10);
                cnLBS_Get_mhPlazaJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                cnLBS_Get_PlazaName(i + 1, (u8 *)p + 0x14);
                i++;
                p++;
            } while (i < ClassInfo.x2);
        }
    }
}
