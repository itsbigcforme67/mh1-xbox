/* lb_bz81 - lobby UI/client 0x005B53F0-0x005B5420: connect_ps2 (first drafted by tools/lbauto.py).
 * The original returns nothing in C but its callers (tcp_init, cmcs_01) use v0, which still holds CpInetTcpOpen's
 * handle (init_select_flags leaves v0 alone). The PC build returns the handle explicitly: without it tcp_init kept
 * whatever eax init_select_flags left (0, the first socket), so a second connection (the return to the town after an
 * online quest) polled the first, closed, socket. */
#include "lobby_a.h"

#ifdef __MWERKS__
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
#else
int connect_ps2(int a, s16 b, s16 c)
{
    struct { int a; s16 b; s16 c; } t;
    int h;
    t.a = a;
    t.b = b;
    t.c = c;
    h = CpInetTcpOpen(&t);
    init_select_flags();
    return h;
}
#endif
