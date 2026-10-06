/* netdev07 - search_sif_call_rpc (SLPM_654.95 0x00233130-0x00233180): calls the IOP device-search RPC and maps errors to -1. Written new in this pass. */
#include "types.h"
extern u8 Search_sif[0x28];
extern u8 SifRpcWork_buf[0x20];
int sceSifCallRpc();

int search_sif_call_rpc(int cmd) {
    int r;

    r = sceSifCallRpc(Search_sif, cmd, 0, SifRpcWork_buf, 0x40, SifRpcWork_buf, 0x40, 0, 0);
    if (r < 0) {
        return -1;
    }
    return r;
}
