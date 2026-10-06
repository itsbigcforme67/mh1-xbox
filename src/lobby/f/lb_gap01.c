/* lb_gap01 - near-match fixes 0x005E92C0-0x005E934C: bs_url_cmp_list. Whole file in lb_ap.c. */
#include "lobby_f.h"
extern u8 BsCacheCurrentBaseUrlstr[];
extern s8 lit_928_00666260[];
u32 strlen();
int strncmp();
int BsUrlSchemeGet();

int bs_url_cmp_list(char **list, char *s) {
    int i;
    char *a;
    a = *list;
    i = 0;
    if (a != 0) {
        for (;;) {
            if (strncmp(s, *list, strlen(a)) != 0) {
                list += 1;
                a = *list;
                i += 1;
                if (a != 0) {
                    continue;
                }
            }
            break;
        }
    }
    if (*list != 0) {
        return i;
    }
    i = -1;
    return i;
}
