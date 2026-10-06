/* lb_gan03 - near-match fixes 0x005E6D70-0x005E6DE4: BsRequestHtmlGetCached. Whole file in lb_an.c. */
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
typedef struct PAIR2 { u8 a, b; } PAIR2;

void BsRequestHtmlGetCached(char **a) {
    u8 c;
    BSNODE *r;
    r = bs_request_queue_add(&BcRequest_head, *a);
    r->used = 4;
    r->x10C = 1;
    r->x10D = 1;
    r->x110 = BsRouteCurrent();
    c = BcCurrentPage[0x10] + 1;
    BcCurrentPage[0x10] = c;
    r->x108 = c;
    r->x109 = 0;
    BcCurrentPage[0] = 1;
}
