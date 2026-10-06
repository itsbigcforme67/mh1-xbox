/* netdev2_nm (not built): near-matches in the network device / PPP area, written new in this pass from m2c drafts.
 * CpInetPppStart (0x00235E40, 720 bytes): about 18 instructions off. Differences: the PPP scratch byte is at sp+63 where the original has it at
 * sp+60 (sp3C), and the original leaves branch delay slots empty (`beq; nop`) in the w[2]/w[4] compare ladders where we hoist the constant into
 * the slot; every if/else-if/switch form of those ladders tried is no closer. The jump table (7 entries, cases 0-4 plus default) needs a
 * `main:rodata` line when this is linked.
 * InetDnsSetAll (0x00237DC0, 292 bytes): about 21 off; the MyEtherInitMode ladder tests 1, 0, 2 in the original, and the epilogue/nops differ.
 */
#include "types.h"
extern s32 Inet_interface_status[4];
typedef struct PPPB { s16 w0; s16 w2; s8 *p4; } PPPB;
typedef struct PPPC { s16 w[5]; } PPPC;
typedef struct PPPA { PPPC *c; PPPB *b; s16 *m; s32 x[5]; } PPPA;
typedef struct PPPARG { PPPA *a; u8 pad4[2]; s8 x6; s8 x7; } PPPARG;
typedef struct PPPSTART {
    s8 f0;
    s8 f1;
    s16 f2;
    s32 w[5];
    s8 *p;
    s16 s0;
    s16 s1;
    s16 s2;
    s16 s3;
    s16 s4;
    s8 f36;
    s8 f37;
} PPPSTART;
int Ave_PppStart();

int CpInetPppStart(PPPARG *arg) {
    PPPSTART st;
    s8 dummy;
    int v;
    s16 t1;
    s16 t2;
    s16 t3;
    s16 t4;

    dummy = 0;
    st.f0 = 1;
    st.f36 = arg->x6;
    st.f1 = arg->x7;
    st.w[0] = arg->a->x[0];
    st.w[1] = arg->a->x[1];
    st.w[2] = arg->a->x[2];
    st.w[3] = arg->a->x[3];
    st.w[4] = arg->a->x[4];
    st.p = arg->a->b->p4;
    if (st.p == 0) {
        st.p = &dummy;
    }
    switch (arg->a->c->w[0]) {
    case 6:
    default:
        st.s0 = 6;
        break;
    case 4:
        st.s0 = 5;
        break;
    case 3:
        st.s0 = 4;
        break;
    case 2:
        st.s0 = 3;
        break;
    case 1:
        st.s0 = 2;
        break;
    case 0:
        st.s0 = 0;
        break;
    }
    if (arg->a->c->w[1] != 1) {
        st.s1 = 0;
    } else {
        st.s1 = 1;
    }
    t1 = arg->a->c->w[2];
    if (t1 != 0) {
        if (t1 != 1) {
            st.s2 = 2;
        } else {
            st.s2 = 1;
        }
    } else {
        st.s2 = 0;
    }
    if (arg->a->c->w[3] != 0) {
        st.s3 = 3;
    } else {
        st.s3 = 2;
    }
    t2 = arg->a->c->w[4];
    if (t2 != 0) {
        if (t2 != 2) {
            st.s4 = 1;
        } else {
            st.s4 = 2;
        }
    } else {
        st.s4 = 0;
    }
    if (arg->a->b->w0 != 1) {
        st.f2 = 0;
    } else {
        st.f2 = 1;
    }
    t3 = arg->a->b->w2;
    if (t3 != 1) {
        *st.p = 0;
    }
    t4 = *arg->a->m;
    switch (t4) {
    case 0:
        st.f37 = 0;
        break;
    case 1:
        st.f37 = 1;
        break;
    case 2:
        st.f37 = 2;
        break;
    case 3:
        st.f37 = 4;
        break;
    }
    v = Ave_PppStart(&st, t3);
    Inet_interface_status[0] = 2;
    return (s16)v;
}

extern s32 *CurDevice;
extern s32 MyDns1;
extern s32 MyDns2;
extern u8 MyDomain[0x100];
extern s8 MyEtherInitMode;
extern s32 NdgConfParam[2];
int CpInetPppGetDns();
int CpInetDhcpGetDns();
int CpInetDnsInitialize();
void InetDnsCacheInitialize();
void CpInetHttpResolvCacheInitialize();

void InetDnsSetAll(void) {
    s32 out[6];
    s32 a2;
    s32 a0;
    s32 d1;
    s32 d2;

    a2 = 0;
    a0 = 0;
    if (CurDevice != 0) {
        if (MyDns1 == 0 || MyDns2 == 0) {
            switch (*CurDevice) {
            case 2:
                CpInetPppGetDns(out);
                a2 = out[2];
                a0 = out[3];
                break;
            case 1:
                switch (MyEtherInitMode) {
                case 2:
                    CpInetPppGetDns(out);
                    a2 = out[2];
                    a0 = out[3];
                    break;
                case 0:
                    break;
                case 1:
                    CpInetDhcpGetDns(MyDomain, NdgConfParam);
                    a2 = NdgConfParam[0];
                    a0 = NdgConfParam[1];
                    break;
                }
                break;
            }
        }
        d1 = MyDns1;
        if (d1 != 0) {
        } else {
            d1 = a2;
        }
        d2 = MyDns2;
        if (d2 != 0) {
        } else {
            d2 = a0;
        }
        CpInetDnsInitialize(MyDomain, d1, d2);
        InetDnsCacheInitialize();
        CpInetHttpResolvCacheInitialize();
    }
}
