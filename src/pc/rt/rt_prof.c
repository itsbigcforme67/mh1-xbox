/*
 * rt_prof.c - RT_PROF=1: CPU time per subsystem (see rt_prof.h). Clock:
 * this thread's CPU time on the PC (other jobs on the machine do not count), the
 * performance counter on the Xbox.
 */
#include "rt_prof.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32     /* the Xbox build (nxdk targets win32) */
#include <windows.h>
#else
#include <time.h>
#endif

static const char *const zname[RTP_N] = {
    "logic (rest)", "set objects", "effects move", "sound tick + ADX", "mixer", "movie video",
    "movie audio", "draw (rest)", "skinning + light", "effects draw", "gfx backend"
};
static const int per_frame[RTP_N] = { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 };
static int on = -1, stack[16], depth;
static double last, cur[RTP_N], sum[RTP_N], mx[RTP_N], fcur[RTP_N];
static int nticks, nframes;
static long cnt[RTPC_N], cnt_max[RTPC_N], cnt_cur[RTPC_N];
static const char *const cname[RTPC_N] = { "vertices skinned+lit", "vertices drawn", "triangles drawn", "draw calls" };

void rt_prof_count(int c, long n)
{
    if (rt_prof_on())
        cnt_cur[c] += n;
}

static double now_ms(void)
{
#ifdef _WIN32     /* the Xbox build (nxdk targets win32) */
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
        on = getenv("RT_PROF") != NULL;
    return on;
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
    fprintf(stderr, "prof: --- %d ticks, %d frames (ms: mean per tick or frame / worst) ---\n", nticks, nframes);
    for (z = 0; z < RTP_N; z++) {
        double m = sum[z] / (per_frame[z] ? (nframes ? nframes : 1) : nticks);
        fprintf(stderr, "prof: %-18s %-5s %7.3f %7.2f\n", zname[z], per_frame[z] ? "frame" : "tick", m, mx[z]);
        if (per_frame[z])
            tf += m;
        else
            tl += m;
        sum[z] = mx[z] = 0;
    }
    fprintf(stderr, "prof: total logic %.3f ms/tick, draw %.3f ms/frame\n", tl, tf);
    for (z = 0; z < RTPC_N; z++) {
        fprintf(stderr, "prof: %-22s %8ld per frame, max %ld\n", cname[z], cnt[z] / (nframes ? nframes : 1), cnt_max[z]);
        cnt[z] = cnt_max[z] = 0;
    }
    nticks = nframes = 0;
}
