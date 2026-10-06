/* imecc: kh_merge_getone (SLPM_654.95 0x002466C0-0x00246784): picks the list with the highest priority from a list of candidate lists, pops its first
 * candidate and recomputes the list's priority. IME engine (name entry), see imead.c for the context. */
#include "types.h"

typedef struct PW PW;
typedef struct BS BS;
typedef struct KH KH;
typedef struct KL KL;

struct KH {
    u8 flag;        /* 0x00 bit0 = continued, 0x80 = none */
    u8 str[5];      /* 0x01 */
    u8 x06;
    u8 x07;
    PW *pw;         /* 0x08 */
    u16 x0C;
    u16 x0E;
    KH *next;       /* 0x10 */
};

struct KL {
    BS *bs;         /* 0x00 */
    u16 pri;        /* 0x04 */
    KH *kh;         /* 0x08 */
    KL *next;       /* 0x0C */
};

KH *kh_skip();
u16 kh_priority();

int kh_merge_getone(KL *list)
{
    int best;
    KH *r;
    KL *sel;
    KL *l;

    best = 0;
    sel = 0;
    for (l = list; l != 0; l = l->next) {
        if (l->kh != 0 && (sel == 0 || (u16)l->pri > (u16)best)) {
            best = l->pri;
            sel = l;
        }
    }
    if (sel == 0) {
        return 0;
    }
    r = sel->kh;
    sel->kh = kh_skip(r, best);
    sel->pri = (sel->kh == 0) ? 0 : (kh_priority(sel->bs, sel->kh->x0E) & 0xFFFF);
    return (int)r;
}
