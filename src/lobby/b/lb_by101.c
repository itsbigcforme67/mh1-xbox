/* lb_by101 - agent B promoted near-match 0x005B9C40-0x005B9CEC: lbc_top_menu_01 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { s8 x0; u8 pad1; u16 x2; u8 pad4[0x1C]; } CLSI;
extern CLSI ClassInfo;
typedef struct { u8 pad0[0x10]; u8 st; u8 pad11[0x14B]; } PLI;
extern PLI PlazaInfo[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[0x2A]; s8 x2C33; s8 x2C34; } CWS_tm1;
#define CWX ((CWS_tm1 *)cw)

void lbc_top_menu_01(void) {
    int i;
    PLI *p;

    CWX->x2C08 = 1;
    i = 0;
    if (0 < ClassInfo.x2) {
        p = PlazaInfo;
        do {
            if (p->st == 3) {
                break;
            }
            i++;
            p++;
            if (i == ClassInfo.x2) {
                To_LogOut(1);
            }
        } while (i < ClassInfo.x2);
    }
    ClassInfo.x0 = i + 1;
    CWX->x2C33 = 2;
    CWX->x2C34 = 0;
}
