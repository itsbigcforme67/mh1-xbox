/* lb_tu_browser - one translation unit 0x005E5F00-0x005E6E80: bs_request_queue_add, bs_request_queue_free_node, bs_request_cache_clear, bs_route_queue_add, bs_route_queue_forward, bs_route_queue_back, bs_route_queue_free_reverse, bs_route_queue_free_after, bs_route_commit, bs_page_status_flag_set, bs_page_status_flag_get, bs_cache_queue_check, bs_cache_queue_get_last, bs_cache_queue_free_node, bs_cache_queue_get, bs_source_cache_clear, bs_image_cache_clear, BsCacheInitialize, BsCacheCleanup, BsRouteForward, BsRouteForwardCheck, BsRouteBack, BsRouteBackCheck, bs_route_current_page_status, BsRouteReload, BsRouteReloadCheck, BsRouteCurrent, BsRequestHtmlGetCached, BsRequestHtmlPost. Built by tools/lbtu.py from the per-run files; functions that are
   not C yet stay original bytes (asm stubs, build/raw/*.inc). */
#include "lobby_f.h"
extern BSNODE *BcRoute_cur;
extern BSNODE BcImage_head;
void BsUrlCopy_SS();
int BsUrlCompare_SS();
BSNODE *bs_route_queue_add();
BSNODE *bs_route_queue_free_after();
static BSNODE *bs_route_queue_forward(BSNODE *head, BSNODE *p);
static BSNODE *bs_route_queue_back(BSNODE *head, BSNODE *p);
BSNODE *bs_cache_queue_get_last();
void bs_cache_queue_image_free_all();
void bs_request_queue_free_node();
void flReleaseTextureHandle_NOWAITDMA();
void flReleasePaletteHandle_NOWAITDMA();
void bs_cache_queue_free_node();
typedef struct PAIR2 { u8 a, b; } PAIR2;
extern BSNODE BcSource_head;
extern BSNODE BcRoute_head;
extern BSNODE BcRequest_head;
extern u8 BcCurrentPage[0x14];
extern u8 BsCachePost[0x1000];
BSNODE *bs_request_queue_add(BSNODE *head, char *url);
static void bs_page_status_flag_set();
static u8 *bs_route_current_page_status();
char *BsRouteCurrent();
int BsUrlFileExtensionGet();
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

void bs_request_cache_clear(BSNODE *arg0) {
    bs_request_queue_free_node(&BcRequest_head, arg0);
    F(s8, arg0, 0x10C) = 0;
    F(s8, arg0, 0x10D) = 0;
    F(s32, arg0, 0x110) = 0;
    F(s32, arg0, 0x114) = 0;
    F(s32, arg0, 0x11C) = 0;
    F(s32, arg0, 0x120) = 0;
    F(s8, arg0, 0x125) = 0;
    F(s8, arg0, 0x124) = 0;
}

BSNODE *bs_route_queue_add(BSNODE *arg0, char *arg1) {
    BSNODE *temp_v0;
    BSNODE *var_s0;
    BSNODE *var_v1;

    var_v1 = 0;
    var_s0 = arg0;
loop_1:
    temp_v0 = var_s0->next;
    if (temp_v0 != 0) {
        var_v1 = var_s0;
        var_s0 = temp_v0;
        goto loop_1;
    }
    var_s0->next = arg0->next;
    arg0->next = var_s0;
    var_v1->next = 0;
    BsUrlCopy_SS(var_s0->url, arg1);
    var_s0->used = 1;
    return var_s0;
}

static BSNODE *bs_route_queue_forward(BSNODE *head, BSNODE *p) {
    BSNODE *n;
    if (head->next == p) {
        return p;
    }
    for (;;) {
        n = head->next;
        if (n == p) {
            return head;
        }
        head = n;
    }
}

static BSNODE *bs_route_queue_back(BSNODE *head, BSNODE *p) {
    BSNODE *n;
    n = p->next;
    if (n == 0) {
        return p;
    }
    if (n->used != 0) {
        return n;
    }
    return p;
}

/* original bytes: build/raw/bs_route_queue_free_reverse.inc (config/c_rawfuncs.txt); near-match C is in lb_am.c */
asm BSNODE *bs_route_queue_free_reverse()
{
#include "bs_route_queue_free_reverse.inc"
}

BSNODE *bs_route_queue_free_after(BSNODE *arg0, BSNODE *arg1) {
    BSNODE *var_a1;

    var_a1 = arg1;
loop_1:
    if (arg0->next != var_a1) {
        var_a1 = bs_route_queue_free_reverse(arg0, var_a1);
        goto loop_1;
    }
    return var_a1;
}

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

static void bs_page_status_flag_set(u8 *p, int bit, int on) {
    u8 m;
    if (on != 0) {
        p[1] = p[1] | bit;
    } else {
        m = bit;
        p[1] &= ~m;
    }
}

s32 bs_page_status_flag_get(u8 *arg0, s32 arg1) {
    return F(u8, arg0, 1) & (arg1 & 0xFFFF) & 0xFF;
}

BSNODE *bs_cache_queue_check(BSNODE *head, char *url) {
    BSNODE *p;
    BSNODE *prev;
    BSNODE *found;
    BSNODE *fprev;
    BSNODE *n;
    p = head;
    found = 0;
    fprev = 0;
    for (;;) {
        n = p->next;
        if (n != 0) {
            prev = p;
            p = n;
            if (n->used != 0 && BsUrlCompare_SS(n->url, url) == 0) {
                found = n;
                fprev = prev;
            } else {
                continue;
            }
        }
        break;
    }
    if (found != 0) {
        if (found != head->next) {
            fprev->next = found->next;
            found->next = head->next;
            head->next = found;
        }
        return found;
    }
    return 0;
}

BSNODE *bs_cache_queue_get_last(arg0)
BSNODE *arg0;
{
    BSNODE *var_a0;
    BSNODE *var_v0;

    var_a0 = arg0;
    var_v0 = 0;
    if (var_a0->next->used == 0) {
        return 0;
    }
loop_2:
    var_a0 = var_a0->next;
    if (var_a0 != 0) {
        if (var_a0->used != 0) {
            var_v0 = var_a0;
        }
        goto loop_2;
    }
    return var_v0;
}

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

/* original bytes: build/raw/bs_cache_queue_get.inc (config/c_rawfuncs.txt); near-match C is in lb_am.c */
asm int bs_cache_queue_get()
{
#include "bs_cache_queue_get.inc"
}

void bs_source_cache_clear(BSNODE *arg0) {
    bs_cache_queue_free_node(&BcSource_head, arg0);
    memset((void *)F(s32, arg0, 0x10C), 0, 0x8000);
    F(s32, arg0, 0x110) = 0;
}

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

/* original bytes: build/raw/BsCacheInitialize.inc (config/c_rawfuncs.txt) */
asm int BsCacheInitialize()
{
#include "BsCacheInitialize.inc"
}

void BsCacheCleanup(void) {
    bs_cache_queue_image_free_all(&BcImage_head);
}

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

int BsRouteForwardCheck(void) {
    BSNODE *c = BcRoute_cur;
    return c != bs_route_queue_forward(&BcRoute_head, c);
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

int BsRouteBackCheck(void) {
    BSNODE *c = BcRoute_cur;
    return c != bs_route_queue_back(&BcRoute_head, c);
}

static u8 *bs_route_current_page_status() {
    return &BcRoute_cur->x108;
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

s32 BsRouteReloadCheck(void) {
    if (bs_page_status_flag_get(bs_route_current_page_status(), 1) != 0) {
        return 0;
    }
    bs_page_status_flag_get(bs_route_current_page_status(), 2);
    return 1;
}

char *BsRouteCurrent(void) {
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

/* original bytes: build/raw/BsRequestPostAdd.inc (config/c_rawfuncs.txt) */
asm int BsRequestPostAdd()
{
#include "BsRequestPostAdd.inc"
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

/* original bytes: build/raw/bs_cache_request_html_core.inc (config/c_rawfuncs.txt) */
asm int bs_cache_request_html_core()
{
#include "bs_cache_request_html_core.inc"
}

/* original bytes: build/raw/bs_cache_request_image_core.inc (config/c_rawfuncs.txt) */
asm int bs_cache_request_image_core()
{
#include "bs_cache_request_image_core.inc"
}

void bs_cache_queue_image_free_all(BSNODE *arg0) {
    BSNODE *var_a0;

    var_a0 = bs_cache_queue_get_last();
    if (var_a0 != 0) {
        do {
            bs_image_cache_clear(var_a0);
            var_a0 = bs_cache_queue_get_last(arg0);
        } while (var_a0 != 0);
    }
}

/* original bytes: build/raw/bs_cache_request_create_texture.inc (config/c_rawfuncs.txt) */
asm int bs_cache_request_create_texture()
{
#include "bs_cache_request_create_texture.inc"
}

/* original bytes: build/raw/bs_request_check_task_http.inc (config/c_rawfuncs.txt) */
asm int bs_request_check_task_http()
{
#include "bs_request_check_task_http.inc"
}

/* original bytes: build/raw/bs_request_set_task_core.inc (config/c_rawfuncs.txt) */
asm int bs_request_set_task_core()
{
#include "bs_request_set_task_core.inc"
}

/* original bytes: build/raw/bs_request_set_task_html.inc (config/c_rawfuncs.txt) */
asm int bs_request_set_task_html()
{
#include "bs_request_set_task_html.inc"
}

/* original bytes: build/raw/bs_request_set_task_image.inc (config/c_rawfuncs.txt) */
asm int bs_request_set_task_image()
{
#include "bs_request_set_task_image.inc"
}

/* original bytes: build/raw/BsRequestTask.inc (config/c_rawfuncs.txt) */
asm int BsRequestTask()
{
#include "BsRequestTask.inc"
}

/* original bytes: build/raw/BsRequestCheck.inc (config/c_rawfuncs.txt) */
asm int BsRequestCheck()
{
#include "BsRequestCheck.inc"
}

void BsRequestCancelHtml(void) {
    switch (BcCurrentPage[0]) {
    case 2:
    case 1:
        BcRoute_cur = bs_route_queue_back(&BcRoute_head, BcRoute_cur);
        BcRoute_cur = bs_route_queue_free_after(&BcRoute_head, BcRoute_cur);
        break;
    case 3:
        BcRoute_cur = bs_route_queue_back(&BcRoute_head, BcRoute_cur);
        break;
    case 4:
        BcRoute_cur = bs_route_queue_forward(&BcRoute_head, BcRoute_cur);
        break;
    case 0:
    case 5:
        break;
    }
}

void BsRequestCancelAll(void) {
    BSNODE *n;
    n = &BcRequest_head;
    for (;;) {
        n = n->next;
        if (n == 0) {
            return;
        }
        if (n->used == 0) {
            return;
        }
        switch (n->used) {
        case 6:
            break;
        case 5:
        case 4:
            n->used = 6;
            n->x124 = 7;
            n->x125 = 0;
            break;
        case 3:
            *(u8 *)(*(int *)((u8 *)n + 0x118) + 0x35) = 1;
            break;
        }
    }
}


extern char *bs_strtbl_html[];
extern char *bs_strtbl_toolmenu[];
extern char *bs_strtbl_titlebar[2];
extern char *bs_strtbl_mmbb_dialog[];
extern char *bs_strtbl_mmbb_ng_msg[];
extern char *bs_strtbl_cap_dialog[];
extern char *bs_strtbl_err_dialog[2];
char *BsStrtblGet(int kind, int idx) {
    switch (kind) {
    case 2:
        return bs_strtbl_html[idx];
    case 3:
        return bs_strtbl_toolmenu[idx];
    case 4:
        return bs_strtbl_titlebar[idx];
    case 5:
        return bs_strtbl_mmbb_dialog[idx];
    case 6:
        return bs_strtbl_mmbb_ng_msg[idx];
    case 7:
        return bs_strtbl_cap_dialog[idx];
    case 8:
        return bs_strtbl_err_dialog[idx];
    }
    return 0;
}

