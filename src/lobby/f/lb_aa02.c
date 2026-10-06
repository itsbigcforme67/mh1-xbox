/* lb_aa02 - lobby chat target list 0x005CB100-0x005CB220: Lb_send_chat_plus(a, b, c) builds chatIDList (8-byte names of the other members chosen
   in the 0x39DAD6 bit mask) and calls Lb_send_chat(a, b, c). The repeated `bit = 1` and the statement order of the initialisers only steer the
   register assignment (found with a statement-order search). Whole file in lb_aa.c. */
#include "lobby_f.h"
extern u8 chatIDList[];
char *strcpy();
void Lb_send_chat();
void Lb_send_chat_plus(a, b, c)
int a;
int b;
int c;
{
    int i;
    int bit;
    u8 *p;
    u8 *id;
    if (*(u8 *)0x39DAD5 != 0) {
        cw[0x32BE] = 0;
    } else {
        bit = 1;
        cw[0x32BE] = 1;
        i = 0;
        p = (u8 *)lb_player;
        id = chatIDList;
        bit = 1;
        chatIDList[0] = 0;
        chatIDList[8] = 0;
        chatIDList[0x10] = 0;
        chatIDList[0x18] = 0;
        chatIDList[0x20] = 0;
        chatIDList[0x28] = 0;
        chatIDList[0x30] = 0;
        do {
            if (i != game_w.master && (*(u8 *)0x39DAD6 & bit)) {
                strcpy((char *)id, (char *)p + 0x24);
                id += 8;
            }
            i += 1;
            p += 0x38;
            bit += bit;
        } while (i < 8);
    }
    Lb_send_chat(a, b, c);
}
