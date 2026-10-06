/* Lobby browser: request / route / cache queues (singly linked lists of BSNODE), hand-written from m2c drafts. */
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
BSNODE *bs_route_queue_forward(BSNODE *head, BSNODE *p) {
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
BSNODE *bs_route_queue_back(BSNODE *head, BSNODE *p) {
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
BSNODE *bs_route_queue_free_reverse(BSNODE *head, BSNODE *keep) {
    BSNODE *s0;
    BSNODE *p;
    BSNODE *r;
    r = keep;
    p = head;
    for (;;) {
        BSNODE *n = p->next;
        if (n != 0) {
            p = n;
            continue;
        }
        break;
    }
    s0 = head->next;
    head->next = s0->next;
    p->next = s0;
    s0->next = 0;
    memset(s0->url, 0, 0x100);
    s0->used = 0;
    memset(&s0->x108, 0, 2);
    if (r == s0) {
        r = head->next;
    }
    return r;
}
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
void bs_page_status_flag_set(u8 *p, int bit, int on) {
    u8 m;
    if (on != 0) {
        p[1] = p[1] | bit;
    } else {
        m = bit;
        p[1] &= ~m;
    }
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
BSNODE *bs_cache_queue_get(BSNODE *head) {
    BSNODE *s0;
    BSNODE *p;
    BSNODE *prev;
    p = head;
    for (;;) {
        s0 = p->next;
        if (s0 == 0) {
            return 0;
        }
        prev = p;
        p = s0;
        if (s0->used == 0) {
            if (prev != head) {
                prev->next = s0->next;
                s0->next = head->next;
                head->next = s0;
            }
            BsUrlCopy_SS(s0->url);
            s0->used = 1;
            return s0;
        }
    }
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
