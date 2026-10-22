/*
 * rt_prof.c - RT_PROF=1: CPU time per subsystem (see rt_prof.h). Clock:
 * this thread's CPU time on the PC (other jobs on the machine do not count), the
 * performance counter on the Xbox.
 */
#include "rt_prof.h"
#include "rt_plat.h"
#include "rt_log.h"
#ifdef MH1_XBOX
void xbox_watch_alive(void);            /* the freeze watchdog (src/pc/xbox/xbox_watch.c): load progress counts as alive */
void xbox_watch_long(int on);           /* and a known long operation gets the longer limit */
#else
#define xbox_watch_alive() ((void)0)
#define xbox_watch_long(on) ((void)0)
#endif
#include "../fmt/fmt.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(MH1_XBOX)     /* the Xbox build (nxdk targets win32) */
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#elif defined(MH1_WINDOWS)
#include <windows.h>
#else
#include <time.h>
#endif

static const char *const zname[RTP_N] = {
    "logic (rest)", "set objects", "effects move", "sound tick + ADX", "mixer", "movie video",
    "movie audio", "draw (rest)", "skinning + light", "effects draw", "gfx backend",
    "motion eval", "joint matrices", "stage draw code", "game prims", "2D / HUD"
};
/* per frame (draw) or per tick; motion and joints run in both and are
 * reported per tick (one frame per tick at 30 fps) */
static const int per_frame[RTP_N] = { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1 };
static int on = -1, stack[16], depth;
static double last, cur[RTP_N], sum[RTP_N], mx[RTP_N], fcur[RTP_N];
static int nticks, nframes;
static long cnt[RTPC_N], cnt_max[RTPC_N], cnt_cur[RTPC_N];
static const char *const cname[RTPC_N] = { "vertices skinned+lit", "vertices drawn", "triangles drawn", "draw calls", "skeleton evaluations" };

void rt_prof_count(int c, long n)
{
    if (rt_prof_on())
        cnt_cur[c] += n;
}

static double now_ms(void)
{
#if defined(MH1_WINDOWS)       /* this thread's CPU time (kernel + user) */
    FILETIME c, e, k, u;
    GetThreadTimes(GetCurrentThread(), &c, &e, &k, &u);
    return (((double)k.dwHighDateTime * 4294967296.0 + k.dwLowDateTime) + ((double)u.dwHighDateTime * 4294967296.0 + u.dwLowDateTime)) / 10000.0;
#elif defined(MH1_XBOX)     /* the Xbox build (nxdk targets win32) */
    LARGE_INTEGER c, f;
    QueryPerformanceCounter(&c);
    QueryPerformanceFrequency(&f);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);   /* CPU time of this thread: other jobs on the machine do not count */
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
#endif
}

int rt_prof_on(void)
{
    if (on < 0)
#ifdef MH1_XBOX
        on = 1;         /* the Xbox has no environment: always on (two clock reads per zone) */
#else
        on = getenv("RT_PROF") != NULL;
#endif
    return on;
}

/* one report line: stderr (on the Xbox that is E:\\Games\\MH1\\mh1log.txt, which the
 * network loader sends to the laptop; it used to go to E:\\mh1_prof.txt, which never
 * left the box and grew by a report every 10 s, forever) and, on the Xbox, the
 * kernel debug output (DbgPrint: xemu's / a debug BIOS's log) */
static void out(const char *fmt, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
#ifdef MH1_XBOX
    DbgPrint("%s", buf);
#endif
    fputs(buf, stderr);
}

void rt_prof_begin(int z)
{
    double t;
    if (!rt_prof_on() || depth >= 16)
        return;
    t = now_ms();
    if (depth)
        cur[stack[depth - 1]] += t - last;
    stack[depth++] = z;
    last = t;
}

void rt_prof_end(int z)
{
    double t;
    if (!rt_prof_on() || !depth)
        return;
    t = now_ms();
    cur[stack[depth - 1]] += t - last;
    last = t;
    while (depth && stack[--depth] != z)        /* unbalanced: close down to z */
        ;
}

static void fold(int frame)
{
    int z;
    for (z = 0; z < RTP_N; z++)
        if (per_frame[z] == frame) {
            sum[z] += cur[z];
            if (cur[z] > mx[z])
                mx[z] = cur[z];
            cur[z] = 0;
        }
}

void rt_prof_tick(void)
{
    if (!rt_prof_on())
        return;
    fold(0);
    nticks++;
}

void rt_prof_frame(void)
{
    int z;
    double tl = 0, tf = 0;
    if (!rt_prof_on())
        return;
    fold(1);
    for (z = 0; z < RTPC_N; z++) {
        cnt[z] += cnt_cur[z];
        if (cnt_cur[z] > cnt_max[z])
            cnt_max[z] = cnt_cur[z];
        cnt_cur[z] = 0;
    }
    nframes++;
    (void)fcur;
    if (nticks < 300)
        return;
    out("prof: --- %d ticks, %d frames (ms: mean per tick or frame / worst) ---\n", nticks, nframes);
    for (z = 0; z < RTP_N; z++) {
        double m = sum[z] / (per_frame[z] ? (nframes ? nframes : 1) : nticks);
        out("prof: %-18s %-5s %7.3f %7.2f\n", zname[z], per_frame[z] ? "frame" : "tick", m, mx[z]);
        if (per_frame[z])
            tf += m;
        else
            tl += m;
        sum[z] = mx[z] = 0;
    }
    out("prof: total logic %.3f ms/tick, draw %.3f ms/frame\n", tl, tf);
    for (z = 0; z < RTPC_N; z++) {
        out("prof: %-22s %8ld per frame, max %ld\n", cname[z], cnt[z] / (nframes ? nframes : 1), cnt_max[z]);
        cnt[z] = cnt_max[z] = 0;
    }
    nticks = nframes = 0;
}

/* ------------------------------------------------------------ load timing */
static double wall_ms(void)
{
#if defined(MH1_WINDOWS) || defined(MH1_XBOX)
    LARGE_INTEGER c, f;
    QueryPerformanceCounter(&c);
    QueryPerformanceFrequency(&f);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
#endif
}

static const char *const lname[RTL_N] = {
    "read", "melt", "texdec", "texup", "model", "motion", "hit", "gc", "snd", "flip"
};
static int lstack[16], ldepth, lt_thr = -1;
static double lt_last, lt_frame0, lcur[RTL_N], lsum[RTL_N], lev_ms;
static long lbytes[RTL_N], lev_bytes[RTL_N], lcount[RTL_N], lev_count[RTL_N];
static int lev_frames, lev_tick0, lev_stage0;

/* " name ms/calls[/KB]" for each zone with time or calls, then " other ms" (total minus the zones) */
static void zones_text(char *buf, int n, int cap, double total, const double *ms, const long *cnt, const long *bytes)
{
    int z;
    double rest = total;
    for (z = 0; z < RTL_N; z++) {
        if (ms[z] < 0.5 && !cnt[z])
            continue;
        rest -= ms[z];
        if (n < cap - 40) {
            if (bytes[z] >= 1024 && (z == RTL_READ || z == RTL_MELT))
                n += snprintf(buf + n, (size_t)(cap - n), " %s %.0f/%ld/%ldK", lname[z], ms[z], cnt[z], bytes[z] >> 10);
            else
                n += snprintf(buf + n, (size_t)(cap - n), " %s %.0f/%ld", lname[z], ms[z], cnt[z]);
        }
    }
    if (n < cap - 24)
        snprintf(buf + n, (size_t)(cap - n), " other %.0f", rest);
}

static void flush_open(double t)
{
    if (ldepth) {
        if (ldepth <= 16)
            lcur[lstack[ldepth - 1]] += t - lt_last;
        lt_last = t;
    }
}

/* A named load (an area change, the village, a quest start; spans nest): one "load: NAME" line
 * with its own time and zones, whatever the frame time around it. */
#define SPANS 4
static struct { double t0, ms[RTL_N]; long cnt[RTL_N], bytes[RTL_N]; char name[40]; } sp[SPANS];
static int nsp;
void rt_lt_span_begin(const char *name)
{
    int z;
    double t = wall_ms();
    xbox_watch_long(1);
    flush_open(t);
    if (nsp >= SPANS) {
        nsp++;
        return;
    }
    snprintf(sp[nsp].name, sizeof sp[nsp].name, "%s", name);
    sp[nsp].t0 = t;
    for (z = 0; z < RTL_N; z++) {
        sp[nsp].ms[z] = lcur[z];
        sp[nsp].cnt[z] = lcount[z];
        sp[nsp].bytes[z] = lbytes[z];
    }
    nsp++;
}

void rt_lt_span_end(void)
{
    char buf[256];
    double ms[RTL_N];
    long cnt[RTL_N], bytes[RTL_N];
    int z, n;
    double t = wall_ms();
    if (nsp)
        xbox_watch_long(0);
    if (!nsp || --nsp >= SPANS)
        return;
    flush_open(t);
    for (z = 0; z < RTL_N; z++) {      /* (a frame end inside a span would reset the counters: spans stay in one tick) */
        ms[z] = lcur[z] - sp[nsp].ms[z];
        cnt[z] = lcount[z] - sp[nsp].cnt[z];
        bytes[z] = lbytes[z] - sp[nsp].bytes[z];
    }
    n = snprintf(buf, sizeof buf, "load: %s %.0f ms:", sp[nsp].name, t - sp[nsp].t0);
    zones_text(buf, n, (int)sizeof buf, t - sp[nsp].t0, ms, cnt, bytes);
    rt_log("%s", buf);
}

void rt_lt_begin(int z)
{
    double t = wall_ms();
    if (ldepth && ldepth <= 16)
        lcur[lstack[ldepth - 1]] += t - lt_last;
    if (ldepth < 16)
        lstack[ldepth] = z;
    ldepth++;
    lt_last = t;
}

void rt_lt_end(int z)
{
    double t = wall_ms();
    if (!ldepth)
        return;
    if (ldepth <= 16)
        lcur[lstack[ldepth - 1]] += t - lt_last;
    ldepth--;
    lt_last = t;
    lcount[z]++;
    xbox_watch_alive();
}

/* called after each disc read / decompression / texture decode: the PS2's load_bin waits a vsync at a time and
 * G draws its loading screen on each (viewer.c load_wait) */
static void (*load_wait_fn)(void);
void rt_set_load_wait(void (*fn)(void)) { load_wait_fn = fn; }

static void fmt_hook(int zone, int begin, long bytes)
{
    static const int map[3] = { RTL_READ, RTL_MELT, RTL_TEXDEC };
    static int busy;
    if (begin)
        rt_lt_begin(map[zone]);
    else {
        lbytes[map[zone]] += bytes;
        rt_lt_end(map[zone]);
        if (load_wait_fn && !busy) {
            busy = 1;
            load_wait_fn();
            busy = 0;
        }
    }
}

void rt_lt_init(void)
{
    int z;
    fmt_time_hook = fmt_hook;
    lt_frame0 = wall_ms();
    for (z = 0; z < RTL_N; z++) {   /* the start-up's loads are not a pause */
        lcur[z] = 0;
        lbytes[z] = lcount[z] = 0;
    }
}

void rt_lt_frame(int tick, int stage)
{
    double t = wall_ms(), dt = t - lt_frame0;
    int z;
    if (lt_thr < 0) {
        const char *e = getenv("RT_LOAD_MS");
        lt_thr = e ? atoi(e) : 150;
    }
    lt_frame0 = t;
    flush_open(t);                  /* a zone still open (should not happen at a frame's end) */
    if (dt >= lt_thr) {
        if (!lev_frames) {
            lev_tick0 = tick;
            lev_stage0 = stage;
        }
        lev_frames++;
        lev_ms += dt;
        for (z = 0; z < RTL_N; z++) {
            lsum[z] += lcur[z];
            lev_bytes[z] += lbytes[z];
            lev_count[z] += lcount[z];
        }
    } else if (lev_frames) {        /* the pause is over: one line for it */
        char buf[256];
        int n = snprintf(buf, sizeof buf, "load: pause %.0f ms, %d frames, t%d-%d, st %d>%d:", lev_ms, lev_frames, lev_tick0, tick, lev_stage0, stage);
        zones_text(buf, n, (int)sizeof buf, lev_ms, lsum, lev_count, lev_bytes);
        rt_log("%s", buf);
        lev_frames = 0;
        lev_ms = 0;
        for (z = 0; z < RTL_N; z++) {
            lsum[z] = 0;
            lev_bytes[z] = lev_count[z] = 0;
        }
    }
    for (z = 0; z < RTL_N; z++) {
        lcur[z] = 0;
        lbytes[z] = lcount[z] = 0;
    }
}
