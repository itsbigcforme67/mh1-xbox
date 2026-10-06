/* Lobby browser: route navigation (forward / back / reload) and request creation, hand-written from m2c drafts. */
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
int BsRouteForwardCheck(int a, BSNODE *p) {
    return p != bs_route_queue_forward(&BcRoute_head, BcRoute_cur);
}
char *BsRouteBack(void) {
    BSNODE *n;
    BSNODE *r;
    n = bs_route_queue_back(&BcRoute_head, BcRoute_cur);
    r = bs_request_queue_add(&BcRequest_head, n->url);
    r->used = 4;
    if (n == BcRoute_cur) {
        r->x10C = 6;
        r->x10D = 1;
        r->x110 = BcRoute_cur->url;
        *(PAIR2 *)&r->x108 = *(PAIR2 *)&n->x108;
        BcCurrentPage[0] = 5;
    } else {
        r->x10C = 5;
        r->x10D = 1;
        r->x110 = n->url;
        *(PAIR2 *)&r->x108 = *(PAIR2 *)&n->x108;
        BcCurrentPage[0] = 4;
    }
    return n->url;
}
int BsRouteBackCheck(int a, BSNODE *p) {
    return p != bs_route_queue_back(&BcRoute_head, BcRoute_cur);
}
char *BsRouteReload(void) {
    BSNODE *r;
    u8 c;
    r = bs_request_queue_add(&BcRequest_head, BcRoute_cur->url);
    r->used = 4;
    r->x10C = 6;
    r->x10D = 1;
    r->x110 = BcRoute_cur->url;
    c = BcCurrentPage[0x10] + 1;
    BcCurrentPage[0x10] = c;
    r->x108 = c;
    r->x109 = bs_route_current_page_status(r)[1];
    BcCurrentPage[0] = 5;
    return BcRoute_cur->url;
}
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
void BsRequestHtmlPost(char **a) {
    u8 c;
    BSNODE *r;
    char *cur;
    r = bs_request_queue_add(&BcRequest_head, *a);
    r->used = 4;
    r->x10C = 3;
    r->x10D = 1;
    cur = BsRouteCurrent();
    r->x110 = cur;
    c = BcCurrentPage[0x10] + 1;
    BcCurrentPage[0x10] = c;
    r->x108 = c;
    r->x109 = 0;
    bs_page_status_flag_set(&r->x108, 1, 1);
    BcCurrentPage[0] = 2;
}
void BsRequestPostClear(void) {
    memset(BsCachePost, 0, 0x1000);
}
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
