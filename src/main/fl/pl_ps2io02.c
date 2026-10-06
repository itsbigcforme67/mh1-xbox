/* ps2 pad module setup (SLPM_654.95 0x00194AE0-0x00194B5C): flPS2PADModuleInit. */
#include "types.h"

extern char lit_198_0035C1E0[];
extern char lit_199_0035C210[];
extern char lit_200_0035C230[];
int flPS2IopModuleLoad(char *, int, int, int);

int flPS2PADModuleInit(void) {
    int i;

    flPS2IopModuleLoad(lit_198_0035C1E0, 0, 0, 0);
    flPS2IopModuleLoad(lit_199_0035C210, 0, 0, 0);
    for (i = 0; i < 2; i++) {
        flPS2IopModuleLoad(lit_200_0035C230, 0, 0, 0);
    }
    return 1;
}
