/*
 * rt_mc.c - the PS2 memory card library (libmc: sceMc*) on host files.
 *
 * The game's own card code runs unchanged on the PC: the save screens
 * (src/main/mc/mccomb.c: McOperationSet / McCardOperation), the action
 * layer (mcact_nm.c: McAct*) and the low-level step machines (mclow_nm.c:
 * mc_check_card, mc_read_file, ...). Only the library under them is new:
 * memory card port 0 is a directory on the host, port 1 has no card.
 *
 *   $MH1_SAVE_DIR, else $XDG_DATA_HOME/mh1pc/memcard0,
 *   else ~/.local/share/mh1pc/memcard0
 *
 * The game's save is the directory BISLPM-65495MH there (mc_file_tbl[0]:
 * the data file BISLPM-65495MH, 0x11450 bytes, encoded by encode_data;
 * icon.sys and icon00.ico), byte for byte what the PS2 writes, so the PS2
 * save layout (User_data, the three character slots, options) is kept.
 *
 * libmc is asynchronous: each call starts a command and sceMcSync later
 * returns its result. Here every command finishes at once; its result is
 * kept for the next sceMcSync (1 = finished, -1 = nothing running), the
 * semantics mc_sync (mclow) relies on. Results, read from how mclow and
 * mcact use them: GetInfo 0 (or -1 the first time: "new card"), type 2 =
 * PS2 card, format 1, free in KB (8000 total); GetDir = entry count;
 * Open = fd or -4 (no such file); Read/Write = bytes; Mkdir 0 or -4
 * (exists, mc_mkdir treats it as success).
 */
#define _DEFAULT_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define CARD_KB 8000

static int pending, last_cmd, last_res;
static int new_card[2] = { 1, 1 };
static char root[512];
static FILE *fds[8];

static int trace(void)
{
    static int t = -1;
    if (t < 0)
        t = getenv("RT_MC_TRACE") != NULL;
    return t;
}

static void done(int cmd, int res)
{
    pending = 1;
    last_cmd = cmd;
    last_res = res;
    if (trace())
        fprintf(stderr, "rt_mc: cmd %d -> %d\n", cmd, res);
}

static void mkdirs(const char *p)
{
    char tmp[600];
    char *s;
    snprintf(tmp, sizeof tmp, "%s", p);
    for (s = tmp + 1; *s; s++)
        if (*s == '/') {
            *s = 0;
            mkdir(tmp, 0755);
            *s = '/';
        }
    mkdir(tmp, 0755);
}

/* The host directory that stands for the card in port 0. */
const char *rt_mc_root(void)
{
    if (!root[0]) {
        const char *e = getenv("MH1_SAVE_DIR"), *x = getenv("XDG_DATA_HOME"), *h = getenv("HOME");
        if (e && *e)
            snprintf(root, sizeof root, "%s", e);
        else if (x && *x)
            snprintf(root, sizeof root, "%s/mh1pc/memcard0", x);
        else
            snprintf(root, sizeof root, "%s/.local/share/mh1pc/memcard0", h ? h : ".");
        mkdirs(root);
    }
    return root;
}

/* card path (with or without a leading '/') -> host path */
static void host_path(char *out, size_t n, const char *name)
{
    while (*name == '/')
        name++;
    snprintf(out, n, "%s/%s", rt_mc_root(), name);
}

static long dir_bytes(const char *path)
{
    DIR *d = opendir(path);
    struct dirent *e;
    long sum = 0;
    char p[1024];
    struct stat st;
    if (!d)
        return 0;
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.')
            continue;
        snprintf(p, sizeof p, "%s/%s", path, e->d_name);
        if (stat(p, &st) == 0)
            sum += S_ISDIR(st.st_mode) ? dir_bytes(p) + 1024 : (st.st_size + 1023) / 1024 * 1024;
    }
    closedir(d);
    return sum;
}

int sceMcInit(void) { return 0; }

int sceMcSync(int mode, int *cmd, int *res)
{
    (void)mode;
    if (!pending)
        return -1;              /* sceMcExecIdle */
    pending = 0;
    if (cmd)
        *cmd = last_cmd;
    if (res)
        *res = last_res;
    return 1;                   /* sceMcExecFinish */
}

int sceMcGetInfo(int port, int slot, int *type, int *free_kb, int *format)
{
    (void)slot;
    if (port != 0) {            /* no card in port 1 */
        if (type)
            *type = 0;
        if (free_kb)
            *free_kb = 0;
        if (format)
            *format = 0;
        done(1, -10);
        return 0;
    }
    if (type)
        *type = 2;
    if (format)
        *format = 1;
    if (free_kb) {
        long used = dir_bytes(rt_mc_root()) / 1024;
        *free_kb = used >= CARD_KB ? 0 : (int)(CARD_KB - used);
    }
    done(1, new_card[0] ? -1 : 0);
    new_card[0] = 0;
    return 0;
}

/* sceMcTblGetDir (0x40 bytes): created(8) modified(8) size(4) attr(2)
 * reserve(2) reserve(4) PdaAplNo(4) name(32) */
static void put_time(uint8_t *o, time_t t)
{
    struct tm tm;
    localtime_r(&t, &tm);
    o[0] = 0;
    o[1] = (uint8_t)tm.tm_sec;
    o[2] = (uint8_t)tm.tm_min;
    o[3] = (uint8_t)tm.tm_hour;
    o[4] = (uint8_t)tm.tm_mday;
    o[5] = (uint8_t)(tm.tm_mon + 1);
    o[6] = (uint8_t)((tm.tm_year + 1900) & 0xFF);
    o[7] = (uint8_t)((tm.tm_year + 1900) >> 8);
}

static void put_ent(uint8_t *o, const char *name, const struct stat *st)
{
    uint16_t attr = S_ISDIR(st->st_mode) ? 0x8427 : 0x8497;  /* readable/writable/exists, dir or file */
    uint32_t sz = S_ISDIR(st->st_mode) ? 0 : (uint32_t)st->st_size;
    memset(o, 0, 0x40);
    put_time(o, st->st_ctime);
    put_time(o + 8, st->st_mtime);
    memcpy(o + 0x10, &sz, 4);
    memcpy(o + 0x14, &attr, 2);
    snprintf((char *)o + 0x20, 32, "%s", name);
}

int sceMcGetDir(int port, int slot, const char *name, unsigned mode, int maxent, void *table)
{
    char path[1024], dirp[1024], pat[256];
    const char *slash;
    struct stat st;
    uint8_t *o = table;
    int n = 0;
    (void)slot;
    if (port != 0) {
        done(2, -10);
        return 0;
    }
    if (mode != 0) {            /* continued listing: everything was given at once */
        done(2, 0);
        return 0;
    }
    host_path(path, sizeof path, name);
    if (!strpbrk(name, "*?")) {     /* one entry: the name itself */
        slash = strrchr(path, '/');
        if (stat(path, &st) == 0) {
            if (maxent > 0 && o)
                put_ent(o, slash + 1, &st);
            n = 1;
        }
        done(2, n);
        return 0;
    }
    slash = strrchr(path, '/');
    snprintf(dirp, sizeof dirp, "%.*s", (int)(slash - path), path);
    snprintf(pat, sizeof pat, "%s", slash + 1);
    {
        DIR *d = opendir(dirp);
        struct dirent *e;
        if (!d) {
            done(2, -4);
            return 0;
        }
        if (!strcmp(pat, "*")) {    /* a directory listing also has "." and ".." */
            if (stat(dirp, &st) == 0 && n < maxent) {
                put_ent(o + 0x40 * n++, ".", &st);
                if (n < maxent)
                    put_ent(o + 0x40 * n++, "..", &st);
            }
        }
        while ((e = readdir(d)) != NULL && n < maxent) {
            char p[1300];
            if (e->d_name[0] == '.' || fnmatch(pat, e->d_name, 0) != 0)
                continue;
            snprintf(p, sizeof p, "%s/%s", dirp, e->d_name);
            if (stat(p, &st) == 0)
                put_ent(o + 0x40 * n++, e->d_name, &st);
        }
        closedir(d);
    }
    done(2, n);
    return 0;
}

/* modes: 1 read, 2 write, 3 read/write, 0x200 create */
int sceMcOpen(int port, int slot, const char *name, int mode)
{
    char path[1024];
    const char *m;
    int fd;
    struct stat st;
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
        m = stat(path, &st) == 0 ? "r+b" : "w+b";
    else if ((mode & 3) == 1)
        m = "rb";
    else
        m = "r+b";
    if (!(fds[fd] = fopen(path, m))) {
        done(3, -4);
        return 0;
    }
    if (trace())
        fprintf(stderr, "rt_mc: open %s mode %X -> fd %d\n", path, mode, fd);
    done(3, fd);
    return 0;
}

int sceMcClose(int fd)
{
    int i;
    if (fd < 0 || fd >= 8 || !fds[fd]) {
        /* mc_create_file closes w->fd, which it never set from the open's
         * result: close whatever is open (only one file at a time is) */
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
    size_t n;
    if (fd < 0 || fd >= 8 || !fds[fd]) {
        done(5, -5);
        return 0;
    }
    n = fread(buf, 1, (size_t)size, fds[fd]);
    done(5, (int)n);
    return 0;
}

int sceMcWrite(int fd, const void *buf, int size)
{
    size_t n;
    if (fd < 0 || fd >= 8 || !fds[fd]) {
        done(6, -5);
        return 0;
    }
    n = fwrite(buf, 1, (size_t)size, fds[fd]);
    fflush(fds[fd]);
    done(6, (int)n);
    return 0;
}

int sceMcMkdir(int port, int slot, const char *name)
{
    char path[1024];
    struct stat st;
    (void)slot;
    if (port != 0) {
        done(0xB, -10);
        return 0;
    }
    host_path(path, sizeof path, name);
    if (stat(path, &st) == 0) {
        done(0xB, -4);
        return 0;
    }
    done(0xB, mkdir(path, 0755) == 0 ? 0 : -5);
    return 0;
}

int sceMcSetFileInfo(int port, int slot, const char *name, const void *info, unsigned valid)
{
    (void)port; (void)slot; (void)name; (void)info; (void)valid;
    done(0xC, 0);               /* attributes are not kept on the host */
    return 0;
}

int sceMcDelete(int port, int slot, const char *name)
{
    char path[1024];
    (void)slot;
    if (port != 0) {
        done(0xD, -10);
        return 0;
    }
    host_path(path, sizeof path, name);
    done(0xD, remove(path) == 0 ? 0 : -4);
    return 0;
}

int sceMcFormat(int port, int slot)
{
    (void)port; (void)slot;
    done(0x10, 0);              /* the host card is always formatted */
    return 0;
}

/* sceCdReadClock: the RTC in BCD (second, minute, hour, pad, day, month,
 * year) at +1..+7, as McReadClock reads it; returns 1 = read. The local
 * time is given and sceScfGetLocalTimefromRTC leaves it as it is. */
static int bcd(int v) { return (v / 10) << 4 | v % 10; }
int sceCdReadClock(uint8_t *c)
{
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    c[0] = 0;
    c[1] = (uint8_t)bcd(tm.tm_sec);
    c[2] = (uint8_t)bcd(tm.tm_min);
    c[3] = (uint8_t)bcd(tm.tm_hour);
    c[4] = 0;
    c[5] = (uint8_t)bcd(tm.tm_mday);
    c[6] = (uint8_t)bcd(tm.tm_mon + 1);
    c[7] = (uint8_t)bcd(tm.tm_year % 100);
    return 1;
}

void sceScfGetLocalTimefromRTC(void *c) { (void)c; }
