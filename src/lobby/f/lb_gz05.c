/* lb_gz05 - browser table/tag handlers 0x00600020-0x0060009C: tagAct_318 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 tagAct_318(s32 arg0, int arg1) {
    if (chack_TableTagClose(arg0, arg1, 1) < 0) {
        return -1;
    }
    tagoutprintf6(arg1);
    init_table_data();
    if (SetTableData(1) < 0) {
        return -1;
    }
    tagAct_310(arg0, arg1);
    return 0;
}
