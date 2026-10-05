/* lb_bz158 - lobby UI/client 0x005BA610-0x005BA6BC: lbc_in_plaza_01 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
typedef struct FRIENDENT { f32 f[12]; } FRIENDENT;      /* friend list entry (0x30 bytes), copied word-wise through the FPU */
typedef struct { u8 pad0000[0x2C08]; s8 x2C08; u8 pad2C09[0x2C34 - 0x2C09]; u8 x2C34; } CWS_ip1;
int Lbs_plaza();

void lbc_in_plaza_01(void) {
    switch (((CWS_ip1 *)cw)->x2C34) {
    case 0:
        ((CWS_ip1 *)cw)->x2C08 = 1;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            ((CWS_ip1 *)cw)->x2C34 = ((CWS_ip1 *)cw)->x2C34 + 1;
            Lbc_init_network_work();
            fade_set(2);
        }
        break;
    case 1:
        switch (Lbs_plaza(network_work)) {
        case 1:
            break;
        case 2:
            To_LogOut(0);
            break;
        }
        break;
    }
}
