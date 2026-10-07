/*
 * mc_xbox.c - libmc (sceMc*) on the Xbox hard disk, the counterpart of rt_mc.c.
 *
 * The game's card code runs unchanged; it sees one card in port 0 that holds
 * the PS2 save directory BISLPM-65495MH (data file BISLPM-65495MH, icon.sys,
 * icon00.ico: byte for byte what rt_mc.c writes on the PC and the PS2 on a
 * card). On disk that directory is
 *
 *     E:\UDATA\<title id>\<save id>\{BISLPM-65495MH, icon.sys, icon00.ico, SaveMeta.xbx}
 *     E:\UDATA\<title id>\TitleMeta.xbx
 *
 * the layout the Xbox dashboard's memory manager reads (title id and save id
 * in xbox_title.h). SaveMeta.xbx / TitleMeta.xbx are UTF-16LE text with a
 * byte order mark ("Name=...", "TitleName=..."); the dashboard's icon images
 * (SaveImage.xbx / TitleImage.xbx) are not written, so it shows its default
 * icon. The meta files are hidden from the game's directory listings.
 *
 * Same asynchronous semantics as rt_mc.c: every command finishes at once and
 * its result is kept for the next sceMcSync. Not run on hardware.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "xbox_title.h"

#define CARD_KB 8000
#define ROOT "E:\\UDATA"

static int pending, last_cmd, last_res;
static int new_card = 1;
static FILE *fds[8];
static char title_dir[64], save_dir[96];

static void done(int cmd, int res) { pending = 1; last_cmd = cmd; last_res = res; }

static void ensure_dirs(void)
{
    if (title_dir[0])
        return;
    snprintf(title_dir, sizeof title_dir, "%s\\%s", ROOT, XBOX_TITLE_ID_STR);
    snprintf(save_dir, sizeof save_dir, "%s\\%s", title_dir, XBOX_SAVE_ID);
    CreateDirectoryA(ROOT, NULL);
    CreateDirectoryA(title_dir, NULL);
}

/* UTF-16LE text with a byte order mark, as the dashboard's .xbx metadata */
static void write_meta(const char *path, const char *text)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return;
    fputc(0xFF, f);
    fputc(0xFE, f);
    for (; *text; text++) {
        fputc((unsigned char)*text, f);
        fputc(0, f);
    }
    fclose(f);
}

static void write_title_meta(void)
{
    char p[128];
    snprintf(p, sizeof p, "%s\\TitleMeta.xbx", title_dir);
    if (GetFileAttributesA(p) == INVALID_FILE_ATTRIBUTES)
        write_meta(p, "TitleName=" XBOX_TITLE_NAME "\r\n");
}

static void write_save_meta(void)
{
    char p[160];
    snprintf(p, sizeof p, "%s\\SaveMeta.xbx", save_dir);
    if (GetFileAttributesA(p) == INVALID_FILE_ATTRIBUTES)
        write_meta(p, "Name=" XBOX_SAVE_NAME "\r\n");
}

/* card path ("BISLPM-65495MH/file", leading '/' allowed) -> disk path */
static void host_path(char *out, size_t n, const char *name)
{
    size_t k = strlen(PS2_SAVE_DIR), i;
    ensure_dirs();
    while (*name == '/')
        name++;
    if (!strncmp(name, PS2_SAVE_DIR, k) && (name[k] == 0 || name[k] == '/')) {
        snprintf(out, n, "%s%s", save_dir, name + k);
    } else {
        snprintf(out, n, "%s\\%s", title_dir, name);
    }
    for (i = 0; out[i]; i++)
        if (out[i] == '/')
            out[i] = '\\';
}

static int is_hidden(const char *name) { return !strcmp(name, "SaveMeta.xbx") || !strcmp(name, "TitleMeta.xbx"); }

static unsigned long long dir_bytes(const char *path)
{
    char pat[300];
    WIN32_FIND_DATAA fd;
    HANDLE h;
    unsigned long long sum = 0;
    snprintf(pat, sizeof pat, "%s\\*", path);
    h = FindFirstFileA(pat, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return 0;
    do {
        if (fd.cFileName[0] == '.')
            continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char sub[300];
            snprintf(sub, sizeof sub, "%s\\%s", path, fd.cFileName);
            sum += dir_bytes(sub) + 1024;
        } else {
            sum += (fd.nFileSizeLow + 1023ull) / 1024 * 1024;
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    return sum;
}

const char *rt_mc_root(void) { ensure_dirs(); return title_dir; }
int sceMcInit(void) { ensure_dirs(); write_title_meta(); return 0; }

int sceMcSync(int mode, int *cmd, int *res)
{
    (void)mode;
    if (!pending)
        return -1;
    pending = 0;
    if (cmd)
        *cmd = last_cmd;
    if (res)
        *res = last_res;
    return 1;
}

int sceMcGetInfo(int port, int slot, int *type, int *free_kb, int *format)
{
    (void)slot;
    if (port != 0) {
        if (type)
            *type = 0;
        if (free_kb)
            *free_kb = 0;
        if (format)
            *format = 0;
        done(1, -10);
        return 0;
    }
    ensure_dirs();
    if (type)
        *type = 2;
    if (format)
        *format = 1;
    if (free_kb) {
        unsigned long long used = dir_bytes(title_dir) / 1024;
        *free_kb = used >= CARD_KB ? 0 : (int)(CARD_KB - used);
    }
    done(1, new_card ? -1 : 0);
    new_card = 0;
    return 0;
}

/* FILETIME (100 ns since 1601) -> sceMcTblGetDir time (sec, min, hour, day, month, year) */
static void put_time(uint8_t *o, const FILETIME *ft)
{
    SYSTEMTIME st;
    memset(o, 0, 8);
    if (!FileTimeToSystemTime(ft, &st))
        return;
    o[1] = (uint8_t)st.wSecond;
    o[2] = (uint8_t)st.wMinute;
    o[3] = (uint8_t)st.wHour;
    o[4] = (uint8_t)st.wDay;
    o[5] = (uint8_t)st.wMonth;
    o[6] = (uint8_t)(st.wYear & 0xFF);
    o[7] = (uint8_t)(st.wYear >> 8);
}

static void put_ent(uint8_t *o, const char *name, const WIN32_FIND_DATAA *fd)
{
    int dir = (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    uint16_t attr = dir ? 0x8427 : 0x8497;
    uint32_t sz = dir ? 0 : fd->nFileSizeLow;
    memset(o, 0, 0x40);
    put_time(o, &fd->ftCreationTime);
    put_time(o + 8, &fd->ftLastWriteTime);
    memcpy(o + 0x10, &sz, 4);
    memcpy(o + 0x14, &attr, 2);
    snprintf((char *)o + 0x20, 32, "%s", name);
}

int sceMcGetDir(int port, int slot, const char *name, unsigned mode, int maxent, void *table)
{
    char path[300];
    WIN32_FIND_DATAA fd;
    HANDLE h;
    uint8_t *o = table;
    int n = 0;
    (void)slot;
    if (port != 0) {
        done(2, -10);
        return 0;
    }
    if (mode != 0) {
        done(2, 0);
        return 0;
    }
    host_path(path, sizeof path, name);
    h = FindFirstFileA(path, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        done(2, strpbrk(name, "*?") ? -4 : 0);
        return 0;
    }
    do {
        const char *nm = fd.cFileName;
        if (is_hidden(nm) || !strcmp(nm, ".") || !strcmp(nm, ".."))
            continue;
        if (!strcmp(nm, XBOX_SAVE_ID))
            nm = PS2_SAVE_DIR;
        if (n < maxent && o)
            put_ent(o + 0x40 * n, nm, &fd);
        n++;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    done(2, n);
    return 0;
}

int sceMcOpen(int port, int slot, const char *name, int mode)
{
    char path[300];
    const char *m;
    int fd;
    (void)slot;
    if (port != 0) {
        done(3, -10);
        return 0;
    }
    host_path(path, sizeof path, name);
    for (fd = 0; fd < 8 && fds[fd]; fd++)
        ;
    if (fd == 8) {
        done(3, -7);
        return 0;
    }
    if (mode & 0x200)
        m = GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES ? "r+b" : "w+b";
    else if ((mode & 3) == 1)
        m = "rb";
    else
        m = "r+b";
    if (!(fds[fd] = fopen(path, m))) {
        done(3, -4);
        return 0;
    }
    done(3, fd);
    return 0;
}

int sceMcClose(int fd)
{
    int i;
    if (fd < 0 || fd >= 8 || !fds[fd]) {      /* see rt_mc.c: mc_create_file closes an fd it never set */
        for (i = 0; i < 8; i++)
            if (fds[i]) {
                fclose(fds[i]);
                fds[i] = NULL;
            }
        done(4, 0);
        return 0;
    }
    fclose(fds[fd]);
    fds[fd] = NULL;
    done(4, 0);
    return 0;
}

int sceMcRead(int fd, void *buf, int size)
{
    if (fd < 0 || fd >= 8 || !fds[fd]) {
        done(5, -5);
        return 0;
    }
    done(5, (int)fread(buf, 1, (size_t)size, fds[fd]));
    return 0;
}

int sceMcWrite(int fd, const void *buf, int size)
{
    if (fd < 0 || fd >= 8 || !fds[fd]) {
        done(6, -5);
        return 0;
    }
    size = (int)fwrite(buf, 1, (size_t)size, fds[fd]);
    fflush(fds[fd]);
    done(6, size);
    return 0;
}

int sceMcMkdir(int port, int slot, const char *name)
{
    char path[300];
    (void)slot;
    if (port != 0) {
        done(0xB, -10);
        return 0;
    }
    host_path(path, sizeof path, name);
    if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) {
        done(0xB, -4);
        return 0;
    }
    if (!CreateDirectoryA(path, NULL)) {
        done(0xB, -5);
        return 0;
    }
    if (!strcmp(path, save_dir))
        write_save_meta();                    /* the dashboard lists the save from this */
    done(0xB, 0);
    return 0;
}

int sceMcSetFileInfo(int port, int slot, const char *name, const void *info, unsigned valid)
{
    (void)port; (void)slot; (void)name; (void)info; (void)valid;
    done(0xC, 0);
    return 0;
}

int sceMcDelete(int port, int slot, const char *name)
{
    char path[300];
    DWORD a;
    (void)slot;
    if (port != 0) {
        done(0xD, -10);
        return 0;
    }
    host_path(path, sizeof path, name);
    a = GetFileAttributesA(path);
    if (a == INVALID_FILE_ATTRIBUTES) {
        done(0xD, -4);
        return 0;
    }
    if (a & FILE_ATTRIBUTE_DIRECTORY) {
        if (!strcmp(path, save_dir)) {        /* deleting the save: its meta file goes too */
            char m[160];
            snprintf(m, sizeof m, "%s\\SaveMeta.xbx", save_dir);
            DeleteFileA(m);
        }
        done(0xD, RemoveDirectoryA(path) ? 0 : -4);
    } else {
        done(0xD, DeleteFileA(path) ? 0 : -4);
    }
    return 0;
}

int sceMcFormat(int port, int slot)
{
    (void)port; (void)slot;
    done(0x10, 0);
    return 0;
}

/* sceCdReadClock: the RTC in BCD (see rt_mc.c) */
static int bcd(int v) { return (v / 10) << 4 | v % 10; }
int sceCdReadClock(uint8_t *c)
{
    SYSTEMTIME t;
    GetLocalTime(&t);
    c[0] = 0;
    c[1] = (uint8_t)bcd(t.wSecond);
    c[2] = (uint8_t)bcd(t.wMinute);
    c[3] = (uint8_t)bcd(t.wHour);
    c[4] = 0;
    c[5] = (uint8_t)bcd(t.wDay);
    c[6] = (uint8_t)bcd(t.wMonth);
    c[7] = (uint8_t)bcd(t.wYear % 100);
    return 1;
}

void sceScfGetLocalTimefromRTC(void *c) { (void)c; }
