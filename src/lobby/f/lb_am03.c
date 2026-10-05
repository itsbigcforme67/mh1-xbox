/* lb_am03 - browser request/route/cache queues 0x005E64F0-0x005E6580: bs_cache_queue_free_node. Whole file in lb_am.c. */
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

void bs_cache_queue_free_node(BSNODE *head, BSNODE *node) {
    BSNODE *last;
    BSNODE *p;
    BSNODE *n;
    p = head;
    last = 0;
    for (;;) {
        n = p->next;
        if (n == 0) {
            break;
        }
        if (n == node) {
            last = p;
        }
        p = n;
    }
    if (last != 0) {
        n = node->next;
        if (n != 0) {
            last->next = n;
            p->next = node;
            node->next = 0;
        }
        memset(node->url, 0, 0x100);
        node->used = 0;
        memset(&node->x108, 0, 2);
    }
}
