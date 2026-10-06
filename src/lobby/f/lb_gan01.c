/* lb_gan01 - near-match fixes 0x005E6A60-0x005E6B2C: BsRouteForward. Whole file in lb_an.c. */
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

char *BsRouteForward(void) {
    BSNODE *n;
    BSNODE *r;
    n = bs_route_queue_forward(&BcRoute_head, BcRoute_cur);
    r = bs_request_queue_add(&BcRequest_head, n->url);
    r->used = 4;
    if (n == BcRoute_cur) {
        r->x10C = 6;
        r->x10D = 1;
        r->x110 = BcRoute_cur->url;
        *(PAIR2 *)&r->x108 = *(PAIR2 *)&n->x108;
        BcCurrentPage[0] = 5;
    } else {
        r->x10C = 4;
        r->x10D = 1;
        r->x110 = BcRoute_cur->url;
        *(PAIR2 *)&r->x108 = *(PAIR2 *)&n->x108;
        BcCurrentPage[0] = 3;
    }
    return n->url;
}
