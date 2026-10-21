/* G online (PC): func_001D5970 (G main 0x1D5970), HAND-FIXED: a jump to strlen with the caller's argument. */
#include "../gonl.h"
#include <string.h>

s32 func_001D5970(const char *s) {
    return (s32)strlen(s);
}
