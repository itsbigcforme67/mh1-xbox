/* lb_n01 - lobby senders 0x005D3640-0x005D3660: sound_call_005D3640. Whole file in lb_n.c. */
#include "lobby_f.h"








void sound_call_005D3640(a, b)
int a;
s8 b;
{
    lb_sys.x80 = a;
    lb_sys.x84 = b;
    lb_sys.x7C = 0x14;
}
