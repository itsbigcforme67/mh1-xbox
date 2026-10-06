/* Network play sync, enemy packets (SLPM_654.95 0x001BB000-0x001BBE58): net_receive_em, applies the four enemy packet kinds of net_send_em to
 * em_work[idx] (0xA10 bytes each): position, angles, counters, then the flag byte (status effects) and the status duration mask. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern u8 em_work[];
extern s16 D_642160[];

void func_535A10(NEMV *, int, int, int);
void func_536110(NEMV *, s16);
void func_536320(NEMV *);
void func_537620(NEMV *, void *);
void func_538580(NEMV *);
void func_53A630(NEMV *, int);
void func_53B6D0(NEMV *, u8);
void func_53B8A0(NEMV *);
void func_53B9C0(NEMV *);
void func_559320(NEMV *);
void func_55A440(NEMV *, s16);
void func_5655D0();
void func_566500(NEMV *, int, int);
void net_emact_set(NEMV *, s8);
void em_dur_set(NEMV *, int);

/* received payload (packet + 4): byte offsets, layouts by kind as written by net_send_em (offset - 4) */
#define PU8(o) (*(u8 *)(p + (o)))
#define PS8(o) (*(s8 *)(p + (o)))
#define PU16(o) (*(u16 *)(p + (o)))
#define PS16(o) (*(s16 *)(p + (o)))
#define PF32(o) (*(f32 *)(p + (o)))

/* status flag byte -> status effects of the enemy; kinds 1 and 2 order the 0x40/0x80 resets after the calls, kind 3 before */
#define EM_FLAGS_AB(fl) \
    if (((fl) & 1) && !(em->x7D3 & 4)) { \
        func_55A440(em, em->x7B8); \
    } \
    if (((fl) & 2) && em->x94E == 0) { \
        func_559320(em); \
    } else if (!((fl) & 2) && em->x94E != 0) { \
        em->x94E = 1; \
    } \
    if (((fl) & 4) && em->x884 == 0) { \
        em->x884 = 2; \
    } \
    if (((fl) & 8) && em->x885 == 0) { \
        em->x885 = 2; \
    } \
    if (((fl) & 0x10) && em->x56A == 0) { \
        em->x56A = 1; \
        em->x572 = 0x4650; \
    } \
    if (!((fl) & 0x10) && em->x56A != 0) { \
        em->x56A = 0; \
        em->x572 = 0; \
    } \
    if ((fl) & 0x20) { \
        if (em->x8B6 == 0) { \
            func_536110(em, em->x8B0); \
            func_536320(em); \
        } \
    } else { \
        em->x8B6 = 0; \
    }

#define EM_DUR(mask) \
    func_53B6D0(em, mask); \
    if (mask != 0) { \
        d = em->x308; \
        for (i = 0; i < 8; i++) { \
            if ((mask) & (1 << i)) { \
                d->cnt++; \
                em_dur_set(em, i); \
                if (d->cnt >= 0x63) { \
                    d->cnt = 0x63; \
                } \
            } \
            d++; \
        } \
    } \
    em->x87D = 0; \
    em->x87E = 0;

void net_receive_em(int slot0, u8 *buf) {
    u8 *p;
    NEMV *em;
    u8 kind;
    u8 a, b;
    u8 fl;
    u8 mask;
    s16 i;
    NEMDUR *d;
    u8 *q;
    u8 v;
    s8 w;

    p = buf + 4;
    if (Online_ck() != 0 && Now_Game_ck() != 0) {
        kind = buf[0];
        switch (kind) {
        case 0:
            break;
        case 1:
            if (game_w.pl_state[PU8(0x23)] == 1) {
                em = (NEMV *)(em_work + PU8(0x14) * 0xA10);
                em->posx = PF32(0);
                em->posy = PF32(4);
                em->posz = PF32(8);
                em->xA0 = PS16(0xC);
                em->xA4 = PS16(0xE);
                em->xA8 = PS16(0x10);
                a = PU8(0x12);
                b = PU8(0x13);
                em->x881 = PU8(0x15);
                em->x882 = PU8(0x16);
                em->x883 = PU8(0x17);
                em->x736 = PU8(0x1A);
                em->x08 = PS16(0x18);
                em->x39A = PU16(0x1C);
                em->x302 = PS16(0x1E);
                em->xA00 = PU8(0x25);
                if (em->xA00 != 0) {
                    em->xA00 = 0;
                    func_537620(em, (u8 *)em + 0x934);
                    func_53B9C0(em);
                    func_53A630(em, 0x2710);
                }
                em->x88B = PU8(0x26);
                if (PU8(0x1B) == 0) {
                    func_566500(em, 0, 0);
                } else {
                    func_566500(em, 1, D_642160[em->x02]);
                }
                if (em->x04 > 0) {
                    if (em->x02 == 0x1F || em->x02 == 0x1C || em->x02 == 0x1B) {
                        if (em->x14 != 5 || em->x15 != 4) {
                            em->x14 = a;
                            em->x15 = b;
                            func_535A10(em, a, b, 0);
                            net_emact_set(em, 0);
                        }
                    } else {
                        em->x14 = a;
                        em->x15 = b;
                        func_535A10(em, a, b, 0);
                        net_emact_set(em, 0);
                    }
                }
                fl = PU8(0x20);
                EM_FLAGS_AB(fl)
                if (fl & 0x40) {
                    func_538580(em);
                    func_5655D0(em);
                    em->x86F = 0;
                }
                if (fl & 0x80) {
                    func_5655D0(em);
                    em->x86F = 0;
                }
                if (em->x02 == 7) {
                    em->x45C = PU8(0x21);
                    em->x45D = PS8(0x22);
                    game_w.x21D = em->x45D;
                }
                if (em->x02 == 0xF) {
                    em->x488 = PU8(0x21);
                    em->x489 = PS8(0x22);
                }
                mask = PU8(0x24);
                EM_DUR(mask)
                return;
            }
            break;
        case 2:
            if (game_w.pl_state[PU8(0x23)] == 1) {
                a = PU8(0x12);
                b = PU8(0x13);
                em = (NEMV *)(em_work + PU8(0x14) * 0xA10);
                em->x302 = PS16(0x1E);
                if (PU8(0x1B) == 0) {
                    func_566500(em, 0, 0);
                } else {
                    func_566500(em, 1, D_642160[em->x02]);
                }
                if (em->x04 > 0) {
                    if (em->x14 != a) {
                        if (em->x15 != b) {
                            em->posx = PF32(0);
                            em->posy = PF32(4);
                            em->posz = PF32(8);
                            em->xA0 = PS16(0xC);
                            em->xA4 = PS16(0xE);
                            em->xA8 = PS16(0x10);
                            em->x881 = PU8(0x15);
                            em->x882 = PU8(0x16);
                            em->x883 = PU8(0x17);
                            em->x736 = PU8(0x1A);
                            em->x08 = PS16(0x18);
                            em->x39A = PU16(0x1C);
                            em->x14 = a;
                            em->x15 = b;
                            func_535A10(em, a, b, 0);
                            net_emact_set(em, 0);
                        }
                    }
                }
                fl = PU8(0x20);
                EM_FLAGS_AB(fl)
                if (fl & 0x40) {
                    func_538580(em);
                    func_5655D0(em);
                    em->x86F = 0;
                }
                if (fl & 0x80) {
                    func_5655D0(em);
                    em->x86F = 0;
                }
                if (em->x02 == 7) {
                    em->x45C = PU8(0x21);
                    em->x45D = PS8(0x22);
                    game_w.x21D = em->x45D;
                    return;
                }
            }
            break;
        case 3:
            if (game_w.pl_state[(u8)PS8(0x22)] == 1) {
                em = (NEMV *)(em_work + PU8(0x15) * 0xA10);
                em->posx = PF32(0);
                em->posy = PF32(4);
                em->posz = PF32(8);
                em->xA0 = PS16(0xC);
                em->xA4 = PS16(0xE);
                em->xA8 = PS16(0x10);
                em->x881 = PU16(0x1C);
                em->x882 = PU8(0x1D);
                em->x883 = PS16(0x1E);
                em->xA00 = PU8(0x24);
                if (em->xA00 != 0) {
                    em->xA00 = 0;
                    func_537620(em, (u8 *)em + 0x934);
                    func_53B9C0(em);
                    func_53A630(em, 0x2710);
                }
                em->x88B = PU8(0x25);
                if (PU8(0x1F) == 0) {
                    func_566500(em, 0, 0);
                } else {
                    func_566500(em, 1, D_642160[em->x02]);
                }
                b = PU8(0x13);
                a = PU8(0x12);
                if (em->x04 > 0) {
                    em->x14 = a;
                    em->x15 = b;
                    func_535A10(em, a, b, 0);
                    net_emact_set(em, 0);
                    switch (em->x02) {
                    case 26:
                    case 14:
                    case 11:
                    case 1:
                        if (a == 4 && b == 0xF) {
                            func_53B8A0(em);
                        }
                        break;
                    case 22:
                    case 17:
                        if (a == 4 && b == 0x11) {
                            func_53B8A0(em);
                        }
                        break;
                    }
                }
                em->x302 = PU8(0x16);
                em->x954 = PS16(0x18);
                em->x08 = (s16)PU8(0x1A);
                fl = PU8(0x14);
                EM_FLAGS_AB(fl)
                if (fl & 0x40) {
                    em->x86F = 0;
                    func_538580(em);
                    func_5655D0(em);
                }
                if (fl & 0x80) {
                    em->x86F = 0;
                    func_5655D0(em);
                }
                if (em->x02 == 7) {
                    em->x45C = PU8(0x20);
                    em->x45D = PU8(0x21);
                    game_w.x21D = em->x45D;
                }
                if (em->x02 == 0xF) {
                    em->x488 = PU8(0x20);
                    em->x489 = PU8(0x21);
                }
                mask = PU8(0x23);
                EM_DUR(mask)
                return;
            }
            break;
        case 4:
            em = (NEMV *)(em_work + PU8(0x15) * 0xA10);
            if (em->x02 != 2) {
                em->posx = PF32(0);
                em->posy = PF32(4);
                em->posz = PF32(8);
                em->xA0 = PS16(0xC);
                em->xA4 = PS16(0xE);
                em->xA8 = PS16(0x10);
            }
            em->x302 = PU8(0x12);
            em->x56A = PU8(0x17);
            em->x572 = PS16(0x18);
            em->x88E = PU8(0x14);
            em->x827 = PU8(0x1B);
            em->x828 = PU16(0x1C);
            em->x829 = PU8(0x1D);
            em->xA00 = PU8(0x20);
            em->x88B = PU8(0x21);
            em->x7C0 = PS8(0x22);
            em->x881 = em->x827;
            em->x882 = em->x828;
            em->x883 = em->x829;
            mask = PU8(0x1A);
            EM_DUR(mask)
            if (em->x02 == 7) {
                v = PS16(0x1E);
                q = (u8 *)em + 0x444;
                if (em->x45C < v) {
                    q[0x18] = v;
                }
                w = PU8(0x1F);
                if ((s8)q[0x19] < w) {
                    q[0x19] = w;
                }
                game_w.x21D = q[0x19];
                if (em->x736 != PU8(0x16)) {
                    q[0x18] = 0;
                    func_5655D0(em, q);
                }
                if (game_w.master == em->x88E) {
                    func_5655D0(em);
                }
            }
            if (em->x02 == 0xF) {
                em->x488 = PS16(0x1E);
                em->x489 = PU8(0x1F);
                q = (u8 *)em + 0x444;
                if (game_w.master == em->x88E && q[0x45] != 0) {
                    func_5655D0(em, q);
                }
            }
            em->x736 = PU8(0x16);
            if (game_w.master == em->x88E) {
                em->x8C3 = 0;
            }
            break;
        }
    }
}
