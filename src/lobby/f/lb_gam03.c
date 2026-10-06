/* lb_gam03 - near-match fixes 0x005E63E0-0x005E64B0: bs_cache_queue_check. Whole file in lb_am.c. */
#include "lobby_f.h"
extern BSNODE *BcRoute_cur;
extern BSNODE BcImage_head;
void BsUrlCopy_SS();
int BsUrlCompare_SS();
BSNODE *bs_route_queue_add();
BSNODE *bs_route_queue_free_after();
BSNODE *bs_route_queue_forward(BSNODE *head, BSNODE *p);
BSNODE *bs_route_queue_back(BSNODE *head, BSNODE *p);
void flReleaseTextureHandle_NOWAITDMA();
void flReleasePaletteHandle_NOWAITDMA();
void bs_cache_queue_free_node();
typedef struct PAIR2 { u8 a, b; } PAIR2;

BSNODE *bs_cache_queue_check(BSNODE *head, char *url) {
    BSNODE *p;
    BSNODE *prev;
    BSNODE *found;
    BSNODE *fprev;
    BSNODE *n;
    p = head;
    found = 0;
    fprev = 0;
    for (;;) {
        n = p->next;
        if (n != 0) {
            prev = p;
            p = n;
            if (n->used != 0 && BsUrlCompare_SS(n->url, url) == 0) {
                found = n;
                fprev = prev;
            } else {
                continue;
            }
        }
        break;
    }
    if (found != 0) {
        if (found != head->next) {
            fprev->next = found->next;
            found->next = head->next;
            head->next = found;
        }
        return found;
    }
    return 0;
}
