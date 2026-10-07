/*
 * xbox_libc.c - C library pieces nxdk's pdclib does not have yet (Xbox build
 * only, tools/build_xbox.py). atof: decimal and exponent forms only, enough
 * for the viewer's command-line numbers.
 */
#include <ctype.h>

double atof(const char *s)
{
    double v = 0, f = 1;
    int neg = 0, e = 0, eneg = 0;

    while (isspace((unsigned char)*s))
        s++;
    if (*s == '+' || *s == '-')
        neg = *s++ == '-';
    while (isdigit((unsigned char)*s))
        v = v * 10 + (*s++ - '0');
    if (*s == '.')
        for (s++; isdigit((unsigned char)*s); s++)
            v += (*s - '0') * (f /= 10);
    if (*s == 'e' || *s == 'E') {
        s++;
        if (*s == '+' || *s == '-')
            eneg = *s++ == '-';
        while (isdigit((unsigned char)*s))
            e = e * 10 + (*s++ - '0');
        while (e--)
            v = eneg ? v / 10 : v * 10;
    }
    return neg ? -v : v;
}

/* fopen with '/' turned into '\' (xbox_compat.h); this file is compiled
 * without that header, so fopen here is pdclib's */
#include <stdio.h>
#include <string.h>

FILE *xbox_fopen(const char *name, const char *mode)
{
    char p[512];
    size_t i, n = strlen(name);

    if (n >= sizeof p)
        return NULL;
    for (i = 0; i <= n; i++)
        p[i] = name[i] == '/' ? '\\' : name[i];
    return fopen(p, mode);
}
