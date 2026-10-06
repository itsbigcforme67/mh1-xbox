/* lb_gz04 - browser table/tag handlers 0x005FFFC0-0x00600018: tagAct_050 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 tagAct_050(int arg0, int arg1) {
    if (chack_TableTagClose(arg0, arg1, 1) < 0) {
        return -1;
    }
    tagoutprintf6(arg1);
    init_table_data();
    return -(SetTableData(1) < 0);
}
