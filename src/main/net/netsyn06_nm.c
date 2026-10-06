/* Near-match (not built into the link): Network play sync, chat packets (SLPM_654.95 0x001BC690-0x001BCA20): net_send_chat, net_receive_chat. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern s8 send_flag;
int strlen(char *);

typedef struct NPKC {
    u8 cmd, len, x2, x3;
    char text[0x40];
    u8 tlen, who, flag;
} NPKC;

void ChatLogAdd_Q(u8, u8, u8 *);

void net_receive_chat(int slot, u8 *buf) {
    u8 len;
    u8 text[0x40];
    u8 *p;
    s8 i;
    u8 *d;
    u8 who;
    u8 kind;

    if (Online_ck() != 0) {
        kind = buf[0];
        p = buf + 4;
        switch (kind) {
        case 0:
            break;
        case 1:
            who = p[0x41];
            if (who != game_w.master) {
                if (p[0x42] & (1 << game_w.master)) {
                    len = p[0x40];
                    d = text;
                    i = 0;
                    for (; i < len; i++) {
                        *d++ = p[i];
                    }
                    *d = 0;
                    ChatLogAdd_Q(who, p[0x42], text);
                }
            }
            break;
        }
    }
}
