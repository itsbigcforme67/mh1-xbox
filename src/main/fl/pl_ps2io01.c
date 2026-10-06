/* ps2 memory card module setup (SLPM_654.95 0x00194A00-0x00194A48): ps2McModuleInit. ps2McInit follows in pl_ps2io_nm.c. */
#include "types.h"

extern char lit_190_0035C190[];
extern char lit_191_0035C1B0[];
int flPS2IopModuleLoad(char *, int, int, int);

int ps2McModuleInit(void) {
    flPS2IopModuleLoad(lit_190_0035C190, 0, 0, 0);
    flPS2IopModuleLoad(lit_191_0035C1B0, 0, 0, 0);
    return 1;
}
