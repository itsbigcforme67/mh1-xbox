/* G online (PC): func_0012BDD0 (G main 0x12BDD0), HAND-FIXED: G's printf to the screen font (varargs: m2c cannot), as
 * G's code: format into a 0x400 buffer, then font_print_sp(-1, "%s", buffer) unless the font is being reset. */
#include "../gonl.h"
#include <stdarg.h>
#include <stdio.h>

extern s32 font_reset_flag;
s32 font_print_sp();

void func_0012BDD0(const char *fmt, ...) {
    static char buf[0x400];
    va_list ap;
    if (font_reset_flag != 0 || fmt == NULL)
        return;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    font_print_sp("%s", buf);   /* (the PC's font_print_sp takes no colour: MH1's signature) */
}
