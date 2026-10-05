/* lb_am05 - browser queues (forward) 0x005E6140-0x005E6180: bs_route_queue_forward. Whole file in lb_am.c. */
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

BSNODE *bs_route_queue_forward(BSNODE *head, BSNODE *p) {
    BSNODE *n;
    if (head->next == p) {
        return p;
    }
    for (;;) {
        n = head->next;
        if (n == p) {
            return head;
        }
        head = n;
    }
}
