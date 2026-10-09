/*
 * win_utf8.h - Windows build only (force-included by tools/build_win.sh into the host-side C, not the game C).
 * The program handles every path as UTF-8 (SDL, the logs, the command line); the C runtime's fopen / stat /
 * getenv take the ANSI code page, which breaks on "C:\\Users\\José" or a Japanese folder name. These map the
 * calls to the wide-character functions. Header-only, static inline.
 */
#ifndef WIN_UTF8_H
#define WIN_UTF8_H
#if defined(_WIN32) && defined(MH1_WIN) && !defined(MH1_NO_UTF8)
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#include <dirent.h>
#undef near          /* windows.h macros that clash with names in the game runtime */
#undef far
#undef min
#undef max
#undef small
#undef interface

static inline wchar_t *mh1_u8w(const char *s, wchar_t *w, int n)
{
    if (!MultiByteToWideChar(CP_UTF8, 0, s, -1, w, n))
        MultiByteToWideChar(CP_ACP, 0, s, -1, w, n);      /* not valid UTF-8: the ANSI reading */
    return w;
}
static inline char *mh1_w8u(const wchar_t *w, char *s, int n)
{
    WideCharToMultiByte(CP_UTF8, 0, w, -1, s, n, NULL, NULL);
    return s;
}
static inline FILE *mh1_fopen(const char *p, const char *m)
{
    wchar_t wp[1100], wm[16];
    return _wfopen(mh1_u8w(p, wp, 1100), mh1_u8w(m, wm, 16));
}
static inline int mh1_remove(const char *p) { wchar_t w[1100]; return _wremove(mh1_u8w(p, w, 1100)); }
static inline int mh1_rename(const char *a, const char *b)
{
    wchar_t wa[1100], wb[1100];
    return MoveFileExW(mh1_u8w(a, wa, 1100), mh1_u8w(b, wb, 1100), MOVEFILE_REPLACE_EXISTING) ? 0 : -1;
}
static inline int mh1_mkdir(const char *p) { wchar_t w[1100]; return _wmkdir(mh1_u8w(p, w, 1100)); }
static inline int mh1_rmdir(const char *p) { wchar_t w[1100]; return _wrmdir(mh1_u8w(p, w, 1100)); }
static inline int mh1_stat(const char *p, struct stat *st) { wchar_t w[1100]; return _wstat64i32(mh1_u8w(p, w, 1100), (struct _stat64i32 *)st); }
static inline const char *mh1_getenv(const char *name)
{
    static char buf[8][1100];
    static int k;
    wchar_t wn[64], *v;
    if (name[0] == 'R' && name[1] == 'T' && name[2] == '_')
        return getenv(name);             /* the test aids are ASCII; this is called every frame in places */
    v = _wgetenv(mh1_u8w(name, wn, 64));
    if (!v)
        return NULL;
    k = (k + 1) & 7;
    return mh1_w8u(v, buf[k], 1100);
}
/* directory listing with UTF-8 names (only d_name is used) */
struct mh1_dirent { char d_name[1100]; };
typedef struct { _WDIR *d; struct mh1_dirent e; } MH1_DIR;
static inline MH1_DIR *mh1_opendir(const char *p)
{
    wchar_t w[1100];
    MH1_DIR *d = (MH1_DIR *)calloc(1, sizeof *d);
    if (!d)
        return NULL;
    d->d = _wopendir(mh1_u8w(p, w, 1100));
    if (!d->d) {
        free(d);
        return NULL;
    }
    return d;
}
static inline struct mh1_dirent *mh1_readdir(MH1_DIR *d)
{
    struct _wdirent *e = _wreaddir(d->d);
    if (!e)
        return NULL;
    mh1_w8u(e->d_name, d->e.d_name, sizeof d->e.d_name);
    return &d->e;
}
static inline int mh1_closedir(MH1_DIR *d) { int r = _wclosedir(d->d); free(d); return r; }

#define fopen mh1_fopen
#define remove mh1_remove
#define rename mh1_rename
#define stat(p, s) mh1_stat((p), (s))
#define getenv mh1_getenv
#define DIR MH1_DIR
#define dirent mh1_dirent
#define opendir mh1_opendir
#define readdir mh1_readdir
#define closedir mh1_closedir
#define mkdir(p, m) mh1_mkdir(p)
#define rmdir(p) mh1_rmdir(p)
#endif
#endif
