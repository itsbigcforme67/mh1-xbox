/* lb_gpc01 - item box / plaza chat / eft25 0x0060D710-0x0060D748: Plaza_chat_init. Whole file in lb_pc.c. */
#include "lobby_f.h"
extern char PitMenu[];
/* plaza chat log state bytes (bss 0x39DADE..) */
#define PZ_HEAD (*(u8 *)0x39DADE)   /* newest entry index */
#define PZ_COUNT (*(u8 *)0x39DADF)  /* number of entries */
#define PZ_TOP (*(u8 *)0x39DAE0)    /* first visible entry */
#define PZ_ATEND (*(u8 *)0x39DAE1)  /* window shows the last entry */
#define PZ_ARROWS (*(u8 *)0x39DAE2) /* bit0 scroll down arrow, bit1 scroll up arrow, bit2/3 scrolled this frame */
void SoftKeyboard_pos_set(f32, int);
int SoftKeyboard_set();
int SoftKeyboard_move();
void SoftKeyboard_exit();
void Plaza_chatlog_i();
int ChatKinsoku_chk();
typedef struct REIBUN { u8 state; u8 b1; s8 b2; u8 sel; } REIBUN;   /* stock phrase edit state (gp) */
extern REIBUN reibun_edit;
u8 Reibun_select_mv();
int Reibun_Edit_Start();
int Reibun_Edit_Core();
extern char str_tbl_reibun0[];
void Put_page_num();
void flfntSetSize();
void font_set_palette();
void flfntLocate();
void Reibun_print();
int put_main_cursor();
f32 flSin(f32);
void PutArrow();
extern u16 System_timer;
void SetFilterMode();
void font_print_uf();
void font_print_double2();
void Name_ID_change();
int plaza_log_id_chk_sub();
extern char D_39DAE3[];
extern char my_user_id[];
extern char lit_384_006685E0[];
int Plaza_get_chat_line_num();
int se_req();
void Lb_send_chat();












int Plaza_chat_init(void) {
    SoftKeyboard_pos_set(100.0f, 0x140);
    return SoftKeyboard_set(1, 0xE, 0x3C, 0);
}
