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
 *
 * Crash catcher: an SEH frame at the bottom of the main thread's stack (fs:[0], the kernel's exception dispatcher
 * walks that chain before it would stop the console). An exception there (access violation, divide by zero,
 * illegal instruction, ...) logs its code, address, registers, 16 stack words and a backtrace (the build's own
 * symbol table, rt_host_symname), commits the log, shows "crashed - sending report" for a few seconds and
 * relaunches the loader. Other threads (audio, this one) are not covered.
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
const char *rt_host_symname(const void *addr, unsigned *off);       /* rt_symtab.c */
void xbox_log_bail(const char *headline, int secs);                 /* xbox_log.c: flush, AC97 off, screen, loader */
int xbox_serial_present(void);                                      /* xbox_log.c: a UART (xemu) */

static volatile unsigned beat;
static volatile int long_ops;
static const char *volatile mark = "start";
static volatile int fired;

void xbox_watch_alive(void) { beat++; }
void xbox_watch_long(int on) { long_ops += on ? 1 : -1; if (long_ops < 0) long_ops = 0; }
void xbox_watch_mark(const char *where) { mark = where; }

static int valid(const void *p, unsigned n)
{
    const char *c = p;
    return MmIsAddressValid((PVOID)c) && MmIsAddressValid((PVOID)(c + n - 1));
}

/* return addresses on the stack: words that point just behind a call into code with a symbol */
static int backtrace_from(void **fr, int max, unsigned eip, const unsigned *sp)
{
    int n = 0, k;
    fr[n++] = (void *)eip;
    for (k = 0; k < 4096 && n < max; k++, sp++) {
        const unsigned char *r;
        unsigned off;
        if (((unsigned)sp & 0xFFF) == 0 && !valid(sp, 4))
            break;
        if (k == 0 && !valid(sp, 4))
            break;
        r = (const unsigned char *)*sp;
        if ((unsigned)r < 0x10005 || !valid(r - 6, 6) || !rt_host_symname(r, &off) || off > 0x20000)
            continue;
        if (r[-5] == 0xE8 || (r[-2] == 0xFF && (r[-1] & 0x38) == 0x10) || (r[-3] == 0xFF && (r[-2] & 0x38) == 0x10)
            || (r[-6] == 0xFF && (r[-5] & 0x38) == 0x10))
            fr[n++] = (void *)r;
    }
    return n;
}

static void log_regs(const CONTEXT *c)
{
    const unsigned *sp = (const unsigned *)c->Esp;
    int i;
    fprintf(stderr, "registers: eip %08lX esp %08lX ebp %08lX eax %08lX ebx %08lX ecx %08lX edx %08lX esi %08lX edi %08lX "
            "eflags %08lX\n", (unsigned long)c->Eip, (unsigned long)c->Esp, (unsigned long)c->Ebp, (unsigned long)c->Eax,
            (unsigned long)c->Ebx, (unsigned long)c->Ecx, (unsigned long)c->Edx, (unsigned long)c->Esi, (unsigned long)c->Edi,
            (unsigned long)c->EFlags);
    fprintf(stderr, "stack:");
    for (i = 0; i < 16 && valid(sp + i, 4); i++)
        fprintf(stderr, "%s%08X", i % 8 ? " " : "\n  ", sp[i]);
    fprintf(stderr, "\n");
}

/* ------------------------------------------------------------ the crash catcher */
static EXCEPTION_DISPOSITION __cdecl seh_handler(EXCEPTION_RECORD *er, void *frame, CONTEXT *cx, void *dc)
{
    static int in;
    void *fr[48];
    char what[200];
    int n;
    unsigned off = 0;
    const char *nm;
    (void)frame;
    (void)dc;
    if (in || (er->ExceptionFlags & 6))     /* a second one while reporting, or unwinding: let it go */
        return ExceptionContinueSearch;
    in = 1;
    fired = 1;
    nm = rt_host_symname(er->ExceptionAddress, &off);
    if (er->ExceptionCode == 0xC0000005 && er->NumberParameters >= 2)
        snprintf(what, sizeof what, "exception 0x%08lX (access violation, %s address %08lX) at %p (%s+0x%x)",
                 (unsigned long)er->ExceptionCode, er->ExceptionInformation[0] ? "writing" : "reading",
                 (unsigned long)er->ExceptionInformation[1], er->ExceptionAddress, nm ? nm : "?", off);
    else
        snprintf(what, sizeof what, "exception 0x%08lX at %p (%s+0x%x)", (unsigned long)er->ExceptionCode,
                 er->ExceptionAddress, nm ? nm : "?", off);
    fprintf(stderr, "CRASH: %s; last checkpoint: %s\n", what, mark);
    log_regs(cx);
    n = backtrace_from(fr, 48, cx->Eip, (const unsigned *)cx->Esp);
    rt_log_crash(what, fr, n);
    xbox_log_bail("Monster Hunter crashed - sending the report (back to the loader)", 4);
    return ExceptionContinueSearch;         /* (xbox_log_bail does not return) */
}

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
    /* the crash catcher: this thread's SEH chain gets our record first */
    __asm__ volatile("movl %%fs:0, %0" : "=r"(reg->prev));
    reg->handler = (void *)seh_handler;
    __asm__ volatile("movl %0, %%fs:0" : : "r"(reg) : "memory");
    CreateThread(NULL, 0, watch_thread, NULL, 0, NULL);
}
