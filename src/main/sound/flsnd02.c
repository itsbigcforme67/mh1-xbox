/* flsnd02 - 0x002162C0-0x002162EC: flSndJointInit(hdbuf, bdbuf) (flSndJointSet(pack) is in flsnd02_nm.c): append the HD and
 * BD parts of a sound pack (header +8/+0xC = HD offset/size, +0x10/+0x14 = BD offset/size) behind the ones
 * already collected in hdpack (see flsnd00.c). */
#include "types.h"

extern int hdpack[];

void flSndJointInit(int hdbuf, int bdbuf) {
    hdpack[7] = hdbuf;
    hdpack[8] = bdbuf;
    hdpack[0] = 0;
    hdpack[5] = 0;
    hdpack[6] = 0;
}
