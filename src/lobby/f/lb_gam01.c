/* lb_gam01 - near-match fixes 0x005E6380-0x005E63B8: bs_page_status_flag_set. Whole file in lb_am.c. */
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

void bs_page_status_flag_set(u8 *p, int bit, int on) {
    u8 m;
    if (on != 0) {
        p[1] = p[1] | bit;
    } else {
        m = bit;
        p[1] &= ~m;
    }
}
