/* G online (PC): hand-written glue for the generated G lobby C (src/lobby/gonl/gen, tools/mhg_gonl.py). */
#include "gonl.h"

void CallBack_Result_SearchUserPlace(s64 arg0);

/* MH1's lobby client (lb_cli.c, Lbs_SeekId) passes this lbtu3 alias name as the user search's callback
 * (config/lobby_aliases.txt). In G the callback at that place is G's own CallBack_Result_SearchUserPlace (lobby
 * 0x5806F0, gen); the generated aliases (gen_rt_auto.py) could not name it reliably across plain / online builds. */
void CallBack_Result_SearchUserPlace_o(s64 arg0)
{
    CallBack_Result_SearchUserPlace(arg0);
}
