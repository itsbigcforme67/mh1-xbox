/* pick.h - the in-game bug reporter (pick.c). PC front-end only. */
#ifndef PICK_H
#define PICK_H
#ifdef XBOX
/* the Xbox build has no bug reporter (it needs the GL backend and a mouse): compiled out */
static inline void pick_arm(void) {}
static inline int pick_busy(void) { return 0; }
static inline int pick_frozen(void) { return 0; }
static inline int pick_hold_ticks(int t) { (void)t; return 0; }
static inline int pick_frame_hook(void) { return 0; }
static inline void pick_frozen_frame(void) {}
static inline int pick_event(const void *e) { (void)e; return 0; }
static inline void pick_poll_pad(void) {}
static inline void pick_ring_frame(void) {}
#else
void pick_arm(void);                    /* freeze (F8) */
int  pick_busy(void);                   /* a report is being made (a headless run waits for it) */
int  pick_frozen(void);                 /* the frozen frame is shown: the viewer skips the scene */
int  pick_hold_ticks(int ticks);        /* no game ticks while armed / frozen (and the RT_PICK_AT trigger) */
int  pick_frame_hook(void);             /* at the end of a drawn frame; 1 = draw the frame once more (id pass) */
void pick_frozen_frame(void);           /* draw the frozen frame and the reporter's panel */
int  pick_event(const void *sdl_event); /* an SDL_Event; 1 = consumed */
void pick_poll_pad(void);               /* the controller combo */
void pick_ring_frame(void);             /* the clip buffer: call before presenting a normal frame */
#endif
#endif
