/* Near-match (not linked): flPADConfigSet (0x0018E630), 10 of 32 instructions differ: the original keeps the counter in a2, the destination in a3 and
 * the port offset in t0, this build swaps the last two. */
#include "types.h"

typedef struct PADPAIR { s16 a, b; } PADPAIR;
extern u8 flpad_root[];
extern u8 flpad_conf[];
extern u8 *flpad_adr[2];
typedef struct PADCFG { PADPAIR k[11]; } PADCFG;
extern PADCFG flpad_config[2];
extern u8 fltpad_config_basic[];

void flPADConfigSetACRtoXX(int, s16, s16, s16);

void flPADConfigSet(PADPAIR *src, int port) {
    PADPAIR *dst = flpad_config[port].k;
    int i = 11;

    do {
        i--;
        *dst++ = *src++;
    } while (i > 0);
    flPADConfigSetACRtoXX(port, flpad_config[port].k[9].a, flpad_config[port].k[9].b, flpad_config[port].k[10].a);
}
