/* menu.h - the PC settings menu (menu.c): F10, or Back + L3 on the controller. PC front-end only; the Xbox build has none. */
#ifndef PC_MENU_H
#define PC_MENU_H
#ifdef XBOX
static inline int menu_event(const void *e) { (void)e; return 0; }
static inline void menu_poll_pad(void) {}
static inline int menu_hold_ticks(int t) { (void)t; return 0; }
static inline int menu_open(void) { return 0; }
static inline int menu_busy(void) { return 0; }
static inline void menu_draw(void) {}
static inline void menu_set_shot(int s) { (void)s; }
#else
int  menu_event(const void *sdl_event);  /* an SDL_Event; 1 = consumed (keys and mouse while open, F10) */
void menu_poll_pad(void);                /* the controller: the open combo, and the navigation while open; once per drawn frame */
int  menu_hold_ticks(int ticks);         /* 1 while the menu is open: no game ticks (also the RT_MENU_AT test trigger) */
int  menu_open(void);
int  menu_busy(void);                    /* a scripted run (RT_MENU_KEYS) has keys left: a --shot waits for them */
void menu_draw(void);                    /* the overlay (when open) and the title-screen hint; after the frame, before it is presented */
void menu_set_shot(int is_shot);         /* --shot runs do not write the settings file */
#endif
#endif
