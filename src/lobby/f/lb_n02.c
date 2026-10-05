/* lb_n02 - lobby senders 0x005D5F40-0x005D5F88: Lb_send_chair_status. Whole file in lb_n.c. */
#include "lobby_f.h"








void Lb_send_chair_status(name, a, b)
char *name;
s8 a;
s8 b;
{
    struct { s8 a; s8 b; char n[0xB]; } p;
    p.a = a;
    p.b = b;
    strcpy(p.n, name);
    lb_send_data(0xD, 5, &p);
    Lb_send_data_to_myself(0xD, 5, &p);
}
