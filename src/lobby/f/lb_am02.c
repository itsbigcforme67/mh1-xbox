/* lb_am02 - browser request/route/cache queues 0x005E6180-0x005E61A8: bs_route_queue_back. Whole file in lb_am.c. */
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

BSNODE *bs_route_queue_back(BSNODE *head, BSNODE *p) {
    BSNODE *n;
    n = p->next;
    if (n == 0) {
        return p;
    }
    if (n->used != 0) {
        return n;
    }
    return p;
}
