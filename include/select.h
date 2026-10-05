#ifndef SELECT_H
#define SELECT_H
/* Declarations for the select.bin overlay (title / demo / character select).
 * Functions without a prototype: argument types not known yet. */
#include "types.h"

/* select.bin sees these main-module globals with its own (partial) layouts; do not
 * include flow.h / f_game.h in the same file. */
typedef struct SYS_W {
    u8 _pad00[0xB];
    u8 x0B;             /* 0x0B set once the card init ran (guess) */
    u8 _pad0C[0x1A - 0xC];
    u8 x1A;             /* 0x1A (title: value 0, 2, 3 vs others picks the text) */
    u8 _pad1B[0x31 - 0x1B];
    u8 x31;             /* 0x31 0 = 640 wide title picture */
    u8 _pad32[0x80 - 0x32];
} SYS_W;
extern SYS_W system_w;
typedef struct SEL_W { u8 _pad00[0xAC]; u16 xAC; /* 0xAC selected quest number */ u8 _padAE[0xC0 - 0xAE]; } SEL_W;
extern SEL_W select_w;
void FlushCache();
void Quest_init();
void Tsk_Execute();
void Tsk_Exit();
void trans();


typedef struct STASK { u8 _pad00[8]; u8 step; /* 0x8 */ } STASK;

void load_texlist();
void load_bin();
void flSndPackLoad();
void SoftKeyboard_init();
void init_std_rate();
void sprite_work_init();
void model_work_init();
void init_card_w();
void MemcardInit();
void PitWork_init();
void McOperationSet();
int McCardOperation();
void system_w_set();

extern s32 PIT_TEX[];
extern void *data_load_ptr;
extern u8 Card_task[], Fade_task[], Demo_task_obj[];

/* Sprite for flps0008 (as in src/game/tuto/tutob.c). */
typedef struct SPR {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    s16 u, v, u2, v2;   /* 0x0C */
} SPR;
void flps0008();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void Put_2TF();
void flfntSetSize();
void flfntSetHalftype();
void font_print_ex();
void font_print_double2();
void font_draw();
void fade_set();
u8 Fade_busy_ck();
void all_reset();
void init_demo_work();
void movie_reset();
void movie_start();
void movie_request();
int movie_server();
void movie_draw();
void movie_exit();

/* demo_w (main 0x3F3620, 0x44 bytes): title / opening demo state */
typedef struct DEMO_W {
    u8 mode;            /* 0x00 0 start, 1 logo, 2 logo2, 3 capcom, 4 title, 5 opening movie, 6 title (fast) */
    u8 step;            /* 0x01 */
    u8 mov;             /* 0x02 movie started flag (opening_demo) */
    u8 _pad03[7];
    s16 timer;          /* 0x0A (opening_demo: frame counter, see cnt) */
    u8 _padC[4];
    u8 x10;             /* 0x10 skip/keep flag (guess) */
    u8 _pad11[0x44 - 0x11];
} DEMO_W;
extern DEMO_W demo_w;
extern u8 Select_task[];
#endif
