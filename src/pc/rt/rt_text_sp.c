/*
 * rt_text_sp.c - sprintf / strcpy / strcat for the game C (rt_text.c).
 * tools/build_pc.sh compiles the decompiled game C with
 * -Dsprintf=rt_text_sprintf -Dstrcpy=rt_text_strcpy -Dstrcat=rt_text_strcat,
 * so only the game's own calls come here and look their format / source
 * string up in the translation registry first; host code (SDL, libmpeg2,
 * the C runtime) keeps the real functions. Deliberately includes no system
 * header (the game C's own prototypes are renamed the same way).
 */
#include <stdarg.h>

const char *rt_text_tr(const char *p);
int vsprintf(char *, const char *, va_list);

int rt_text_sprintf(char *s, const char *fmt, ...)
{
    va_list ap;
    int n;
    va_start(ap, fmt);
    n = vsprintf(s, rt_text_tr(fmt), ap);
    va_end(ap);
    return n;
}

char *rt_text_strcpy(char *d, const char *s)
{
    char *r = d;
    s = rt_text_tr(s);
    while ((*d++ = *s++) != 0)
        ;
    return r;
}

char *rt_text_strcat(char *d, const char *s)
{
    char *r = d;
    s = rt_text_tr(s);
    while (*d)
        d++;
    while ((*d++ = *s++) != 0)
        ;
    return r;
}
