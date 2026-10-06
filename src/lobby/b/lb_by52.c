/* lb_by52 - agent B promoted near-match 0x005383D0-0x0053845C: Lb_make_price_str (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char shopStr[];

char *Lb_make_price_str(char *arg0, int arg1) {
    char *temp_v0;

    strcpy(shopStr, arg0);
    temp_v0 = (char *)strrchr(shopStr, 0x24);
    if (temp_v0 == 0) {
        return arg0;
    }
    Lb_num_to_str(arg1, temp_v0);
    strcat(shopStr, (char *)strrchr(arg0, 0x24) + 1);
    return shopStr;
}
