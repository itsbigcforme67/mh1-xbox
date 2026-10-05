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
void bs_route_commit(BSNODE *head, BSNODE *req, void *c) {
    s8 k;
    u8 a;
    u8 b;
    k = req->x10C;
    switch (k) {
    case 4:
        BcRoute_cur = bs_route_queue_forward(head, BcRoute_cur);
        return;
    case 5:
        BcRoute_cur = bs_route_queue_back(head, BcRoute_cur);
        return;
    case 6:
        a = req->x108;
        b = req->x109;
        BcRoute_cur->x108 = a;
        BcRoute_cur->x109 = b;
        return;
    default:
        BcRoute_cur = bs_route_queue_free_after(head);
        BcRoute_cur = bs_route_queue_add(head, c);
        a = req->x108;
        b = req->x109;
        BcRoute_cur->x108 = a;
        BcRoute_cur->x109 = b;
        return;
    }
}
void bs_page_status_flag_set(u8 *p, int bit, int on) {
    if (on != 0) {
        p[1] = p[1] | bit;
    } else {
        p[1] = p[1] & (u8)(~(bit & 0xFF));
    }
}
BSNODE *bs_cache_queue_check(BSNODE *head, char *url) {
    BSNODE *s0;
    BSNODE *s3;
    BSNODE *s1;
    BSNODE *s2;
    BSNODE *s4;
    s4 = head;
    s2 = 0;
    s1 = 0;
    for (;;) {
        s0 = s4->next;
        if (s0 != 0) {
            s3 = s4;
            s4 = s0;
            if (s0->used != 0 && BsUrlCompare_SS(s0->url, url) == 0) {
                s2 = s0;
                s1 = s3;
            } else {
                continue;
            }
        }
        break;
    }
    if (s2 != 0) {
        if (s2 != head->next) {
            s1->next = s2->next;
            s2->next = head->next;
            head->next = s2;
        }
        return s2;
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
    BSNODE *prev;
    BSNODE *p;
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
            BsUrlCopy_SS(s0->url, p);
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
