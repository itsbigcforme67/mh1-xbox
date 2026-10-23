/* xbox_watch.h - the Xbox build's freeze watchdog and crash catcher (xbox_watch.c). On the PC the calls are empty. */
#ifndef XBOX_WATCH_H
#define XBOX_WATCH_H
#ifdef XBOX
typedef struct xbox_seh_reg { struct xbox_seh_reg *prev; void *handler; } xbox_seh_reg;
void xbox_watch_start(xbox_seh_reg *reg);   /* main's first lines: reg is a local of main (an SEH record must be on the stack) */
void xbox_watch_alive(void);                /* progress: a drawn frame, a finished file read / unpack (rt_lt_end) */
void xbox_watch_long(int on);               /* around a known long operation (rt_lt_span_*): a longer limit */
void xbox_watch_mark(const char *where);    /* the last checkpoint passed (a string literal), shown in a freeze report */
#else
#define xbox_watch_alive() ((void)0)
#define xbox_watch_long(on) ((void)0)
#define xbox_watch_mark(where) ((void)0)
#endif
#endif
