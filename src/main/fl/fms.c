/* Frame memory stack ("fms"). SLPM_654.95 0x0016ABB0-0x0016ACC8: one source file, four functions.
 * The stack has a low end growing up (cur) and a high end growing down (top); a "frame" remembers one end so
 * it can be released later (frm[0] = cur, frm[1] = top). Field names are guesses from use. */
typedef unsigned int u32;

typedef struct FMSTK {
    u32 base;           /* 0x00 */
    u32 lo;             /* 0x04 aligned start */
    u32 hi;             /* 0x08 aligned end */
    u32 frm[2];         /* 0x0C cur (low end), 0x10 top (high end) */
    u32 align;          /* 0x14 */
} FMSTK;

typedef struct FMSOUT {
    u32 frame;
    int no;
} FMSOUT;

/* original bytes: build/raw/fmsInitialize.inc (config/c_rawfuncs.txt); the C below is a near-match (8 of 30 instructions off, scheduling), used by the PC build */
#ifdef __MWERKS__
asm int fmsInitialize(FMSTK *p, u32 base, u32 size, u32 align)
{
#include "fmsInitialize.inc"
}
#else
int fmsInitialize(FMSTK *p, u32 base, u32 size, u32 align) {
    u32 mask = ~(align - 1);
    u32 sz = mask & (size + align - 1);

    p->base = base;
    p->align = align;
    p->lo = ~(p->align - 1) & ((u32)p->base + p->align - 1);
    p->hi = ~(p->align - 1) & (p->align + (p->base + sz) - 1);
    p->frm[0] = p->lo;
    p->frm[1] = p->hi;
    return 1;
}
#endif

/* Allocates size bytes (rounded up to the alignment) from the low end, or from the high end
 * when fromTop is set. Returns the address or 0 when the two ends would meet. */
/* original bytes: build/raw/fmsAllocMemory.inc (config/c_rawfuncs.txt); the C below is a near-match (11 of 24 off, scheduling), used by the PC build */
#ifdef __MWERKS__
asm u32 fmsAllocMemory(FMSTK *p, u32 size, int fromTop)
{
#include "fmsAllocMemory.inc"
}
#else
u32 fmsAllocMemory(FMSTK *p, u32 size, int fromTop) {
    u32 cur = p->frm[0];
    u32 top = p->frm[1];
    u32 sz = ~(p->align - 1) & (size + p->align - 1);

    if (top < cur + sz) {
        return 0;
    }
    if (fromTop) {
        p->frm[1] = top - sz;
        return p->frm[1];
    }
    p->frm[0] = cur + sz;
    return cur;
}
#endif

int fmsGetFrame(FMSTK *f, int no, FMSOUT *out) {
    out->frame = f->frm[no];
    out->no = no;
    return 1;
}

void fmsReleaseFrame(FMSTK *f, FMSOUT *out) {
    f->frm[out->no] = out->frame;
}
