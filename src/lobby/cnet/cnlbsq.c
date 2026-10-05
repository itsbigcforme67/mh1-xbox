/* cnlbs, run 17: Write_Socket .. Write_Socket (lobby.bin 0x005AE300-0x005AE320): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

void Write_Socket(w)
u16 *w;
{
    CpInetTcpSend(CnetSys_w.sock, (u8 *)w + 4, (*w + 0xC) << 16 >> 16);
}
