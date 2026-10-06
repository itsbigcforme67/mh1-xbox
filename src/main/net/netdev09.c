/* netdev09 - rpc_initialize (SLPM_654.95 0x002339E0-0x00233A00). Written new in this pass. */
#include "types.h"
int CpInetInitialize();

int rpc_initialize(void) {
    CpInetInitialize();
    return 0;
}
