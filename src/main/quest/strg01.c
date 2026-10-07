/* SLPM_654.95 0x00229AE0-0x00229B40: str_gattai, a printf-style wrapper (vsprintf into dst) used by the quest code. */
#include "types.h"
#include "va.h"
int vsprintf(char *, const char *, va_list);
void str_gattai(char *dst, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsprintf(dst, fmt, ap);
}
