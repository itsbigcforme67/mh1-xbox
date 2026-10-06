/* netdev16 - prot_02 (SLPM_654.95 0x002399A0-0x00239AB0): third step handler of the connection state machine (InetConnectAllCore): tracks how long
   the state has lasted, then step 0 moves on to step 1 and step 1 runs InetDisconnectAll until it reports done, when it clears the state and
   returns 4 (names guessed from use). Written new in this pass from an m2c draft. */
#include "types.h"
extern u8 InetSys[0x40];
int InetDisconnectAll();

int prot_02(s8 *a, s8 *b, s8 *c, s16 *d, s16 *e, s16 *f, s16 *g, s8 *h) {
    InetSys[0xD] = (InetSys[0xD] < InetSys[0xC]) ? InetSys[0xD] + 1 : InetSys[0xC];
    switch (*b) {
    case 0:
        *b = *b + 1;
        *c = 0;
        *d = 0;
        return 0;
    case 1:
        if (InetDisconnectAll(c, d) != 0) {
            *a = 0;
            *b = 0;
            *c = 0;
            *d = 0;
            *e = 0;
            *f = 0;
            *g = 0;
            return 4;
        }
        return 0;
    default:
        return 0;
    }
}
