/*
 * rt_text.c - the text layer (docs/english.md).
 *
 * The game's text is Shift-JIS inside its executables and quest files. This
 * layer lets a translation table (a plain text file kept outside the repo,
 * made with tools/text_dump.py) replace strings by their location in the
 * Japanese build. No table, or an empty one, means the original text.
 *
 * Table file, one entry per line ('#' starts a comment line):
 *     main:0x35B2B0 = text             string at that PS2 address of SLPM_654.95
 *     game:0x...   lobby:0x...   select:0x...     same for the overlays
 *     quest:10:0x4DB0 = text           quest 10's file, string at that file offset
 *     0x2EF860[3] = text               (no prefix) the pointer in slot 3 of the table at that address
 *     @proportional = 0                (optional) 0: keep fixed-width ASCII
 * Value: "\n" is a line feed, "\\" a backslash, "~Cnn" a colour code (kept
 * as is). A value may start with "{w=PIXELS}": the text is wrapped at spaces
 * to fit that width (measured at font size 20). An empty value leaves the
 * Japanese. The text is used as typed (ASCII, or Shift-JIS bytes).
 *
 * How it takes effect:
 *  - pointer words of the data tables and images that point at a replaced
 *    string are re-pointed while they are relocated (rt_data.c: map_ptr,
 *    map_lb, map_sel), so every consumer sees the new text;
 *  - strings the C reads as arrays or through code addresses (the lit_ and D_
 *    literals) are caught where text is consumed: font_print and friends, and
 *    the game C's sprintf / strcpy / strcat (renamed with -D in GAMEFLAGS,
 *    rt_text_sp.c), look their string pointer up here (rt_text_tr);
 *  - quest text: rt_text_quest rewrites the loaded mission file.
 */
#include "rt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { SP_MAIN, SP_GAME, SP_LOBBY, SP_SELECT, SP_YN, SP_QUEST, SP_SLOT };

typedef struct {
    uint64_t key;       /* space << 32 | va (quest: no << 16 | offset) */
    char *text;
    int limit;          /* wrap width in px, 0: none */
} ovr;

static ovr *ovrs;
static int novr, parsed, proportional = 1;

typedef struct { const void *host; char *text; } reg;
static reg *regs;
static int nreg;

static int cmp_ovr(const void *a, const void *b)
{
    uint64_t x = ((const ovr *)a)->key, y = ((const ovr *)b)->key;
    return x < y ? -1 : x > y;
}
static int cmp_reg(const void *a, const void *b)
{
    uintptr_t x = (uintptr_t)((const reg *)a)->host, y = (uintptr_t)((const reg *)b)->host;
    return x < y ? -1 : x > y;
}

static ovr *find(uint64_t key)
{
    int lo = 0, hi = novr - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (ovrs[mid].key == key)
            return &ovrs[mid];
        if (ovrs[mid].key < key)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return NULL;
}

static int stat_repoint[8];     /* pointer words re-pointed while relocating, per space (RT_TEXT_STAT) */
int rt_text_active(void) { return novr > 0; }

const char *rt_text_for(int space, uint32_t va)
{
    ovr *o;
    if (!novr)
        return NULL;
    o = find((uint64_t)space << 32 | va);
    if (o)
        stat_repoint[space & 7]++;
    return o ? o->text : NULL;
}

static int space_of(const char *s, size_t n)
{
    static const char *const names[] = { "main", "game", "lobby", "select", "yn", "quest" };
    int i;
    for (i = 0; i < 6; i++)
        if (strlen(names[i]) == n && !strncmp(s, names[i], n))
            return i;
    return -1;
}

/* one table line; returns 1 added, 0 skipped (comment, empty value), -1 bad */
static int parse_line(char *q)
{
    char *eq, *r, *w, *txt;
    int space, limit = 0;
    unsigned long a, b = 0;
    uint64_t key;
    size_t len = strlen(q);
    while (len && (q[len - 1] == '\n' || q[len - 1] == '\r'))
        q[--len] = 0;
    if (!len || q[0] == '#')
        return 0;
    eq = strstr(q, " =");
    if (q[0] == '@') {
        if (!strncmp(q, "@proportional", 13) && eq)
            proportional = atoi(eq + 2) != 0;
        return 0;
    }
    if (!eq)
        return -1;
    txt = eq + 2;
    if (*txt == ' ')
        txt++;
    if (!*txt)
        return 0;                       /* untranslated: keep the Japanese */
    if (!(r = strchr(q, ':')) || (space = space_of(q, (size_t)(r - q))) < 0)
        space = SP_SLOT, r = q - 1;     /* "0x2EF860[3]": a table slot */
    r++;
    if (space == SP_QUEST) {
        char *e;
        a = strtoul(r, &e, 0);
        if (*e != ':')
            return -1;
        b = strtoul(e + 1, NULL, 0);
        if (a > 0xFFFF || b > 0xFFFF)
            return -1;
        key = (uint64_t)SP_QUEST << 32 | (uint32_t)(a << 16 | b);
    } else if (space == SP_SLOT) {
        char *e;
        uint64_t va;
        a = strtoul(r, &e, 0);
        if (*e == '[')
            b = strtoul(e + 1, NULL, 0);
        va = a + 4 * b;
        key = (uint64_t)SP_SLOT << 32 | (uint32_t)va;
    } else {
        a = strtoul(r, NULL, 0);
        key = (uint64_t)space << 32 | (uint32_t)a;
    }
    if (!strncmp(txt, "{w=", 3)) {
        limit = atoi(txt + 3);
        txt = strchr(txt, '}');
        if (!txt)
            return -1;
        txt++;
        if (*txt == ' ')
            txt++;
    }
    {
        char *str = malloc(strlen(txt) + 1);
        ovr *o;
        for (w = str, r = txt; *r; r++) {
            if (r[0] == '\\' && r[1] == 'n') {
                *w++ = '\n';
                r++;
            } else if (r[0] == '\\' && r[1] == '\\') {
                *w++ = '\\';
                r++;
            } else {
                *w++ = *r;
            }
        }
        *w = 0;
        ovrs = realloc(ovrs, (size_t)(novr + 1) * sizeof *ovrs);
        o = &ovrs[novr++];
        o->key = key;
        o->text = str;
        o->limit = limit;
    }
    return 1;
}

int rt_text_parse(const char *path)
{
    FILE *f;
    char line[4096];
    int bad = 0, n = 0, i, k;
    if (parsed)
        return 0;
    parsed = 1;
    if (!path)
        path = getenv("RT_TEXT_TABLE");
    if (!path || !*path)
        return 0;
    if (sizeof(void *) != 4) {          /* the data tables hold 32-bit pointer words */
        fprintf(stderr, "rt_text: needs the 32-bit build\n");
        return -1;
    }
    if (!(f = fopen(path, "rb"))) {
        fprintf(stderr, "rt_text: cannot open %s\n", path);
        return -1;
    }
    while (fgets(line, sizeof line, f)) {
        int r = parse_line(line);
        if (r < 0)
            bad++;
        else
            n += r;
    }
    fclose(f);
    if (getenv("RT_PROP"))
        proportional = atoi(getenv("RT_PROP")) != 0;
    qsort(ovrs, (size_t)novr, sizeof *ovrs, cmp_ovr);
    for (i = k = 0; i < novr; i++)      /* later lines win over earlier ones with the same id */
        if (i + 1 < novr && ovrs[i].key == ovrs[i + 1].key)
            continue;
        else
            ovrs[k++] = ovrs[i];
    novr = k;
    fprintf(stderr, "rt_text: %s: %d strings, %d bad lines, proportional %d\n", path, novr, bad, proportional);
    return bad ? -1 : 0;
}

/* ------------------------------------------------------------ registry */
void rt_text_finish(void)
{
    int i, k, n;
    const void *h[8];
    if (!novr)
        return;
    regs = malloc((size_t)novr * 4 * sizeof *regs + sizeof *regs);
    if (getenv("RT_TEXT_STAT"))
        fprintf(stderr, "rt_text: re-pointed words: main/game %d, lobby %d, select %d\n", stat_repoint[0] + stat_repoint[1],
                stat_repoint[2], stat_repoint[3]);
    for (i = 0; i < novr; i++) {
        int space = (int)(ovrs[i].key >> 32);
        uint32_t va = (uint32_t)ovrs[i].key;
        if (space == SP_QUEST || space == SP_YN)
            continue;
        if (space == SP_SLOT) {         /* the slot itself: set its pointer word */
            n = rt_data_hosts(SP_MAIN, va, h);
            for (k = 0; k < n; k++) {
                uint32_t v = (uint32_t)(uintptr_t)ovrs[i].text;
                memcpy((void *)h[k], &v, 4);
                break;                  /* the first host copy is the table's */
            }
            continue;
        }
        n = rt_data_hosts(space, va, h);
        if (!n && getenv("RT_TEXT_STAT"))
            fprintf(stderr, "rt_text: no host for space %d 0x%X\n", space, (unsigned)va);
        for (k = 0; k < n; k++) {
            regs[nreg].host = h[k];
            regs[nreg].text = ovrs[i].text;
            nreg++;
        }
    }
    qsort(regs, (size_t)nreg, sizeof *regs, cmp_reg);
}

const char *rt_text_tr(const char *p)
{
    int lo = 0, hi = nreg - 1;
    uintptr_t x = (uintptr_t)p;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        uintptr_t y = (uintptr_t)regs[mid].host;
        if (y == x)
            return regs[mid].text;
        if (y < x)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return p;
}

static const unsigned char *font_adv;       /* set by rt_text_font_ready */
static void wrap(char *s, int limit, const unsigned char *adv);
typedef struct { char *p; int limit; } pend;
static pend pending[256];
static int npend;

/* ------------------------------------------------------------ quest files */
/* The mission file keeps its text at the end and points at it from words
 * in its first part (header +0x60..+0x6C: title, goal, failure, client text;
 * more for the clear lines). The replacement is appended to the file and
 * the one word that held the old offset is changed. */
size_t rt_text_quest(int no, uint8_t *buf, size_t n, size_t cap)
{
    int i;
    uint32_t t0;
    if (!novr || n < 0x70)
        return n;
    memcpy(&t0, buf + 0x60, 4);
    if (t0 >= n)
        return n;
    for (i = 0; i < novr; i++) {
        uint32_t k = (uint32_t)ovrs[i].key;
        size_t len;
        uint32_t w, off;
        if ((int)(ovrs[i].key >> 32) != SP_QUEST || (int)(k >> 16) != no)
            continue;
        off = k & 0xFFFF;
        len = strlen(ovrs[i].text) + 1;
        if (n + len + 4 > cap)
            continue;
        for (w = 0; w + 4 <= t0; w += 4) {
            uint32_t v;
            memcpy(&v, buf + w, 4);
            if (v == off) {
                n = (n + 3) & ~(size_t)3;
                memcpy(buf + n, ovrs[i].text, len);
                if (ovrs[i].limit > 0) {        /* wrap this copy now, or when the font is known */
                    if (font_adv)
                        wrap((char *)buf + n, ovrs[i].limit, font_adv);
                    else if (npend < 256) {
                        pending[npend].p = (char *)buf + n;
                        pending[npend++].limit = ovrs[i].limit;
                    }
                }
                v = (uint32_t)n;
                memcpy(buf + w, &v, 4);
                n += len;
                break;
            }
        }
    }
    return n;
}

/* Monster Hunter G's mission file (mNNNNN.mib, AFS index 2934 + n in file-name order, `no` is the number in the
 * file name): a 0x48-byte header, the text strings one after the other from 0x68 up to the offset in word 0x60,
 * and AT that offset a table of the strings' file offsets (title, goal, failure, client, ...). The clear lines
 * (the reward screen's "All requested monsters slain.", the "Monsters to defeat: N" banners) sit at the end of
 * the file; header word 0x30 points at a table of their offsets (MH1's file has the same table, which
 * rt_text_quest's scan of the first part already finds). The replacement is appended and the table entry
 * changed, as for the MH1 file; nothing after the old text moves. */
size_t rt_text_quest_g(int no, uint8_t *buf, size_t n, size_t cap)
{
    int i, j;
    uint32_t end, clr;
    if (!novr || n < 0x70)
        return n;
    memcpy(&end, buf + 0x60, 4);
    memcpy(&clr, buf + 0x30, 4);
    if (end < 0x68 || end + 16 > n)
        return n;
    if (clr < 0x70 || clr >= n)
        clr = 0;
    for (i = 0; i < novr; i++) {
        uint32_t k = (uint32_t)ovrs[i].key;
        size_t len;
        uint32_t w, off, v;
        if ((int)(ovrs[i].key >> 32) != SP_QUEST || (int)(k >> 16) != no)
            continue;
        off = k & 0xFFFF;
        len = strlen(ovrs[i].text) + 1;
        if (n + len + 4 > cap)
            continue;
        for (j = 0; j < 4 + 16; j++) {      /* the first four table entries, then up to 16 clear-line entries */
            w = j < 4 ? end + 4 * j : clr ? clr + 4 * (j - 4) : 0;
            if (!w || w + 4 > n)
                break;
            memcpy(&v, buf + w, 4);
            if (v == off) {
                n = (n + 3) & ~(size_t)3;
                memcpy(buf + n, ovrs[i].text, len);
                if (ovrs[i].limit > 0) {
                    if (font_adv)
                        wrap((char *)buf + n, ovrs[i].limit, font_adv);
                    else if (npend < 256) {
                        pending[npend].p = (char *)buf + n;
                        pending[npend++].limit = ovrs[i].limit;
                    }
                }
                v = (uint32_t)n;
                memcpy(buf + w, &v, 4);
                n += len;
                break;
            }
        }
    }
    return n;
}

/* ------------------------------------------------------------ wrapping */
static int adv_of(const unsigned char *adv, unsigned c)
{
    return adv[c & 0xFF];
}

/* the width of a replacement's ASCII text: widths in px at font size 20 */
static void wrap(char *s, int limit, const unsigned char *adv)
{
    int x = 0, wordw = 0;
    char *sp = NULL;
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c == '\n') {
            x = 0;
            sp = NULL;
            continue;
        }
        if (c == '~' && s[1] == 'C' && s[2] && s[3]) {      /* colour code: no width */
            s += 3;
            continue;
        }
        if (c == ' ') {
            sp = s;
            x += adv_of(adv, c);
            continue;
        }
        x += adv_of(adv, c);
        (void)wordw;
        if (x > limit && sp) {
            *sp = '\n';
            x = 0;
            for (sp++; sp <= s; sp++)
                x += adv_of(adv, (unsigned char)*sp);
            sp = NULL;
        }
    }
}

void rt_text_font_ready(const unsigned char *adv)
{
    int i;
    font_adv = adv;
    for (i = 0; i < npend; i++)
        wrap(pending[i].p, pending[i].limit, adv);
    npend = 0;
    for (i = 0; i < novr; i++)
        if (ovrs[i].limit > 0)
            wrap(ovrs[i].text, ovrs[i].limit, adv);
}

int rt_text_proportional(void) { return novr > 0 && proportional; }

/* ------------------------------------------------------------ formats whose %s arguments are game strings
 * vsnprintf, but each %s argument is looked up like a format (rt_text_tr): the game prints many strings as
 * font_print("%s", title) or DispFrameListA's title, so a replaced string passed as an argument would stay Japanese
 * (seen in G: the quest-info and equipment window titles, the "back to the village" banner). With no table this is
 * plain vsnprintf. */
#include <stdarg.h>
int rt_text_vsnprintf(char *buf, size_t n, const char *fmt, va_list ap)
{
    size_t o = 0;
    const char *p = fmt;
    if (!novr)
        return vsnprintf(buf, n, fmt, ap);
    if (n)
        buf[0] = 0;
    while (*p) {
        char spec[32];
        size_t k = 0;
        int w;
        if (*p != '%') {
            if (o + 1 < n)
                buf[o] = *p;
            o++;
            p++;
            continue;
        }
        spec[k++] = *p++;
        while (*p && k < sizeof spec - 8 && strchr("-+ #0123456789.*hlLqjzt", *p)) {
            if (*p == '*') {            /* a width / precision argument */
                k += (size_t)snprintf(spec + k, sizeof spec - k, "%d", va_arg(ap, int));
                p++;
                continue;
            }
            spec[k++] = *p++;
        }
        if (!*p)
            break;
        spec[k++] = *p;
        spec[k] = 0;
        {
            char tmp[0x400];
            const char *mods = spec + 1;
            char c = *p++;
            int is_ll = strstr(mods, "ll") != NULL || strchr(mods, 'q') != NULL;
            int is_l = !is_ll && strchr(mods, 'l') != NULL;
            switch (c) {
            case '%': w = snprintf(tmp, sizeof tmp, "%%"); break;
            case 's': w = snprintf(tmp, sizeof tmp, spec, rt_text_tr(va_arg(ap, const char *))); break;
            case 'c': w = snprintf(tmp, sizeof tmp, spec, va_arg(ap, int)); break;
            case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': case 'a': case 'A':
                w = snprintf(tmp, sizeof tmp, spec, va_arg(ap, double)); break;
            case 'p': w = snprintf(tmp, sizeof tmp, spec, va_arg(ap, void *)); break;
            case 'n': (void)va_arg(ap, int *); w = 0; tmp[0] = 0; break;
            case 'd': case 'i': case 'o': case 'u': case 'x': case 'X':
                if (is_ll) w = snprintf(tmp, sizeof tmp, spec, va_arg(ap, long long));
                else if (is_l) w = snprintf(tmp, sizeof tmp, spec, va_arg(ap, long));
                else w = snprintf(tmp, sizeof tmp, spec, va_arg(ap, int));
                break;
            default:                    /* not a conversion the game uses: copy it as text */
                w = snprintf(tmp, sizeof tmp, "%s", spec);
                break;
            }
            if (w < 0)
                w = 0;
            if (w > (int)sizeof tmp - 1)
                w = (int)sizeof tmp - 1;
            if (o < n)
                memcpy(buf + o, tmp, (size_t)w < n - o ? (size_t)w : n - o);
            o += (size_t)w;
        }
    }
    if (n)
        buf[o < n ? o : n - 1] = 0;
    return (int)o;
}
