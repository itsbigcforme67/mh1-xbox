/* dcon01 - f_connect (0x00271050-0x00271094): dcon_task_init, reset the dial-connect task work. */
#include "types.h"

extern u8 D_6DD7E0[];
extern void *cw;
extern u8 dcon_tk_w[];
extern u8 memory_card_port;
typedef struct { u8 pad[0xD]; u8 x0D; } NETCW0;
extern NETCW0 net_common_w;
void *memset(void *, int, int);
int func_5B25B0();

void dcon_task_init(void)
{
    cw = D_6DD7E0;
    memset(dcon_tk_w, 0, 0x1A);
    memory_card_port = net_common_w.x0D;
    func_5B25B0();
}
