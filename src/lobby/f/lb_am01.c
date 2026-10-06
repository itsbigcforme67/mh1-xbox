/* lb_am01 - browser request/route/cache queues 0x005E5F90-0x005E6080: bs_request_queue_add, bs_request_queue_free_node. Whole file in lb_am.c. */
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

BSNODE *bs_request_queue_add(BSNODE *head, char *url) {
    BSNODE *p;
    BSNODE *n;
    p = head;
    for (;;) {
        n = p->next;
        if (n == 0) {
            return 0;
        }
        p = n;
        if (n->used == 0) {
            BsUrlCopy_SS(n->url, url);
            n->used = 1;
            return n;
        }
    }
}

void bs_request_queue_free_node(BSNODE *head, BSNODE *node) {
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
