/* lb_gpc02 - item box / plaza chat / eft25 0x0060D810-0x0060D918: plaza_chat_log_disp_line, plaza_name_sprint. Whole file in lb_pc.c. */
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












int plaza_chat_log_disp_line(int a) {
    int n = a & 0xFF;
    int sum = 0;
    int head = *(u8 *)0x39DADE - 1;
    int i = head - n;
    int cnt = *(u8 *)0x39DADF - n;
    if (cnt != 0) {
        do {
            i &= 0x3F;
            cnt -= 1;
            sum = (sum + *(u8 *)(PitMenu + i * 0x5D + 0x61)) & 0xFF;
            i -= 1;
        } while (cnt != 0);
    }
    return sum;
}

void plaza_name_sprint(s8 *dst, s8 *src) {
    int n;
    s8 *d;
    s8 c;
    d = dst;
    n = 11;
    do {
        c = *src;
        if (c == 0) {
            n -= 1;
            if (n > 0) {
                do {
                    *d = 0x20;
                    n -= 1;
                    d += 1;
                } while (n > 0);
            }
            *d++ = 0x81;
            *d++ = 0x46;
            *d = 0;
            return;
        }
        *d = c;
        n -= 1;
        d += 1;
        src += 1;
    } while (n > 0);
    dst[9] = 0xA5;
    dst[10] = 0x81;
    dst[11] = 0x46;
    dst[12] = 0;
}
