/* boot03 - 0x0022FE00-0x0022FE0C: cnNet_ModuleLoad, boots the PS2 network module (CngNetPS2ModuleBootInitialize
 * with both flags set). Sits between main and SlashToBackslash in boot01/boot02. */
#include "types.h"

void CngNetPS2ModuleBootInitialize(int, int);

void cnNet_ModuleLoad(void) {
    CngNetPS2ModuleBootInitialize(1, 1);
}
