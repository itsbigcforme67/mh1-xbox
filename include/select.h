#ifndef SELECT_H
#define SELECT_H
/* Declarations for the select.bin overlay (title / demo / character select).
 * Functions without a prototype: argument types not known yet. */
#include "types.h"
#include "game.h"
#include "pl.h"

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
/* edit_w (main 0x3F32E0, 0x4C bytes): character edit work (layout partly guessed) */
typedef struct EDIT_W {
    u8 x0[8];           /* 0x00 cleared by char_make_init */
    u32 col;            /* 0x08 current colour (from sample_col) */
    f32 eye[3];         /* 0x0C camera position for the preview (ed_view_set) */
    f32 at[3];          /* 0x18 camera target */
    s8 name[0x12];      /* 0x24 character name (SJIS bytes, 0 = end) */
    u16 x36;            /* 0x36 */
    s16 x38;            /* 0x38 */
    u8 x3A;             /* 0x3A colour index */
    u8 x3B;             /* 0x3B */
    s8 x3C;             /* 0x3C */
    s8 x3D;             /* 0x3D */
    u16 x3E;            /* 0x3E button state used by ed_color_sel (guess) */
    u8 _pad40[0x4C - 0x40];
} EDIT_W;
extern EDIT_W edit_w;
extern s32 sample_col[];
extern s16 voice_idx[];
extern s16 decide_chr_tbl[];
extern u16 Psw[];       /* pad state, Psw[4] = pressed buttons */
void se_req();
int ran_suu();
void pl_chr_set2();
void flSetRenderState();
void Sel_back_disp();
void Sel_menu_disp();
int cmn_mongon_check_sub();
void Set_equip_data();
void DispFrameMessageA();
int strlen();
int strncmp();
extern u8 help_mess_005387B0[];

/* Raw access by byte offset into player_work-style blocks (identical code to a struct field). */
#define B8(p, o)   (*(u8 *)((u8 *)(p) + (o)))
#define BS8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define B16(p, o)  (*(u16 *)((u8 *)(p) + (o)))
#define BS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B32(p, o)  (*(s32 *)((u8 *)(p) + (o)))
#define BF(p, o)   (*(f32 *)((u8 *)(p) + (o)))
#define BP(p, o)   (*(void **)((u8 *)(p) + (o)))
s8 Get_hunter_rank();
void Set_equip_idx();
void Warehouse_equip();
int Warehouse_equip_stack();
void flMemset();
extern u8 User_data[];
extern u8 option_w[];
extern f32 stage_start_pos[][3];
extern s32 edit_top[];
void Ed_trans_pl();
int get_mdlw_ptr();
s16 get_prim();
void *get_prim_ptr();
void parts_init();
void pl_create_model();
void weapon_create_model();
void armor_create_model();
void yure_init();
void trans_pl_sub();
void Put_sprite_rotate();
f32 flSin(f32);
extern s16 arr_id_tbl[];
extern s8 _ctype_[];
extern s8 check_mongon[];
int cmn_mongon_set(s8 *, s8 *, int);
void cmn_mongon_check_filter(s8 *, s8 *, int);
void font_print_ex();
extern char lit_501_0053B820[], lit_502_0053B840[], lit_503_0053B870[], lit_504_0053B8A0[];
void flps0004();
void flps0005();
void Disp_button(f32, int, int, int, int);
extern u16 System_timer;
extern char *edit_menu_msg[];
extern char *sex_char_tbl[];
extern char lit_319_0053B628[], lit_320_0053B630[], lit_321_0053B640[], lit_322_0053B658[];
void Sel_csr_disp();
extern char lit_463_0053B6A0[], lit_464_0053B6C0[], lit_465_0053B6D8[], lit_466_0053B6F0[], lit_467_0053B720[], lit_468_0053B740[], lit_469_0053B770[], lit_470_0053B7A0[], lit_471_0053B7A8[];
typedef struct LPVIEW { f32 at[3]; f32 eye[3]; } LPVIEW;
extern LPVIEW *lpView;
void flvecCopy(f32 *, f32 *);
void get_joint_pos();
extern char lit_485_0053B7B0[], lit_486_0053B7D0[], lit_487_0053B7E0[], lit_488_0053B800[];
void SoftKeyboard_pos_set(int, f32);
void DispSoftkeyboard();
void font_set_palette();
void font_print();
void flfntLocate();
void waku_disp(f32, f32, f32, f32, f32);
extern u8 color_mess[];
extern char lit_656_0053B8C8[], lit_657_0053B8D0[], lit_658_0053B8D8[];
#endif
