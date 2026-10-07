/*
 * install.c - copy the game's data files from the player's own ISO into a data folder, once.
 *
 * A small ISO9660 reader (2048-byte sectors, root directory only, no external tools). It checks the disc:
 * SLPM_654.95 must be there and every file's size and CRC32 must be the known Japanese MH1 ones (the
 * numbers are checksums of the files, not their contents). The files are copied with a progress bar and
 * installed.ok records what was installed. Later launches read the data folder; the ISO is never needed
 * again. Nothing is shipped: the files come only from the player's disc.
 */
#include "install.h"
#include "rt/rt_plat.h"
#include "rt/rt_log.h"

#include <SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef MH1_WINDOWS
#include <direct.h>
#include <windows.h>
#define MKDIR(p) _mkdir(p)
#else
#define MKDIR(p) mkdir((p), 0755)
#endif

static const struct { const char *name; uint32_t size, crc; int required; } want[] = {
    { "AFS_DATA.AFS", 121866240u, 0x3BD5B15Au, 1 },
    { "AFS00.AFS", 775729152u, 0xE2312A4Du, 1 },
    { "AFS01.AFS", 20463616u, 0xD7B1D61Fu, 1 },
    { "SLPM_654.95", 5648908u, 0xCF1A62C0u, 1 },
    { "SYSTEM.CNF", 76u, 0x9CF85397u, 0 },
};
#define NWANT (int)(sizeof want / sizeof want[0])

int install_is_iso_name(const char *p)
{
    size_t n = p ? strlen(p) : 0;
    return n > 4 && (p[n - 4] == '.') && (p[n - 3] | 32) == 'i' && (p[n - 2] | 32) == 's' && (p[n - 1] | 32) == 'o';
}

static int exists(const char *p)
{
    FILE *f = fopen(p, "rb");
    if (f)
        fclose(f);
    return f != NULL;
}

int install_has_data(const char *dir)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/AFS_DATA.AFS", dir);
    if (!exists(p))
        return 0;
    snprintf(p, sizeof p, "%s/SLPM_654.95", dir);
    return exists(p);
}

static void mkdirs(const char *path)
{
    char tmp[1024], *s;
    snprintf(tmp, sizeof tmp, "%s", path);
    for (s = tmp + 1; *s; s++)
        if (*s == '/' || *s == '\\') {
            char c = *s;
            *s = 0;
            MKDIR(tmp);
            *s = c;
        }
    MKDIR(tmp);
}

static int writable_dir(const char *dir)
{
    char p[1100];
    FILE *f;
    mkdirs(dir);
    snprintf(p, sizeof p, "%s/.w_test", dir);
    f = fopen(p, "wb");
    if (!f)
        return 0;
    fclose(f);
    remove(p);
    return 1;
}

static void user_dir(char *out, size_t n)
{
#ifdef MH1_WINDOWS
    const char *a = getenv("APPDATA");
    snprintf(out, n, "%s/mh1pc/data", a && *a ? a : ".");
#else
    const char *x = getenv("XDG_DATA_HOME"), *h = getenv("HOME");
    if (x && *x)
        snprintf(out, n, "%s/mh1pc/data", x);
    else
        snprintf(out, n, "%s/.local/share/mh1pc/data", h ? h : ".");
#endif
}

static void exe_dir(char *out, size_t n)
{
    char *b = SDL_GetBasePath();
    snprintf(out, n, "%s", b ? b : "./");
    if (b)
        SDL_free(b);
    while (strlen(out) > 1 && (out[strlen(out) - 1] == '/' || out[strlen(out) - 1] == '\\'))
        out[strlen(out) - 1] = 0;
}

int install_find_data(char *out, size_t n)
{
    char e[900], d[1024];
    exe_dir(e, sizeof e);
    snprintf(d, sizeof d, "%s/data", e);
    if (install_has_data(d)) { snprintf(out, n, "%s", d); return 1; }
    snprintf(d, sizeof d, "%s/disc", e);
    if (install_has_data(d)) { snprintf(out, n, "%s", d); return 1; }
    user_dir(d, sizeof d);
    if (install_has_data(d)) { snprintf(out, n, "%s", d); return 1; }
    return 0;
}

void install_default_dir(char *out, size_t n)
{
#ifdef MH1_WINDOWS
    char e[900], d[1024];
    exe_dir(e, sizeof e);
    snprintf(d, sizeof d, "%s/data", e);
    if (writable_dir(d)) { snprintf(out, n, "%s", d); return; }
#endif
    user_dir(out, n);
}

/* ------------------------------------------------------------ ISO9660 */
typedef struct { FILE *f; } iso_t;
typedef struct { uint32_t lba, size; } iso_file;

/* the ISO is 4 GB and long / off_t are 32 bits here: the 64-bit file functions */
#ifdef MH1_WINDOWS
#define open_big(p) fopen((p), "rb")
static int seek64(FILE *f, uint64_t off) { return _fseeki64(f, (long long)off, SEEK_SET); }
#else
extern FILE *fopen64(const char *, const char *);
extern int fseeko64(FILE *, long long, int);
#define open_big(p) fopen64((p), "rb")
static int seek64(FILE *f, uint64_t off) { return fseeko64(f, (long long)off, SEEK_SET); }
#endif

static int rd_sector(iso_t *i, uint32_t lba, void *buf, size_t n)
{
    return seek64(i->f, (uint64_t)lba * 2048u) == 0 && fread(buf, 1, n, i->f) == n;
}
static uint32_t le32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }

/* the root directory entry of NAME (";1" version suffix ignored, case-insensitive); 0 = found */
static int iso_find(iso_t *i, const char *name, iso_file *out)
{
    uint8_t pvd[2048], *dir;
    uint32_t lba, size, pos = 0;
    if (!rd_sector(i, 16, pvd, 2048) || memcmp(pvd + 1, "CD001", 5) != 0 || pvd[0] != 1)
        return -2;
    lba = le32(pvd + 156 + 2);
    size = le32(pvd + 156 + 10);
    if (size == 0 || size > (1u << 22) || !(dir = malloc(size)))
        return -2;
    if (!rd_sector(i, lba, dir, size)) {
        free(dir);
        return -2;
    }
    while (pos < size) {
        uint8_t len = dir[pos];
        if (!len) {
            pos = (pos + 2048) & ~2047u;
            continue;
        }
        if (pos + len <= size) {
            uint8_t nl = dir[pos + 32];
            char nm[64];
            size_t k;
            for (k = 0; k < nl && k < 63; k++)
                nm[k] = (char)dir[pos + 33 + k];
            nm[k] = 0;
            {
                char *sc = strchr(nm, ';');
                if (sc)
                    *sc = 0;
            }
            if (!strcmp(nm, name) && !(dir[pos + 25] & 2)) {
                out->lba = le32(dir + pos + 2);
                out->size = le32(dir + pos + 10);
                free(dir);
                return 0;
            }
        }
        pos += len;
    }
    free(dir);
    return -1;
}

/* ------------------------------------------------------------ crc */
static uint32_t crc_tab[256];
static uint32_t crc_upd(uint32_t c, const uint8_t *p, size_t n)
{
    size_t i;
    if (!crc_tab[1]) {
        uint32_t k, j;
        for (k = 0; k < 256; k++) {
            uint32_t v = k;
            for (j = 0; j < 8; j++)
                v = v & 1 ? 0xEDB88320u ^ (v >> 1) : v >> 1;
            crc_tab[k] = v;
        }
    }
    c = ~c;
    for (i = 0; i < n; i++)
        c = crc_tab[(c ^ p[i]) & 255] ^ (c >> 8);
    return ~c;
}

/* ------------------------------------------------------------ progress window */
static SDL_Window *pw;
static SDL_Renderer *pr;
static int cancelled, use_gui;

static void bar(double frac)
{
    SDL_Event ev;
    if (!pw)
        return;
    while (SDL_PollEvent(&ev))
        if (ev.type == SDL_QUIT)
            cancelled = 1;
    SDL_SetRenderDrawColor(pr, 24, 24, 32, 255);
    SDL_RenderClear(pr);
    {
        SDL_Rect frame = { 20, 40, 440, 32 }, fill = { 24, 44, (int)(432 * frac), 24 };
        SDL_SetRenderDrawColor(pr, 200, 200, 210, 255);
        SDL_RenderDrawRect(pr, &frame);
        SDL_SetRenderDrawColor(pr, 90, 170, 90, 255);
        SDL_RenderFillRect(pr, &fill);
    }
    SDL_RenderPresent(pr);
}

static int fail(const char *msg)
{
    fprintf(stderr, "install: %s\n", msg);
    rt_warn("install failed: %s", msg);
    if (use_gui)
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Monster Hunter data install", msg, pw);
    return -1;
}

int install_from_iso(const char *isopath, const char *dest, int gui)
{
    iso_t iso;
    iso_file fl[NWANT];
    uint64_t total = 0, done = 0;
    int k;
    char p[1100], msg[1300];
    static uint8_t buf[1 << 20];
    FILE *ok;
    uint32_t crc[NWANT];

    use_gui = gui;
    rt_log("install: reading %s into %s", rt_log_path(isopath), rt_log_path(dest));
    if (SDL_WasInit(0) == 0)
        SDL_Init(SDL_INIT_VIDEO);
    iso.f = open_big(isopath);
    if (!iso.f) {
        snprintf(msg, sizeof msg, "Cannot open %s.", isopath);
        return fail(msg);
    }
    for (k = 0; k < NWANT; k++) {
        int r = iso_find(&iso, want[k].name, &fl[k]);
        if (r == -2) {
            fclose(iso.f);
            return fail("This is not a plain ISO9660 disc image (2048-byte sectors). Use the .iso of the Japanese Monster Hunter PS2 disc.");
        }
        if (r != 0 && want[k].required) {
            fclose(iso.f);
            snprintf(msg, sizeof msg, "%s is not in this image: it is not the Japanese Monster Hunter (SLPM-65495) disc.", want[k].name);
            return fail(msg);
        }
        if (r != 0) {
            fl[k].size = 0;
            continue;
        }
        if (fl[k].size != want[k].size) {
            fclose(iso.f);
            snprintf(msg, sizeof msg, "%s has the wrong size (%u, expected %u): not the Japanese Monster Hunter (SLPM-65495) disc.",
                     want[k].name, (unsigned)fl[k].size, (unsigned)want[k].size);
            return fail(msg);
        }
        total += fl[k].size;
    }
    mkdirs(dest);
    if (!writable_dir(dest)) {
        fclose(iso.f);
        snprintf(msg, sizeof msg, "Cannot write to %s.", dest);
        return fail(msg);
    }
    if (gui && SDL_WasInit(SDL_INIT_VIDEO) && (pw = SDL_CreateWindow("Monster Hunter: installing game data (once) from your disc image",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 112, 0)) != NULL)
        pr = SDL_CreateRenderer(pw, -1, SDL_RENDERER_SOFTWARE);
    bar(0);
    for (k = 0; k < NWANT; k++) {
        FILE *o;
        uint32_t left = fl[k].size, c = 0;
        if (!left)
            continue;
        snprintf(p, sizeof p, "%s/%s.part", dest, want[k].name);
        o = fopen(p, "wb");
        if (!o) {
            fclose(iso.f);
            snprintf(msg, sizeof msg, "Cannot create %s.", p);
            return fail(msg);
        }
        seek64(iso.f, (uint64_t)fl[k].lba * 2048u);
        while (left && !cancelled) {
            size_t n = left < sizeof buf ? left : sizeof buf;
            if (fread(buf, 1, n, iso.f) != n || fwrite(buf, 1, n, o) != n) {
                fclose(o);
                fclose(iso.f);
                remove(p);
                snprintf(msg, sizeof msg, "Read / write error while copying %s (disk full?).", want[k].name);
                return fail(msg);
            }
            c = crc_upd(c, buf, n);
            left -= (uint32_t)n;
            done += n;
            bar((double)done / (double)total);
            if ((done >> 20) % 50 == 0)
                fprintf(stderr, "install: %d%%\r", (int)(done * 100 / total));
        }
        fclose(o);
        if (cancelled) {
            remove(p);
            fclose(iso.f);
            rt_warn("install cancelled");
            return -1;
        }
        crc[k] = c;
        if (c != want[k].crc) {
            fclose(iso.f);
            remove(p);
            snprintf(msg, sizeof msg, "%s does not match the known Japanese disc (checksum %08X, expected %08X): damaged image or another version.",
                     want[k].name, (unsigned)c, (unsigned)want[k].crc);
            return fail(msg);
        }
    }
    fclose(iso.f);
    for (k = 0; k < NWANT; k++) {
        char q[1100];
        if (!fl[k].size)
            continue;
        snprintf(p, sizeof p, "%s/%s.part", dest, want[k].name);
        snprintf(q, sizeof q, "%s/%s", dest, want[k].name);
        remove(q);
        if (rename(p, q) != 0)
            return fail("Cannot rename the copied files.");
    }
    snprintf(p, sizeof p, "%s/installed.ok", dest);
    ok = fopen(p, "wb");
    if (ok) {
        fprintf(ok, "mh1 data installed from a Japanese MH1 (SLPM-65495) disc image\n");
        for (k = 0; k < NWANT; k++)
            if (fl[k].size)
                fprintf(ok, "%s %u crc32=%08X\n", want[k].name, (unsigned)fl[k].size, (unsigned)crc[k]);
        fclose(ok);
    }
    if (pr) SDL_DestroyRenderer(pr);
    if (pw) SDL_DestroyWindow(pw);
    pr = NULL;
    pw = NULL;
    fprintf(stderr, "install: done, %u MB copied to %s\n", (unsigned)(done >> 20), dest);
    rt_log("install: OK, %u MB copied, checksums match the known disc; the ISO is not needed any more", (unsigned)(done >> 20));
    return 0;
}

/* ------------------------------------------------------------ asking for the ISO */
int install_prompt(char *out, size_t n)
{
    SDL_Window *w;
    int got = 0, running = 1;
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0 && SDL_Init(SDL_INIT_VIDEO) != 0)
        return -1;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Monster Hunter",
        "The game's data is not installed yet.\n\nDrag your own Japanese Monster Hunter (PS2) disc image (.iso) onto the next window"
#ifdef MH1_WINDOWS
        ", or press Enter to browse for it"
#endif
        ".\nIt is copied once (about 925 MB) and the ISO is not needed afterwards.", NULL);
    w = SDL_CreateWindow("Drop your Monster Hunter .iso here", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 160, 0);
    if (!w)
        return -1;
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    while (running) {
        SDL_Event ev;
        while (SDL_WaitEventTimeout(&ev, 200)) {
            if (ev.type == SDL_QUIT)
                running = 0;
            else if (ev.type == SDL_DROPFILE) {
                snprintf(out, n, "%s", ev.drop.file);
                SDL_free(ev.drop.file);
                got = 1;
                running = 0;
            }
#ifdef MH1_WINDOWS
            else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN) {
                OPENFILENAMEA o;
                char f[1024] = "";
                memset(&o, 0, sizeof o);
                o.lStructSize = sizeof o;
                o.lpstrFilter = "Disc image (*.iso)\0*.iso\0All files\0*.*\0";
                o.lpstrFile = f;
                o.nMaxFile = sizeof f;
                o.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                if (GetOpenFileNameA(&o)) {
                    snprintf(out, n, "%s", f);
                    got = 1;
                    running = 0;
                }
            }
#endif
        }
    }
    SDL_DestroyWindow(w);
    return got ? 0 : -1;
}
