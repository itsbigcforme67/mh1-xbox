/* lb_gs01 - agent C 0x005C3020-0x005C30B4: Get_ServerName (find the connected lobby server record in BsLbsInfo, 0x102 stride; return pointer to its name). */
#include "lobby_a.h"
extern char BsLbsInfo[];
extern char ConnectLbsId[];
int Get_ServerName(void) {
    s32 i;
    char *p;

    i = 0;
    p = BsLbsInfo;
    do {
        if (memcmp(ConnectLbsId, p, 0xC) == 0) {
            break;
        }
        i++;
        p += 0x102;
    } while (i < 0xA);
    if (i < 0xA) {
        return (int)(BsLbsInfo + i * 0x102 + 0xD);
    }
    return 0;
}
