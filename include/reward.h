#ifndef REWARD_H
#define REWARD_H
/* Quest reward screen (f_reward*.c): shared work struct and externs. */
#include "f_game.h"
#include "plf.h"

/* reward screen work (0x3?????): fields by offset, meanings are guesses */
typedef struct REWARD_W {
    u8 x0;              /* 0x00 mode: 0 list, 1 item pick, 2 done */
    u8 x1;              /* 0x01 frame list index */
    u8 x2;              /* 0x02 sub step */
    u8 x3;              /* 0x03 */
    u8 x4;              /* 0x04 cursor (0-15) */
    u8 x5;              /* 0x05 second cursor (0-19) */
    s16 x6;             /* 0x06 time left */
    s16 x8;             /* 0x08 repeating keys */
    s8 xA;              /* 0x0A key repeat counter */
    s8 xB;              /* 0x0B */
    u8 xC;              /* 0x0C */
    u8 _padD[3];
} REWARD_W;
extern REWARD_W reward_w;
extern struct { u8 _pad00[0x10]; u8 x10; u8 x11; s16 x12; } PitMenu;
u16 reward_key_repeat();
extern u16 Psw[];
void se_req();

/* ---- declarations shared by f_reward*.c ---- */
void trans_result_0(void);
void trans_result_1(void);
void trans_result_2(void);
void trans_result_3(void);
void end_fade_set(void);
int movie_add_check(GAME_W *, int);

typedef struct QUEST_WR {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest number */
    u8 _pad0A[0x14 - 0xA];
    s32 x14;            /* 0x14 reward money (gold_init) */
    u8 _pad18[0x34 - 0x18];
    s16 x34;            /* 0x34 */
    u8 _pad36[0x64 - 0x36];
    s32 *x64;           /* 0x64 quest data pointer (result_init) */
    u8 _pad68[0x94 - 0x68];
    s32 *x94;           /* 0x94 */
    u8 _pad98[0x14C - 0x98];
    s16 x14C;           /* 0x14C */
} QUEST_WR;
extern QUEST_WR quest_w;

typedef struct MOVIE_ADD {
    s16 quest;          /* 0x00 quest number */
    s16 flag;           /* 0x02 omake flag set when the movie is shown */
} MOVIE_ADD;

extern s16 key_quest_tbl[];
extern MOVIE_ADD movie_add_tbl[];
extern s32 card_list_frame_tbl_00357850[];
extern u8 card_prim[];

int Quest_clear_bit_ck(s16);
int movie_add_ck();
int Omake_flag_ck();
void Omake_flag_set();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void Put_2TF();
void Disp_back();
void disp_reward();
void DispFrameListA();
void Disp_button(f32, int, int, int, int);
void SetTrnslMode();
void flSetRenderState();
void str_fadein_vol();
void str_play();
void *memset(void *, int, int);

typedef struct SPR2TF {
    s16 pos[2];         /* 0x00 top-left */
    s16 size[2];        /* 0x04 */
    u32 col;            /* 0x08 */
    s16 uv[4];          /* 0x0C */
} SPR2TF;

extern s32 quest_price;
extern u16 Psw[];
extern u8 ot8[4];
int Quest_f_dra_ck();
void Quest_clear_bit_set();
void Quest_price_return();
int Quest_clear_ck();
int Online_ck();
void se_stop_all();
void se_req();
void remuneration_item_set();
void reward_init();
s16 reward_mv();
void ItemCopy_Pl2Ud();
void fade_set();
int gold_init();
int gold_main();
void gold_disp();
int result_init();
int result_main();
void result_disp();
void add_disp();
void error_disp();
void add_prim2();
#define USER_x1A (*(s16 *)((u8 *)&User_data + 0x1A))

typedef struct RESULT_W {
    s32 gold;           /* 0x00 points/money left to hand out on the result screen */
    u8 rank_new;        /* 0x04 hunter rank now */
    u8 rank_old;        /* 0x05 rank when the screen started */
    u8 _pad06[2];
} RESULT_W;
extern RESULT_W result_w;       /* small data */
int Get_hunter_rank();
int Hunter_point_add();
void Gold_add();
int sprintf(char *, const char *, ...);
int strlen();
int strlen_sp();
void font_print_sp();
char *Quest_str_get();
extern char lit_465_003865F0[];
extern char lit_466_00386610[];
extern char lit_467_00386620[];
extern char lit_468_00386630[];
extern char lit_469_00386648[];
extern char lit_470_00386658[];
extern char lit_471_00386668[];
extern char lit_472_00386678[];
extern char lit_473_00386690[];
extern char lit_474_003866B0[];
extern char lit_475_003866D0[];

extern char lit_632_00386730[];
extern char lit_633_00386750[];
extern char lit_634_00386770[];
extern char lit_635_00386790[];
extern char lit_636_003867B0[];
extern char lit_637_003867D0[];
extern char lit_638_003867F0[];
extern char lit_639_00386810[];
extern char lit_640_00386830[];
extern char lit_641_00386850[];
extern char lit_642_00386880[];
extern char lit_643_003868B0[];
extern char lit_658_00386910[];
extern char lit_659_00386920[];
extern char lit_660_00386940[];
extern char lit_661_00386970[];
extern char lit_662_00386990[];
extern char *hunter_appellation[];

#define CENTER_X(buf) ((s16)(0x140 - strlen_sp(buf) * 18 / 4))

extern char lit_674_003869B0[];
extern char lit_675_003869C0[];
extern char lit_676_003869E0[];
extern char *movie_add_str_tbl[];


#endif
