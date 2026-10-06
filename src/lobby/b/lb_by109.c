/* lb_by109 - agent B promoted near-match 0x005BA070-0x005BA164: CallBack_Result_Plaza_ReadAllocation (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int cnLBS_Get_PlazaStatus(u16 id, void *p);
int cnLBS_Get_mhPlazaJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_PlazaName(u16 id, void *p);
typedef struct { s8 x0; u8 pad1; u16 x2; u8 pad4[0x1C]; } CLSI;
extern CLSI ClassInfo;
typedef struct { s16 x0; u8 pad2[0x15A]; } PLZ;
extern PLZ PlazaInfo[];
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 pad2C32[1]; s8 x2C33; s8 x2C34; u8 pad2C35[0x10]; u8 x2C45; } CWS_ra;
#define CWX ((CWS_ra *)cw)

void CallBack_Result_Plaza_ReadAllocation(CNET_RES res) {
    int i;
    PLZ *p;

    if ((CWX->x2C31 != 5) && (CWX->x2C45 == 6) && (res.val != 2) && (res.val == 0)) {
        CWX->x2C45 = 0;
        CWX->x2C33 = 1;
        CWX->x2C34 = 0;
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
