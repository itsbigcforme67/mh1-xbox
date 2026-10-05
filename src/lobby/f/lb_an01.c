/* lb_an01 - browser request post clear 0x005E7060-0x005E7074: BsRequestPostClear. Whole file in lb_an.c. */
#include "lobby_f.h"
extern BSNODE *BcRoute_cur;
extern BSNODE BcRoute_head;
extern BSNODE BcRequest_head;
extern u8 BcCurrentPage[0x14];
extern u8 BsCachePost[0x1000];
BSNODE *bs_route_queue_forward(BSNODE *head, BSNODE *p);
BSNODE *bs_route_queue_back(BSNODE *head, BSNODE *p);
BSNODE *bs_request_queue_add(BSNODE *head, char *url);
void bs_page_status_flag_set();
u8 *bs_route_current_page_status();
char *BsRouteCurrent();
int BsUrlFileExtensionGet();

void BsRequestPostClear(void) {
    memset(BsCachePost, 0, 0x1000);
}
