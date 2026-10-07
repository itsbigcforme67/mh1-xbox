/*
 * mc_null.c - libmc (sceMc*) for the first Xbox link: no memory card
 * (the game says so and plays without saving). The real one will keep the
 * PS2 save files under E:\UDATA (docs/xbox.md), as rt_mc.c does on a host
 * directory.
 */
#include <stdint.h>
#include <string.h>

static int pending, last_cmd, last_res;
static int done(int cmd, int res) { pending = 1; last_cmd = cmd; last_res = res; return 0; }
const char *rt_mc_root(void) { return ""; }
int sceMcInit(void) { return 0; }
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
    (void)port; (void)slot;
    if (type)
        *type = 0;
    if (free_kb)
        *free_kb = 0;
    if (format)
        *format = 0;
    return done(1, -10);        /* no card */
}
int sceMcGetDir(int port, int slot, const char *name, unsigned mode, int maxent, void *table)
{
    (void)port; (void)slot; (void)name; (void)mode; (void)maxent; (void)table;
    return done(2, -10);
}
int sceMcOpen(int port, int slot, const char *name, int mode) { (void)port; (void)slot; (void)name; (void)mode; return done(3, -10); }
int sceMcClose(int fd) { (void)fd; return done(4, -10); }
int sceMcRead(int fd, void *buf, int size) { (void)fd; (void)buf; (void)size; return done(5, -10); }
int sceMcWrite(int fd, const void *buf, int size) { (void)fd; (void)buf; (void)size; return done(6, -10); }
int sceMcMkdir(int port, int slot, const char *name) { (void)port; (void)slot; (void)name; return done(0xB, -10); }
int sceMcSetFileInfo(int port, int slot, const char *name, const void *info, unsigned valid)
{
    (void)port; (void)slot; (void)name; (void)info; (void)valid;
    return done(0xC, -10);
}
int sceMcDelete(int port, int slot, const char *name) { (void)port; (void)slot; (void)name; return done(0xD, -10); }
int sceMcFormat(int port, int slot) { (void)port; (void)slot; return done(0x10, -10); }
int sceCdReadClock(uint8_t *c) { memset(c, 0, 8); return 1; }
void sceScfGetLocalTimefromRTC(void *c) { (void)c; }
