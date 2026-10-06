/* lb_by96 - agent B promoted near-match 0x005B6160-0x005B626C: get_next_server (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 lbs_tryed_ctr;
extern char D_3A39A0[];
extern char LbsTryedWork[];
extern char LbsInfoWork[];
extern char ConnectLbsId[];
typedef struct { u8 pad00[0x4]; u8 x04; u8 pad05[0x1]; u8 x06; u8 padEND[0x29]; } CNW;
extern CNW CnetWork;

s32 get_next_server(void) {
    s32 i;
    u8 *p;

    if (*(u16 *)((u8 *)&D_3A39A0 + CnetWork.x04 * 0x14) != 0) {
        if (CnetWork.x06 != 0) {
            i = 0;
            if (0 < lbs_tryed_ctr) {
                p = (u8 *)&LbsTryedWork;
                do {
                    if (strncmp(p, (u8 *)&LbsInfoWork + CnetWork.x04 * 0x14, 0xC) == 0) {
                        return 0;
                    }
                    i++;
                    p += 0x14;
                } while (i < lbs_tryed_ctr);
            }
        }
        memcpy(&ConnectLbsId, (u8 *)&LbsInfoWork + CnetWork.x04 * 0x14, 0x14);
        return 1;
    }
    return 0;
}
