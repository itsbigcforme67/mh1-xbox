/*
 * rt_log.c - the automatic debug log (every run, no switch needed).
 *
 * One file per session, logs/mh1_YYYYMMDD_HHMMSS.log, in a folder next to the
 * save directory (Linux ~/.local/share/mh1pc/logs, Windows %APPDATA%\mh1pc\logs,
 * Xbox E:\Games\MH1\logs, or $MH1_LOG_DIR); the newest 20 are kept. It holds
 * a header (build, OS, GPU, window, controller, audio), the game's events
 * (boot, mode / stage changes, quest start / clear / fail, save and load
 * results, movies), warnings (missing files, no-op stand-ins called, audio
 * underruns), a fps / CPU line once a minute, and on a crash the signal, a
 * backtrace (names from the build's own symbol table, rt_symtab.c), the last
 * 200 log lines and the game's mode / stage / quest.
 *
 * Privacy: nothing is sent anywhere; no memory dump is written (the crash
 * handler ends the process itself, so no core file / Windows error report);
 * user names and home directories are scrubbed from every line (~).
 *
 * Cheap: a buffered stdio file, flushed on warnings and every 3 seconds,
 * fsynced on a crash.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE        /* ucontext REG_EIP */
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#include "rt_plat.h"
#include "rt_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(MH1_WINDOWS) || defined(MH1_XBOX)
#include <windows.h>
#ifdef MH1_WINDOWS
#include <io.h>
#include <signal.h>
#endif
#else
#include <dirent.h>
#include <execinfo.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <ucontext.h>
#include <unistd.h>
#endif

const char *rt_mc_root(void);                       /* rt_mc.c / mc_xbox.c: the save directory */
const char *rt_host_symname(const void *addr, unsigned *off);   /* rt_symtab.c (tools/gen_symtab.py) */

#ifndef MH1_VERSION
#define MH1_VERSION "unknown"
#endif
#define KEEP_LOGS 20
#define RING_LINES 200
#define LINE_MAX_ 240

static FILE *lf;
static int lfd = -1;
static char log_path[700];
static char ring[RING_LINES][LINE_MAX_];
static unsigned ring_n;                 /* lines ever written */
static unsigned long t0;                /* ms at start */
static int in_crash, inited;
static char home[300];                  /* the user's home / profile directory (scrubbed from lines) */
static unsigned long last_flush;
static unsigned n_warn, n_standin, n_frames_total;
static int cur_mode = -1, cur_step, cur_stage = -1, cur_quest, cur_tick, cur_result;
static const char *const mode_name[] = { "init", "loading", "quest", "quest clear", "waiting for players", "result", "village" };

/* ------------------------------------------------------------ platform */
static unsigned long now_ms(void)
{
#if defined(MH1_WINDOWS) || defined(MH1_XBOX)
    return (unsigned long)GetTickCount();
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (unsigned long)(t.tv_sec * 1000UL + t.tv_nsec / 1000000UL);
#endif
}

static unsigned long cpu_ms(void)               /* CPU time of the whole process */
{
#ifdef MH1_WINDOWS
    FILETIME c, e, k, u;
    if (GetProcessTimes(GetCurrentProcess(), &c, &e, &k, &u))
        return (unsigned long)((((unsigned long long)k.dwHighDateTime << 32 | k.dwLowDateTime)
                                + ((unsigned long long)u.dwHighDateTime << 32 | u.dwLowDateTime)) / 10000);
    return 0;
#elif defined(MH1_XBOX)
    return 0;
#else
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (unsigned long)(t.tv_sec * 1000UL + t.tv_nsec / 1000000UL);
#endif
}

static void stamp(char *out, size_t n)          /* local date and time: YYYYMMDD_HHMMSS */
{
#if defined(MH1_WINDOWS) || defined(MH1_XBOX)
    SYSTEMTIME s;
    GetLocalTime(&s);
    snprintf(out, n, "%04d%02d%02d_%02d%02d%02d", s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond);
#else
    time_t t = time(NULL);
    struct tm tmv;
    localtime_r(&t, &tmv);
    strftime(out, n, "%Y%m%d_%H%M%S", &tmv);
#endif
}

static void make_dirs(const char *path)
{
    char tmp[700];
    char *s;
    snprintf(tmp, sizeof tmp, "%s", path);
    for (s = tmp + 1; *s; s++)
        if (*s == '/' || *s == '\\') {
            char c = *s;
            *s = 0;
#if defined(MH1_XBOX)
            CreateDirectoryA(tmp, NULL);
#else
            mkdir(tmp, 0755);
#endif
            *s = c;
        }
#if defined(MH1_XBOX)
    CreateDirectoryA(tmp, NULL);
#else
    mkdir(tmp, 0755);
#endif
}

static int cmp_str(const void *a, const void *b) { return strcmp((const char *)a, (const char *)b); }

/* delete the oldest session logs so that at most KEEP_LOGS - 1 stay (the new one makes KEEP_LOGS) */
static void rotate(const char *dir)
{
    static char names[256][48];
    int n = 0, i;
#if defined(MH1_XBOX)
    WIN32_FIND_DATAA fd;
    char pat[720];
    HANDLE h;
    snprintf(pat, sizeof pat, "%s\\mh1_*.log", dir);
    h = FindFirstFileA(pat, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (n < 256 && strlen(fd.cFileName) < 48)
                strcpy(names[n++], fd.cFileName);
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
#else
    DIR *d = opendir(dir);
    struct dirent *e;
    if (d) {
        while ((e = readdir(d)) != NULL)
            if (!strncmp(e->d_name, "mh1_", 4) && strlen(e->d_name) > 4 && !strcmp(e->d_name + strlen(e->d_name) - 4, ".log")
                && n < 256 && strlen(e->d_name) < 48)
                strcpy(names[n++], e->d_name);
        closedir(d);
    }
#endif
    qsort(names, (size_t)n, sizeof names[0], cmp_str);      /* the names sort by date */
    for (i = 0; i < n - (KEEP_LOGS - 1); i++) {
        char p[760];
        snprintf(p, sizeof p, "%s/%s", dir, names[i]);
        remove(p);
    }
}

/* ------------------------------------------------------------ scrubbing */
/* replace user names / home directories in a text line by ~ */
static void replace_all(char *s, const char *from, size_t fl, const char *to)
{
    char *p;
    size_t tl = strlen(to);
    while ((p = strstr(s, from)) != NULL) {
        memmove(p + tl, p + fl, strlen(p + fl) + 1);
        memcpy(p, to, tl);
        s = p + tl;
    }
}

static int users_at(const char *s)
{
    return (s[0] | 32) == 'u' && (s[1] | 32) == 's' && (s[2] | 32) == 'e' && (s[3] | 32) == 'r' && (s[4] | 32) == 's';
}

static void scrub(char *s)
{
    char *p;
    if (home[0])
        replace_all(s, home, strlen(home), "~");
    /* anything else that looks like a home directory: /home/NAME, /Users/NAME, X:\Users\NAME (either slash) */
    for (p = s; *p; p++) {
        char *q = NULL;
        if (!strncmp(p, "/home/", 6) || !strncmp(p, "/Users/", 7)) {
            q = p + (p[1] == 'h' ? 6 : 7);
            while (*q && *q != '/' && *q != ' ' && *q != '"')
                q++;
            memmove(p + 1, q, strlen(q) + 1);
            p[0] = '~';
        } else if (p[0] == ':' && (p[1] == '\\' || p[1] == '/') && p > s && users_at(p + 2) && (p[7] == '\\' || p[7] == '/')) {
            q = p + 8;
            while (*q && *q != '\\' && *q != '/' && *q != ' ' && *q != '"')
                q++;
            memmove(p - 1, q, strlen(q) + 1);
            p[-1] = '~';
            p--;
        }
    }
}

const char *rt_log_path(const char *path)
{
    static char buf[4][700];
    static int k;
    char *b = buf[k++ & 3];
    snprintf(b, 700, "%s", path ? path : "(null)");
    scrub(b);
    return b;
}

/* ------------------------------------------------------------ writing */
static void put_line(char level, const char *text)
{
    char line[LINE_MAX_ + 40];
    unsigned long ms = now_ms() - t0;
    int n = snprintf(line, sizeof line, "%7lu.%03lu t%-6d %c %s", ms / 1000, ms % 1000, cur_tick, level, text);
    char *r = ring[ring_n % RING_LINES];
    if (n >= (int)sizeof line)
        n = (int)sizeof line - 1;
    scrub(line);
    n = (int)strlen(line);
    if (n > LINE_MAX_ - 2)
        n = LINE_MAX_ - 2;
    line[n] = '\n';
    line[n + 1] = 0;
    memcpy(r, line, (size_t)n + 2);
    ring_n++;
    if (lf) {
        fwrite(line, 1, (size_t)n + 1, lf);
        if (level == 'W' || now_ms() - last_flush > 3000) {
            fflush(lf);
            last_flush = now_ms();
        }
    }
}

static void vlog(char level, const char *fmt, va_list ap)
{
    char text[LINE_MAX_];
    vsnprintf(text, sizeof text, fmt, ap);
    put_line(level, text);
}

void rt_log(const char *fmt, ...)
{
    va_list ap;
    if (in_crash)
        return;
    va_start(ap, fmt);
    vlog('I', fmt, ap);
    va_end(ap);
}

void rt_warn(const char *fmt, ...)
{
    va_list ap;
    if (in_crash)
        return;
    n_warn++;
    va_start(ap, fmt);
    vlog('W', fmt, ap);
    va_end(ap);
}

void rt_warn_once(const char *key, const char *fmt, ...)
{
    static char seen[64][64];
    static int ns;
    int i;
    va_list ap;
    if (in_crash)
        return;
    for (i = 0; i < ns; i++)
        if (!strncmp(seen[i], key, 63))
            return;
    if (ns < 64)
        snprintf(seen[ns++], 64, "%s", key);
    n_warn++;
    va_start(ap, fmt);
    vlog('W', fmt, ap);
    va_end(ap);
}

void rt_log_standin(const char *name)
{
    /* the first call of each no-op stand-in; the stand-ins are few hundred at most */
    n_standin++;
    rt_warn("stand-in called (not ported, does nothing): %s", name);
}

/* ------------------------------------------------------------ game state */
void rt_log_game(int mode, int step, int stage, int quest, int result)
{
    static int last_quest_mode = -1;
    cur_tick++;
    cur_step = step;
    cur_result = result;
    if (stage != cur_stage) {
        if (cur_stage >= 0 || mode >= 2)
            rt_log("stage %d -> %d (mode %d)", cur_stage, stage, mode);
        cur_stage = stage;
    }
    if (mode != cur_mode) {
        const char *nm = mode >= 0 && mode < 7 ? mode_name[mode] : "?";
        rt_log("mode %d -> %d (%s), step %d, stage %d, quest %d", cur_mode, mode, nm, step, stage, quest);
        if (mode == 2 && cur_mode != 2) {
            cur_quest = quest;
            rt_log("quest %d START (stage %d)", quest, stage);
            last_quest_mode = 2;
        } else if (cur_mode == 2 && mode == 3) {
            rt_log("quest %d CLEAR (result code %d)", cur_quest, result);
        } else if (cur_mode == 2 && mode == 5) {
            rt_log("quest %d %s (result code %d)", cur_quest, result == 6 || result == 7 ? "FAILED" : "ended", result);
        } else if (cur_mode == 3 && mode == 5) {
            rt_log("quest %d reward screen", cur_quest);
        } else if (mode == 6 && last_quest_mode == 2) {
            rt_log("quest %d over, back in the village", cur_quest);
            last_quest_mode = -1;
        }
        cur_mode = mode;
    }
}

/* one tick of the boot sequence (the game modes call rt_log_game instead) */
void rt_log_boot_tick(void) { cur_tick++; }

/* ------------------------------------------------------------ periodic */
static volatile int au_late, au_calls, au_late_total;
static volatile double au_worst_gap, au_worst_work;
static volatile int au_period = 21;
void rt_log_audio_cb(double gap_ms, double work_ms, int period_ms)
{
    au_calls++;
    au_period = period_ms;
    if (gap_ms > au_worst_gap)
        au_worst_gap = gap_ms;
    if (work_ms > au_worst_work)
        au_worst_work = work_ms;
    if (gap_ms > 2.5 * period_ms || work_ms > period_ms) {
        au_late++;
        au_late_total++;
    }
}

static void crash_test(void)
{
    const char *e = getenv("RT_CRASH_TEST");
    if (!e || n_frames_total != 60)
        return;
    rt_log("RT_CRASH_TEST=%s: forcing a crash now", e);
    if (e[0] == '2')
        abort();
    *(volatile int *)0 = 1;
}

void rt_log_frame(void)
{
    static unsigned long sum_t0, last_frame, worst;
    static unsigned frames;
    static unsigned long sum_cpu0;
    static int last_ticks;
    unsigned long now = now_ms();
    if (!inited)
        return;
    n_frames_total++;
    if (!sum_t0) {
        sum_t0 = now;
        sum_cpu0 = cpu_ms();
        last_ticks = cur_tick;
    }
    if (last_frame && now - last_frame > worst)
        worst = now - last_frame;
    last_frame = now;
    frames++;
    crash_test();
    if (lf && now - last_flush > 3000) {
        fflush(lf);
        last_flush = now;
    }
    if (au_late) {      /* the audio thread cannot log itself: report from here (at most once a second) */
        static unsigned long last_au;
        if (now - last_au > 1000) {
            rt_warn("audio underrun: %d late callbacks (worst gap %.0f ms, worst mix %.1f ms, period %d ms)", au_late,
                    (double)au_worst_gap, (double)au_worst_work, au_period);
            au_late = 0;
            last_au = now;
        }
    }
    if (now - sum_t0 >= 60000) {
        unsigned long cpu = cpu_ms();
        double wall = (double)(now - sum_t0);
        rt_log("summary: %.1f fps (%u frames), worst frame %lu ms, %d game ticks, cpu %.0f%% of one core, "
               "mode %d, stage %d, quest %d, warnings %u, audio late %d of %d",
               frames * 1000.0 / wall, frames, worst, cur_tick - last_ticks, (cpu - sum_cpu0) * 100.0 / wall,
               cur_mode, cur_stage, cur_quest, n_warn, au_late_total, au_calls);
        sum_t0 = now;
        sum_cpu0 = cpu;
        frames = 0;
        worst = 0;
        last_ticks = cur_tick;
        au_worst_gap = au_worst_work = 0;
    }
}

/* ------------------------------------------------------------ crash report */
static void raw_write(const char *s, size_t n)
{
#if defined(MH1_WINDOWS)
    if (lfd >= 0)
        _write(lfd, s, (unsigned)n);
#elif defined(MH1_XBOX)
    (void)s; (void)n;
#else
    if (lfd >= 0) {
        ssize_t r = write(lfd, s, n);
        (void)r;
    }
#endif
}
static void raw_puts(const char *s) { raw_write(s, strlen(s)); }

static void crash_report(const char *what, void *const *frames, int nf)
{
    char b[400];
    int i;
    unsigned long ms = now_ms() - t0;
    raw_puts("\n===== CRASH =====\n");
    snprintf(b, sizeof b, "reason: %s\nbuild: %s\nuptime: %lu.%03lu s, tick %d, drawn frames %u\n", what, MH1_VERSION, ms / 1000, ms % 1000,
             cur_tick, n_frames_total);
    raw_puts(b);
    snprintf(b, sizeof b, "game: mode %d (%s), step %d, stage %d, quest %d, result code %d\n", cur_mode,
             cur_mode >= 0 && cur_mode < 7 ? mode_name[cur_mode] : "boot / unknown", cur_step, cur_stage, cur_quest, cur_result);
    raw_puts(b);
    raw_puts("backtrace (names from the build's symbol table; static functions show as the symbol before them):\n");
    for (i = 0; i < nf; i++) {
        unsigned off = 0;
        const char *nm = rt_host_symname(frames[i], &off);
        if (nm)
            snprintf(b, sizeof b, "  #%d %p %s+0x%x\n", i, frames[i], nm, off);
        else
            snprintf(b, sizeof b, "  #%d %p\n", i, frames[i]);
        raw_puts(b);
    }
    snprintf(b, sizeof b, "last %u log lines (the stdio buffer is not flushed on a crash, so these repeat the file's tail):\n",
             ring_n < RING_LINES ? ring_n : RING_LINES);
    raw_puts(b);
    for (i = ring_n < RING_LINES ? 0 : (int)(ring_n - RING_LINES); (unsigned)i < ring_n; i++)
        raw_puts(ring[(unsigned)i % RING_LINES]);
    raw_puts("===== end of crash report (no memory dump is written) =====\n");
#if defined(MH1_WINDOWS)
    if (lfd >= 0)
        _commit(lfd);
#elif !defined(MH1_XBOX)
    if (lfd >= 0)
        fsync(lfd);
#endif
}

#if defined(MH1_WINDOWS)
static LONG WINAPI win_filter(EXCEPTION_POINTERS *ep)
{
    void *fr[64];
    int n = 0;
    char what[200];
    EXCEPTION_RECORD *er = ep->ExceptionRecord;
    const char *nm = "exception";
    switch (er->ExceptionCode) {
    case EXCEPTION_ACCESS_VIOLATION: nm = "access violation"; break;
    case EXCEPTION_STACK_OVERFLOW: nm = "stack overflow"; break;
    case EXCEPTION_ILLEGAL_INSTRUCTION: nm = "illegal instruction"; break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO: nm = "integer divide by zero"; break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO: nm = "float divide by zero"; break;
    case EXCEPTION_PRIV_INSTRUCTION: nm = "privileged instruction"; break;
    case EXCEPTION_IN_PAGE_ERROR: nm = "page error"; break;
    }
    if (in_crash)
        return EXCEPTION_EXECUTE_HANDLER;
    in_crash = 1;
    if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2)
        snprintf(what, sizeof what, "%s (code 0x%08lX) at %p: %s address %p", nm, er->ExceptionCode, er->ExceptionAddress,
                 er->ExceptionInformation[0] == 1 ? "writing" : er->ExceptionInformation[0] == 8 ? "executing" : "reading",
                 (void *)er->ExceptionInformation[1]);
    else
        snprintf(what, sizeof what, "%s (code 0x%08lX) at %p", nm, er->ExceptionCode, er->ExceptionAddress);
    fr[n++] = er->ExceptionAddress;
#if defined(__i386__)
    {   /* the stack holds the return addresses; take the words that point just behind a call instruction in this exe */
        unsigned char *base = (unsigned char *)GetModuleHandleA(NULL);
        IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
        unsigned char *end = base + nt->OptionalHeader.SizeOfImage;
        DWORD *sp = (DWORD *)ep->ContextRecord->Esp, *top = (DWORD *)((NT_TIB *)NtCurrentTeb())->StackBase;
        int k;
        for (k = 0; sp < top && k < 65536 && n < 64; sp++, k++) {
            unsigned char *r = (unsigned char *)*sp;
            if (r < base + 8 || r >= end)
                continue;
            if (r[-5] == 0xE8 || (r[-2] == 0xFF && (r[-1] & 0x38) == 0x10) || (r[-3] == 0xFF && (r[-2] & 0x38) == 0x10)
                || (r[-6] == 0xFF && (r[-5] & 0x38) == 0x10))
                fr[n++] = r;
        }
    }
#endif
    crash_report(what, fr, n);
    return EXCEPTION_EXECUTE_HANDLER;       /* ends the process: no Windows error report, no dump */
}
static void win_abort(int sig)
{
    void *fr[1];
    (void)sig;
    if (in_crash)
        _exit(134);
    in_crash = 1;
    fr[0] = 0;
    crash_report("abort() / assertion failed (SIGABRT)", fr, 0);
    _exit(134);
}
static void install_crash(void)
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    SetUnhandledExceptionFilter(win_filter);
    signal(SIGABRT, win_abort);
}
#elif defined(MH1_XBOX)
static void install_crash(void) {}
#else
static char altstack[65536];
static void sig_handler(int sig, siginfo_t *si, void *uc)
{
    void *fr[64];
    int n;
    char what[200];
    const char *nm = sig == SIGSEGV ? "SIGSEGV (segmentation fault)" : sig == SIGBUS ? "SIGBUS (bus error)" :
                     sig == SIGFPE ? "SIGFPE (arithmetic error)" : sig == SIGILL ? "SIGILL (illegal instruction)" :
                     sig == SIGABRT ? "SIGABRT (abort / failed assertion)" : "signal";
    if (in_crash)
        _exit(128 + sig);
    in_crash = 1;
    if (sig == SIGABRT)
        snprintf(what, sizeof what, "%s", nm);
    else
        snprintf(what, sizeof what, "%s, fault address 0x%lx", nm, (unsigned long)si->si_addr);
    n = backtrace(fr, 64);
#if defined(__i386__)
    {   /* the frame that faulted first: backtrace() starts inside this handler */
        ucontext_t *u = (ucontext_t *)uc;
        memmove(fr + 1, fr, sizeof fr[0] * (size_t)(n < 63 ? n : 63));
        fr[0] = (void *)u->uc_mcontext.gregs[REG_EIP];
        n = n < 63 ? n + 1 : 64;
    }
#else
    (void)uc;
#endif
    crash_report(what, fr, n);
    _exit(128 + sig);                       /* no core dump: it would hold the game's data */
}
static void term_handler(int sig)      /* killed from outside: keep what the log buffered */
{
    if (in_crash)
        _exit(128 + sig);
    rt_log("terminated by signal %d", sig);
    if (lf)
        fflush(lf);
    _exit(128 + sig);
}
static void install_crash(void)
{
    static const int sigs[] = { SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT };
    stack_t ss;
    struct sigaction sa;
    void *warm[2];
    size_t i;
    ss.ss_sp = altstack;
    ss.ss_size = sizeof altstack;
    ss.ss_flags = 0;
    sigaltstack(&ss, NULL);
    backtrace(warm, 2);                     /* loads libgcc's unwinder now, not inside the handler */
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = sig_handler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESETHAND;
    sigemptyset(&sa.sa_mask);
    for (i = 0; i < sizeof sigs / sizeof sigs[0]; i++)
        sigaction(sigs[i], &sa, NULL);
    signal(SIGTERM, term_handler);
    signal(SIGHUP, term_handler);
}
#endif

/* ------------------------------------------------------------ start / end */
static void logs_dir(char *out, size_t n)
{
    const char *e = getenv("MH1_LOG_DIR");
    char *s;
    if (e && *e) {
        snprintf(out, n, "%s", e);
        return;
    }
#ifdef MH1_XBOX
    snprintf(out, n, "E:\\Games\\MH1\\logs");
#else
    snprintf(out, n, "%s", rt_mc_root());       /* .../mh1pc/memcard0 -> .../mh1pc/logs */
    s = out + strlen(out);
    while (s > out + 1 && (s[-1] == '/' || s[-1] == '\\'))
        *--s = 0;
    while (s > out && s[-1] != '/' && s[-1] != '\\')
        s--;
    if (s == out)
        snprintf(out, n, "logs");
    else
        snprintf(s, n - (size_t)(s - out), "logs");
#endif
}

static void header(const char *disc, int argc, char **argv)
{
    char b[400];
    int i;
    rt_log("MH1 PC debug log (local file only; nothing is sent anywhere)");
    rt_log("build: %s, compiled %s %s", MH1_VERSION, __DATE__, __TIME__);
#if defined(MH1_WINDOWS)
    {
        typedef LONG (WINAPI *rtlgv)(OSVERSIONINFOW *);
        typedef const char *(__cdecl *wgv)(void);
        HMODULE nt = GetModuleHandleA("ntdll.dll");
        OSVERSIONINFOW v;
        rtlgv f = nt ? (rtlgv)(void *)GetProcAddress(nt, "RtlGetVersion") : NULL;
        wgv wine = nt ? (wgv)(void *)GetProcAddress(nt, "wine_get_version") : NULL;
        SYSTEM_INFO si;
        memset(&v, 0, sizeof v);
        v.dwOSVersionInfoSize = sizeof v;
        if (f)
            f(&v);
        GetSystemInfo(&si);
        rt_log("platform: Windows, 32-bit build; OS %lu.%lu build %lu%s%s%s, %lu logical CPUs", v.dwMajorVersion, v.dwMinorVersion,
               v.dwBuildNumber, wine ? " (Wine " : "", wine ? wine() : "", wine ? ")" : "", si.dwNumberOfProcessors);
    }
#elif defined(MH1_XBOX)
    rt_log("platform: original Xbox (nxdk)");
#else
    {
        struct utsname u;
        uname(&u);         /* not the host name */
        rt_log("platform: %s, %s build; kernel %s %s, %ld logical CPUs", u.sysname,
               sizeof(void *) == 4 ? "32-bit" : "64-bit", u.release, u.machine, sysconf(_SC_NPROCESSORS_ONLN));
    }
#endif
    rt_log("disc directory: %s", rt_log_path(disc ? disc : "(none)"));
    rt_log("save directory: %s", rt_log_path(rt_mc_root()));
    rt_log("log file: %s", rt_log_path(log_path));
    b[0] = 0;
    for (i = 1; i < argc && strlen(b) < 300; i++)
        snprintf(b + strlen(b), sizeof b - strlen(b), "%s%s", i > 1 ? " " : "", argv[i]);
    rt_log("arguments: %s", rt_log_path(b));
}

void rt_log_init(const char *disc, int argc, char **argv)
{
    char dir[700], st[32];
    const char *h;
    int k;
    if (inited)
        return;
    t0 = now_ms();
#ifdef MH1_WINDOWS
    h = getenv("USERPROFILE");
#else
    h = getenv("HOME");
#endif
    if (h && strlen(h) > 2 && strlen(h) < sizeof home)
        snprintf(home, sizeof home, "%s", h);
    logs_dir(dir, sizeof dir);
    make_dirs(dir);
    rotate(dir);
    stamp(st, sizeof st);
    for (k = 0; k < 100 && !lf; k++) {
        FILE *t;
        if (k)
            snprintf(log_path, sizeof log_path, "%s/mh1_%s_%d.log", dir, st, k + 1);
        else
            snprintf(log_path, sizeof log_path, "%s/mh1_%s.log", dir, st);
        if ((t = fopen(log_path, "rb")) != NULL) {     /* two runs in one second */
            fclose(t);
            continue;
        }
        lf = fopen(log_path, "wb");
        if (!lf)
            break;
    }
    if (lf) {
        static char buf[32768];
        setvbuf(lf, buf, _IOFBF, sizeof buf);
#if defined(MH1_WINDOWS)
        lfd = _fileno(lf);
#elif !defined(MH1_XBOX)
        lfd = fileno(lf);
#endif
    } else
        fprintf(stderr, "log: cannot create %s (no log file this run)\n", rt_log_path(dir));
    inited = 1;
    last_flush = now_ms();
    install_crash();
    atexit(rt_log_shutdown);
    header(disc, argc, argv);
    if (lf) {
        fflush(lf);
        last_flush = now_ms();
    }
}

int rt_log_started(void) { return inited; }

void rt_log_shutdown(void)
{
    unsigned long ms;
    if (!inited || in_crash)
        return;
    ms = now_ms() - t0;
    rt_log("session end: clean exit after %lu.%03lu s, %u drawn frames, %d game ticks, %u warnings (%u stand-ins)", ms / 1000, ms % 1000,
           n_frames_total, cur_tick, n_warn, n_standin);
    if (lf) {
        fflush(lf);
        fclose(lf);
        lf = NULL;
    }
    inited = 0;
}
