/*
 * xbox_watch.c - the Xbox build's freeze watchdog and crash catcher (owner's request, 21 Oct 2026).
 *
 * Freeze watchdog: a second thread looks every half second whether the game made progress (a drawn frame,
 * xbox_log_frame; or a finished load step, rt_lt_end: a file read, an unpack, a texture). With none for 5 s
 * (60 s while a known long operation runs, rt_lt_span_*, and always 60 s in xemu, which can run far below real
 * time on a busy laptop; RT_WATCHDOG_MS in env.txt overrides, 0 = off) it writes a freeze report into the log
 * (where the game was: mode / step / stage / quest, the last checkpoint passed, the last log lines; the kernel has no
 * call that reads another thread's registers, so no backtrace of the frozen thread), commits the log and the save files to the disk,
 * stops the AC97 and goes back to the network loader, which sends the log (and the save) to the laptop.

 */
#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xbox_watch.h"

void rt_log_crash(const char *what, void *const *frames, int nf);  /* rt_log.c: the crash report (game state, log tail) */
void xbox_log_bail(const char *headline, int secs);                 /* xbox_log.c: flush, AC97 off, screen, loader */
int xbox_serial_present(void);                                      /* xbox_log.c: a UART (xemu) */

static volatile unsigned beat;
static volatile int long_ops;
static const char *volatile mark = "start";
static volatile int fired;

void xbox_watch_alive(void) { beat++; }
void xbox_watch_long(int on) { long_ops += on ? 1 : -1; if (long_ops < 0) long_ops = 0; }
void xbox_watch_mark(const char *where) { mark = where; }

/* ------------------------------------------------------------ the freeze watchdog */
static DWORD WINAPI watch_thread(LPVOID arg)
{
    unsigned last = beat, since = GetTickCount(), lim0 = 5000;
    const char *e = getenv("RT_WATCHDOG_MS");
    (void)arg;
    if (xbox_serial_present())
        lim0 = 60000;                       /* xemu */
    if (e)
        lim0 = (unsigned)atoi(e);
    if (lim0 == 0)
        return 0;
    for (;;) {
        unsigned now, lim;
        Sleep(500);
        now = GetTickCount();
        if (beat != last || fired) {
            last = beat;
            since = now;
            continue;
        }
        lim = long_ops ? (lim0 > 60000 ? lim0 : 60000) : lim0;
        if (now - since < lim)
            continue;
        {
            char what[200];
            void *fr[1];
            int n = 0;
            fired = 1;
            snprintf(what, sizeof what, "freeze: no frame and no load progress for %u s (last checkpoint: %s%s)",
                     (now - since) / 1000, mark, long_ops ? ", in a long operation" : "");
            fprintf(stderr, "FREEZE: %s\n", what);
            rt_log_crash(what, fr, n);
            xbox_log_bail("Monster Hunter froze - sending the report (back to the loader)", 4);
        }
    }
    return 0;
}

void xbox_watch_start(xbox_seh_reg *reg)
{
    (void)reg;                          /* (the crash catcher's SEH record: next commit) */
    CreateThread(NULL, 0, watch_thread, NULL, 0, NULL);
}
