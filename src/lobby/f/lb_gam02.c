/* lb_gam02 - near-match fixes 0x005E62B0-0x005E6378: bs_route_commit. Whole file in lb_am.c. */
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

void bs_route_commit(BSNODE *head, BSNODE *req, void *c) {
    s8 k;
    k = req->x10C;
    switch (k) {
    case 4:
        BcRoute_cur = bs_route_queue_forward(head, BcRoute_cur);
        return;
    case 5:
        BcRoute_cur = bs_route_queue_back(head, BcRoute_cur);
        return;
    case 6:
        *(PAIR2 *)&BcRoute_cur->x108 = *(PAIR2 *)&req->x108;
        return;
    default:
        BcRoute_cur = bs_route_queue_free_after(head, BcRoute_cur);
        BcRoute_cur = bs_route_queue_add(head, c);
        *(PAIR2 *)&BcRoute_cur->x108 = *(PAIR2 *)&req->x108;
        return;
    }
}
