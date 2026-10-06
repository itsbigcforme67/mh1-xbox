/* lb_am04 - browser request/route/cache queues 0x005E6650-0x005E66CC: bs_image_cache_clear. Whole file in lb_am.c. */
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

void bs_image_cache_clear(BSNODE *n) {
    u32 h;
    bs_cache_queue_free_node(&BcImage_head, n);
    if (n->tex != 0) {
        flReleaseTextureHandle_NOWAITDMA(n->tex & 0xFFFF);
        h = (n->tex & 0xFFFF0000) >> 0x10;
        if (h != 0) {
            flReleasePaletteHandle_NOWAITDMA(h);
        }
    }
    n->tex = 0;
    *(s32 *)&n->x10C = 0;
    n->x110 = 0;
    n->x118 = 0;
    n->x11A = 0;
}
