/* lb_gpc03 - item box / plaza chat / eft25 0x0060DFA0-0x0060E180: Plaza_log_id_chk, Plaza_ReibunEdit_i, Plaza_ReibunEdit_mv. Whole file in lb_pc.c. */
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












/* collect pointers to the distinct chat-log speakers (other than me / the system id) into list; returns the count */
u32 Plaza_log_id_chk(u8 **list) {
    u8 *e;
    int left;
    u32 n;
    u8 **p;
    left = PZ_COUNT;
    if (left == 0) {
        *list = 0;
        return 0;
    }
    e = (u8 *)D_39DAE3;
    n = 0;
    if (left != 0) {
        p = list;
        do {
            if (strcmp(e + 0x44, my_user_id) != 0 && strcmp(e + 0x44, lit_384_006685E0) != 0 && plaza_log_id_chk_sub(e + 0x44, list, n) == 1) {
                *p = e;
                n += 1;
                p += 1;
            }
            left -= 1;
            e += 0x5D;
        } while (left != 0);
    }
    if (n < 0x40) {
        list[n] = 0;
    }
    return n;
}

void Plaza_ReibunEdit_i(void) {
    *(s16 *)&reibun_edit = 0;
    reibun_edit.b2 = 0;
    reibun_edit.sel = 0;
}

int Plaza_ReibunEdit_mv(int arg) {
    s8 r;
    int pad;
    REIBUN *e;
    pad = arg;
    e = &reibun_edit;
    switch (reibun_edit.state) {
    case 0:
        e->sel = Reibun_select_mv(arg, e->sel);
        if ((u16)pad & 0x40) {
            e->b2 = 0;
            e->sel = 0;
        } else if ((u16)pad & 0x20) {
            if (Reibun_Edit_Start(e->sel) == 1) {
                SoftKeyboard_pos_set(100.0f, 0x140);
                e->state += 1;
            }
        }
        break;
    case 1:
        pad = (u16)(pad & 0xFFBF);
        if ((r = Reibun_Edit_Core(e->sel)) != 0) {
            e->state = 0;
        }
        break;
    }
    return pad;
}
