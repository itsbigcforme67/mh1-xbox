/* G online (PC): lobby_func_0057A370, the login mode's step dispatch (G lobby 0x57A370; HAND-FIXED: written by hand,
 * a jump through G's table of the login steps, lobby 0x5FBD90: lbc_login_init .. lbc_login_finish_after). */
#include "../gonl.h"

extern u8 *cw;
extern void (*g_login_steps[])(void);

void lobby_func_0057A370(void)
{
    g_login_steps[cw[0xB9B]]();
}
