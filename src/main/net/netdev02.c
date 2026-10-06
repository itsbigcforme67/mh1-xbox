/* netdev02 - bind_rpc_blocking (SLPM_654.95 0x00233940-0x002339D4): binds an IOP RPC client (sceSifBindRpc) and retries with a delay loop until
   it is served. Written new in this pass. */
#include "types.h"
typedef struct SIFC { u8 x00[0x24]; int serve; } SIFC;
int sceSifBindRpc();
void bind_rpc_blocking(SIFC *sif, int id) {
    volatile int i;

    for (;;) {
        if (sceSifBindRpc(sif, id, 0) >= 0 && sif->serve != 0) {
            break;
        }
        i = 0x10000;
        while (i != 0) {
            i--;
        }
    }
}

