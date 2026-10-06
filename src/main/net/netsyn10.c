/* Network play sync, enemy packets (SLPM_654.95 0x001BA8E0-0x001BB000): net_send_em, sends the master's enemy state (position, angles, state
 * counters, a flag byte built from several work fields) in four packet kinds, 0x2C bytes each. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;

void net_emact_set(NEMV *, s8);

typedef union NEMPK {
    u8 pad[0x40];
    struct {                          /* kinds 1 and 2 */
        u8 cmd, len, x2, x3;
        f32 f4, f8, fC;
        s16 s10, s12, s14;
        u8 b16, b17, b18, b19, b1A, b1B;
        u16 u1C;
        u8 b1E, b1F;
        u16 u20;
        s16 s22;
        u8 flags, b25;
        s8 c26;
        u8 b27, b28;
        s8 c29;
        u8 b2A;
    } a;
    struct {                          /* kind 3 */
        u8 cmd, len, x2, x3;
        f32 f4, f8, fC;
        s16 s10, s12, s14;
        u8 b16, b17, flags, b19;
        s16 s1A;
        u16 u1C;
        s16 s1E;
        u8 b20, b21, b22, b23, b24, b25, b26, b27;
        s8 c28;
        u8 b29;
    } c;
    struct {                          /* kind 4 */
        u8 cmd, len, x2, x3;
        f32 f4, f8, fC;
        s16 s10, s12, s14, s16_;
        u8 b18, b19, b1A, b1B;
        s16 s1C;
        u8 b1E, b1F, b20, b21, b22, b23;
        s8 c24;
        u8 b25;
        s16 s26;
    } d;
} NEMPK;

void net_send_em(NEMV *em, u8 kind, s8 arg2) {
    NEMPK pk;
    u8 f;

    if (Online_ck() != 0) {
        if (Now_Game_ck() != 0) {
            pk.a.x3 = 0;
            pk.a.x2 = 0;
            switch (kind) {
            case 0:
                break;
            case 1:
                pk.a.cmd = kind;
                pk.a.len = 0x2C;
                pk.a.f4 = em->posx;
                pk.a.f8 = em->posy;
                pk.a.fC = em->posz;
                pk.a.s10 = em->xA0;
                pk.a.s12 = em->xA4;
                pk.a.s14 = em->xA8;
                pk.a.b16 = em->x794;
                pk.a.b17 = em->x795;
                pk.a.b19 = em->x797;
                pk.a.b1A = em->x7A8;
                pk.a.b1B = em->x7A9;
                pk.a.b1E = em->x736;
                pk.a.b18 = em->x0C;
                pk.a.u1C = em->x08;
                pk.a.b1F = em->x888;
                pk.a.u20 = em->x954;
                pk.a.s22 = em->x302;
                pk.a.b28 = em->x87E;
                em->x87E = 0;
                pk.a.c29 = em->xA00;
                em->xA00 = 0;
                pk.a.b2A = em->x88B;
                f = 0;
                if (em->x7D3 & 4) f |= 1;
                if (em->x94E != 0) f |= 2;
                if (em->x884 != 0) f |= 4;
                if (em->x885 != 0) f |= 8;
                if (em->x56A != 0) f |= 0x10;
                if (em->x8B6 != 0) f |= 0x20;
                if (em->x86F == 1) {
                    f |= 0x40;
                    em->x86F = 0;
                }
                if (em->x86F == 2) {
                    f |= 0x80;
                    em->x86F = 0;
                }
                pk.a.flags = f;
                if (em->x02 == 7) {
                    pk.a.b25 = em->x45C;
                    pk.a.c26 = em->x45D;
                    em->x462 = 0x96;
                }
                if (em->x02 == 0xF) {
                    pk.a.b25 = em->x488;
                    pk.a.c26 = em->x489;
                }
                pk.a.b27 = game_w.master;
                net_emact_set(em, arg2);
                break;
            case 2:
                pk.a.cmd = kind;
                pk.a.len = 0x2C;
                pk.a.f4 = em->posx;
                pk.a.f8 = em->posy;
                pk.a.fC = em->posz;
                pk.a.s10 = em->xA0;
                pk.a.s12 = em->xA4;
                pk.a.s14 = em->xA8;
                pk.a.b16 = em->x14;
                pk.a.b17 = em->x15;
                pk.a.b19 = em->x797;
                pk.a.b1A = em->x7A8;
                pk.a.b1B = em->x7A9;
                pk.a.b1E = em->x736;
                pk.a.b18 = em->x0C;
                pk.a.u1C = em->x08;
                pk.a.b1F = em->x888;
                pk.a.u20 = em->x954;
                pk.a.s22 = em->x302;
                f = 0;
                if (em->x7D3 & 4) f |= 1;
                if (em->x94E != 0) f |= 2;
                if (em->x884 != 0) f |= 4;
                if (em->x885 != 0) f |= 8;
                if (em->x56A != 0) f |= 0x10;
                if (em->x8B6 != 0) f |= 0x20;
                if (em->x86F == 1) {
                    f |= 0x40;
                    em->x86F = 0;
                }
                if (em->x86F == 2) {
                    f |= 0x80;
                    em->x86F = 0;
                }
                pk.a.flags = f;
                if (em->x02 == 7) {
                    pk.a.b25 = em->x45C;
                    pk.a.c26 = em->x45D;
                    em->x462 = 0x96;
                }
                pk.a.b27 = game_w.master;
                break;
            case 3:
                pk.c.cmd = kind;
                pk.c.len = 0x2C;
                pk.c.f4 = em->posx;
                pk.c.f8 = em->posy;
                pk.c.fC = em->posz;
                pk.c.s10 = em->xA0;
                pk.c.s12 = em->xA4;
                pk.c.s14 = em->xA8;
                pk.c.b16 = em->x794;
                pk.c.b17 = em->x795;
                pk.c.b19 = em->x0C;
                pk.c.s1A = em->x302;
                pk.c.u1C = em->x954;
                pk.c.s1E = em->x08;
                pk.c.b20 = em->x797;
                pk.c.b21 = em->x7A8;
                pk.c.b22 = em->x7A9;
                pk.c.b23 = em->x888;
                pk.c.b27 = em->x87E;
                em->x87E = 0;
                pk.c.c28 = em->xA00;
                em->xA00 = 0;
                pk.c.b29 = em->x88B;
                f = 0;
                if (em->x7D3 & 4) f |= 1;
                if (em->x94E != 0) f |= 2;
                if (em->x884 != 0) f |= 4;
                if (em->x885 != 0) f |= 8;
                if (em->x56A != 0) f |= 0x10;
                if (em->x8B6 != 0) f |= 0x20;
                if (em->x86F == 1) {
                    f |= 0x40;
                    em->x86F = 0;
                }
                if (em->x86F == 2) {
                    f |= 0x80;
                    em->x86F = 0;
                }
                pk.c.flags = f;
                pk.c.b26 = game_w.master;
                if (em->x02 == 7) {
                    pk.c.b24 = em->x45C;
                    pk.c.b25 = em->x45D;
                    em->x462 = 0x96;
                }
                if (em->x02 == 0xF) {
                    pk.c.b24 = em->x488;
                    pk.c.b25 = em->x489;
                }
                break;
            case 4:
                pk.d.cmd = kind;
                pk.d.len = 0x2C;
                pk.d.b19 = em->x0C;
                pk.d.f4 = em->posx;
                pk.d.f8 = em->posy;
                pk.d.fC = em->posz;
                pk.d.s10 = em->xA0;
                pk.d.s12 = em->xA4;
                pk.d.s14 = em->xA8;
                pk.d.s16_ = em->x302;
                pk.d.b18 = em->x949;
                pk.d.b1A = em->x736;
                pk.d.b1B = em->x56A;
                pk.d.s1C = em->x572;
                pk.d.b1E = em->x87E;
                pk.d.b1F = em->x827;
                pk.d.b20 = em->x828;
                pk.d.b21 = em->x829;
                pk.d.s26 = em->x7C0;
                em->x87E = 0;
                pk.d.c24 = em->xA00;
                em->xA00 = 0;
                pk.d.b25 = em->x88B;
                if (em->x02 == 7) {
                    pk.d.b22 = em->x45C;
                    pk.d.b23 = em->x45D;
                    em->x462 = 0x96;
                }
                if (em->x02 == 0xF) {
                    pk.d.b22 = em->x488;
                    pk.d.b23 = em->x489;
                }
                break;
            }
            AQ_data_put(8, pk.pad, 0);
        }
    }
}
