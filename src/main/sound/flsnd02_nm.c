/* flSndJointSet (1 instruction off: the original loads the first memcpy length through a0, not the saved copy
 * s0; not built). Part of flsnd02.c's range 0x002162C0-0x002163C4. */
#include "types.h"

extern int hdpack[];

void *memcpy(void *, const void *, int);

typedef struct SNDPK {
    u8 _pad00[8];
    int hdoff;      /* 0x08 */
    int hdsize;     /* 0x0C */
    int bdoff;      /* 0x10 */
    int bdsize;     /* 0x14 */
} SNDPK;

void flSndJointSet(SNDPK *pk) {
    u8 *dst = (u8 *)hdpack[7] + hdpack[5];

    memcpy(dst, (u8 *)pk + pk->hdoff, pk->hdsize);
    hdpack[1 + hdpack[0]] = (int)dst;
    memcpy((u8 *)hdpack[8] + hdpack[6], (u8 *)pk + pk->bdoff, pk->bdsize);
    hdpack[5] = hdpack[5] + pk->hdsize;
    hdpack[6] = hdpack[6] + pk->bdsize;
    hdpack[0] = hdpack[0] + 1;
}
