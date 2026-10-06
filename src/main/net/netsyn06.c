/* Network play sync, chat packets (SLPM_654.95 0x001BC690-0x001BCA20): net_send_chat, net_receive_chat. */
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

void net_send_chat(u8 who, u8 kind, char *str, s8 flag) {
    NPKC pk;
    int len;
    s8 i;
    char *d;

    if (Online_ck() != 0 && who == game_w.master) {
        len = strlen(str);
        if (len >= 0x40) {
            len = 0x3F;
        }
        pk.x3 = 0;
        pk.x2 = 0;
        switch (kind) {
        case 0:
            break;
        case 1:
            pk.cmd = kind;
            pk.who = who;
            pk.len = 0x48;
            d = pk.text;
            i = 0;
            pk.flag = flag;
            pk.tlen = len;
            for (; i < len; i++) {
                *d++ = *str++;
            }
            *d = 0;
            break;
        }
        send_flag = AQ_data_put(6, (u8 *)&pk, 0);
    }
}
