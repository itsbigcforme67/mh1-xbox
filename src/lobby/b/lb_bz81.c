/* lb_bz81 - lobby UI/client 0x005B53F0-0x005B5420: connect_ps2 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void connect_ps2(a, b, c)
int a;
s16 b;
s16 c;
{
    struct { int a; s16 b; s16 c; } t;
    t.a = a;
    t.b = b;
    t.c = c;
    CpInetTcpOpen(&t);
    init_select_flags();
}
