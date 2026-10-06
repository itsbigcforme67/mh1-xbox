/* flsnd02 - 0x002162C0-0x002163C4: flSndJointInit(hdbuf, bdbuf) and flSndJointSet(pack): append the HD and
 * BD parts of a sound pack (header +8/+0xC = HD offset/size, +0x10/+0x14 = BD offset/size) behind the ones
 * already collected in hdpack (see flsnd00.c). */
#include "types.h"

extern int hdpack[];

void *memcpy(void *, const void *, unsigned int);

typedef struct SNDPK {
    u8 x00[8];
    int hdoff;      /* 0x08 */
    int hdsize;     /* 0x0C */
    int bdoff;      /* 0x10 */
    int bdsize;     /* 0x14 */
} SNDPK;

void flSndJointInit(int hdbuf, int bdbuf) {
    hdpack[7] = hdbuf;
    hdpack[8] = bdbuf;
    hdpack[0] = 0;
    hdpack[5] = 0;
    hdpack[6] = 0;
}

void flSndJointSet(SNDPK *pk) {
    u8 *dst = (u8 *)hdpack[7] + hdpack[5];

    memcpy(dst, (u8 *)pk + pk->hdoff, pk->hdsize);
    hdpack[1 + hdpack[0]] = (int)dst;
    memcpy((u8 *)hdpack[8] + hdpack[6], (u8 *)pk + pk->bdoff, pk->bdsize);
    hdpack[5] = hdpack[5] + pk->hdsize;
    hdpack[6] = hdpack[6] + pk->bdsize;
    hdpack[0] = hdpack[0] + 1;
}
