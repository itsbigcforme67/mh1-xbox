/* lb_gan04 - near-match fixes 0x005E7080-0x005E711C: BsRequestImage. Whole file in lb_an.c. */
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

int BsRequestImage(char **a) {
    int v;
    BSNODE *r;
    u8 *st;
    r = bs_request_queue_add(&BcRequest_head, *a);
    r->used = 4;
    r->x10C = 1;
    r->x10D = 2;
    r->x110 = BsRouteCurrent();
    st = bs_route_current_page_status();
    *(PAIR2 *)&r->x108 = *(PAIR2 *)st;
    v = BsUrlFileExtensionGet(*a);
    if (!(v & 0xFF)) {
        r->used = 6;
        r->x124 = 2;
        r->x125 = 0;
        return v;
    }
    return v;
}
