/* Lobby plaza chat: chat entry (soft keyboard), chat log window and reibun (stock phrase) edit
   (SLPM_654.95 lobby overlay 0x60D710-0x60E330). Whole file; runs split into lb_pcNN.c */
#include "lobby_f.h"
extern char PitMenu[];
void SoftKeyboard_pos_set(f32, int);
int SoftKeyboard_set();
int SoftKeyboard_move();
void SoftKeyboard_exit();
void Plaza_chatlog_i();
int ChatKinsoku_chk();
void Lb_send_chat();

int Plaza_chat_init(void) {
    SoftKeyboard_pos_set(100.0f, 0x140);
    return SoftKeyboard_set(1, 0xE, 0x3C, 0);
}

int Plaza_chat_move(int a) {
    char buf[0x40];
    int r;
    buf[0] = 0;
    r = (s8)SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
    if (r == -1) {
        SoftKeyboard_exit();
        Plaza_chatlog_i();
        return -1;
    }
    if (r != 0) {
        if (buf[0] != 0 && r > 0 && ChatKinsoku_chk(buf) != 0) {
            Lb_send_chat(game_w.master, buf, 0);
        }
        SoftKeyboard_exit();
        Plaza_chatlog_i();
        return -1;
    }
    return 0;
}

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
