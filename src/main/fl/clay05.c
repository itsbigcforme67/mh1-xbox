/* fl clay (SLPM_654.95 0x0016C690-0x0016D8F0, one source file): material DMA packets (half colour, per-material
 * data builder, texture register retouch), the per-clay texture list, the DMA tag rebase after a clay moved and
 * flExecuteClay (kept raw, see below). Field names are guesses from use. */
#include "types.h"

typedef unsigned __int128 u128;

typedef struct FLMATR {         /* flMATERIAL entry, 0x4C bytes */
    u8 x00[4];
    f32 f[12];          /* 0x04..0x30 colours */
    u8 x34[0x44 - 0x34];
    s32 tex;            /* 0x44 */
    s32 key;            /* 0x48 */
} FLMATR;

typedef struct CLAYV {          /* model type descriptor, 0x4C of a clay */
    s32 x00;
    s32 a;              /* 0x04 */
    s32 b;              /* 0x08 */
    s32 c;              /* 0x0C */
    long (*fn)();       /* 0x10 builds the draw packets */
    s32 mode;           /* 0x14 index into material_data_size */
} CLAYV;

typedef struct CLAYT {
    s32 flags;          /* 0x000 bit 1 prebuilt, bit 2 */
    s32 used;           /* 0x004 */
    s32 x08;
    s32 rs;             /* 0x00C render state it was built for */
    u8 x10[0x1C - 0x10];
    s32 mem2;           /* 0x01C system buffer handle */
    u8 *base;           /* 0x020 buffer address */
    s32 tagOfs;         /* 0x024 */
    u32 size;           /* 0x028 */
    s32 tblOfs;         /* 0x02C offset table (one chain start per entry) */
    s32 x30;
    s32 cnt;            /* 0x034 material / chain count */
    s32 ntex;           /* 0x038 */
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 matkey;         /* 0x048 */
    CLAYV *v;           /* 0x04C */
    u8 x50[0x60 - 0x50];
    u8 mat[0x20];       /* 0x060 material index per entry */
    s32 mattex[0x20];   /* 0x080 */
    s32 texlist[0x20];  /* 0x100 */
    u8 x180[0x180 - 0x180];
} CLAYT;

typedef struct FLST {
    u8 x00[0x3EC];
    s32 lastKey;        /* 0x3EC */
    u32 nprim;          /* 0x3F0 */
    s32 skip;           /* 0x3F4 */
} FLST;

extern FLST flPs2State;
extern s32 flSystemRenderState;
extern u8 flPs2VIF1Control[];
u8 *flPS2GetSystemTmpBuff();
int flPS2DmaAddQueue2();
void flPS2DmaTerminate();
void flPS2ReleaseSystemMemory();
void flPS2GetMLCLAY();
int flPS2CreateClay();
void flCompact();
void flPS2ReloadTexture();
void flPS2RetouchMaterialTexData();
void flPS2MakeMaterialDmaData();
void func_0016AD48();
void flPS2ClayMakeTextureList(struct CLAYT *, int);

extern CLAYT flPS2Clay[0x180];
u8 *flPS2GetSystemBuffAdrs(int);
void flPS2ClayRetouchMaterialTag_sub(CLAYT *);


extern FLMATR flMATERIAL[];
extern s32 material_data_size[];
void flPS2DmaAddEndTag();
void flPS2DmaAddCallTag();
void flPS2VIF1CodeAddUnpackr();
void flPS2VIF1CodeAddMscnt();
void flPS2SetMaterialData();
void flPS2SetTextureRegister();
extern int flSystemRenderOperation;
extern int flTextureStage[2];
void flPS2SetMaterialData_sub();
void flPS2SetMaterialData_sub_mult();

f32 flPS2HalfColorSub(u32 c, int mode) {
    f32 f = 0.0f;

    if (mode == 0) {
        if (c == 0xFF) {
            f = 128.0f;
        } else if (c != 0) {
            c >>= 1;
            if (c == 0) {
                c = 1;
            }
            f = (f32)c;
        }
    } else {
        f = (f32)c / 255.0f;
        if (!(f <= 1.0f)) {
            f = 1.0f;
        }
        if (f < 0.0f) {
            f = 0.0f;
        }
    }
    return f;
}

void flPS2MakeMaterialDmaData(CLAYT *c, int n, int mode, u32 *buf, u8 *idx) {
    int i;
    s32 *tbl;
    int step;
    u8 *base;

    step = material_data_size[mode];
    base = c->base;
    tbl = (s32 *)(base + c->tblOfs);
    for (i = 0; i < n; i++) {
        flPS2SetMaterialData(base + tbl[idx[i]], buf, mode, idx[i]);
        buf = (u32 *)((u8 *)buf + step);
    }
    flPS2DmaAddEndTag(buf, 1, 1, 0);
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
}

#define BW(o) (*(u32 *)(buf + (o)))
#define BF(o) (*(f32 *)(buf + (o)))
#define BL(o) (*(long *)(buf + (o)))
#define HDR(qwc, unp, flag)                                \
    flPS2DmaAddCallTag(buf, qwc, addr, 0, 0);              \
    flPS2VIF1CodeAddUnpackr(buf + 0x10, unp, 0);           \
    *(u128 *)(buf + 0x20) = 0;                             \
    BL(0x30) = (long)(flag) | ((long)0x10000000 << 32);    \
    BW(0x38) = 0xE;                                        \
    BW(0x3C) = 0

static void flPS2SetMaterialData(u32 addr, void *b0, u32 mode, int idx) {
    FLMATR *m;

    m = &flMATERIAL[idx];
    switch (mode) {
    case 0:
        {
        u8 *buf = (u8 *)b0;
        HDR(0xC, 0xA, 0x8006);
        flPS2SetMaterialData_sub(m->tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90);
        BF(0xA0) = m->f[0];
        BF(0xA4) = m->f[1];
        BF(0xA8) = m->f[2];
        BF(0xAC) = 128.0f * m->f[3];
        BF(0xB0) = m->f[8];
        BF(0xB4) = m->f[9];
        BF(0xB8) = m->f[10];
        BF(0xBC) = m->f[11];
        flPS2VIF1CodeAddMscnt(buf + 0xC0);
        }
        break;
    case 1:
        {
        u8 *buf = (u8 *)b0;
        HDR(0xC, 0xA, 0x8006);
        BL(0x40) = 0;
        BL(0x48) = 0x3B;
        BL(0x50) = 0;
        BL(0x58) = 0x14;
        BL(0x60) = 0;
        BL(0x68) = 6;
        BL(0x70) = 0;
        BL(0x78) = 8;
        BL(0x80) = 0;
        BL(0x88) = 0x34;
        BL(0x90) = 0;
        BL(0x98) = 0x36;
        BF(0xA0) = m->f[0];
        BF(0xA4) = m->f[1];
        BF(0xA8) = m->f[2];
        BF(0xAC) = 128.0f * m->f[3];
        BF(0xB0) = m->f[8];
        BF(0xB4) = m->f[9];
        BF(0xB8) = m->f[10];
        BF(0xBC) = m->f[11];
        flPS2VIF1CodeAddMscnt(buf + 0xC0);
        }
        break;
    case 2:
        {
        u8 *buf = (u8 *)b0;
        HDR(0xE, 0xC, 0x8006);
        flPS2SetMaterialData_sub(m->tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90);
        BF(0xA0) = m->f[0];
        BF(0xA4) = m->f[1];
        BF(0xA8) = m->f[2];
        BF(0xAC) = 128.0f * m->f[3];
        BF(0xB0) = m->f[8];
        BF(0xB4) = m->f[9];
        BF(0xB8) = m->f[10];
        BF(0xBC) = m->f[11];
        BF(0xC0) = m->f[4];
        BF(0xC4) = m->f[5];
        BF(0xC8) = m->f[6];
        BF(0xCC) = m->f[7];
        {
            int a;
            int b;

            switch (m->key) {
            case 0:
            case 1:
            case 2:
                a = 1;
                b = 1;
                break;
            case 4:
                a = 2;
                b = 1;
                break;
            case 8:
                a = 3;
                b = 1;
                break;
            case 0x10:
                a = 4;
                b = 1;
                break;
            case 0x20:
                a = 5;
                b = 1;
                break;
            case 0x40:
                a = 6;
                b = 1;
                break;
            case 0x80:
                a = 7;
                b = 1;
                break;
            case 0x100:
                a = 8;
                b = 1;
                break;
            default:
                a = m->key - 1;
                b = 0;
                break;
            }
            BW(0xD0) = a;
            BW(0xD4) = b;
            BW(0xD8) = 0;
            BW(0xDC) = 0;
        }
        flPS2VIF1CodeAddMscnt(buf + 0xE0);
        }
        break;
    case 3:
        {
        u8 *buf = (u8 *)b0;
        HDR(0xA, 8, 0x8006);
        flPS2SetMaterialData_sub(m->tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90);
        flPS2VIF1CodeAddMscnt(buf + 0xA0);
        }
        break;
    case 4:
        {
        u8 *buf = (u8 *)b0;
        HDR(0xB, 9, 0x8006);
        BL(0x40) = 0;
        BL(0x48) = 0x3B;
        BL(0x50) = 0;
        BL(0x58) = 0x14;
        BL(0x60) = 0;
        BL(0x68) = 6;
        BL(0x70) = 0;
        BL(0x78) = 8;
        BL(0x80) = 0;
        BL(0x88) = 0x34;
        BL(0x90) = 0;
        BL(0x98) = 0x36;
        *(u128 *)(buf + 0xA0) = 0;
        flPS2VIF1CodeAddMscnt(buf + 0xB0);
        }
        break;
    case 5:
        {
        u8 *buf = (u8 *)b0;
        HDR(0x10, 0xE, 0x800B);
        flPS2SetMaterialData_sub_mult(m->tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90,
                                      flTextureStage[1], buf + 0xA0, buf + 0xB0, buf + 0xC0, buf + 0xD0, buf + 0xE0);
        BF(0xF0) = m->f[0];
        BF(0xF4) = m->f[1];
        BF(0xF8) = m->f[2];
        BF(0xFC) = 128.0f * m->f[3];
        flPS2VIF1CodeAddMscnt(buf + 0x100);
        }
        break;
    }
}

void flPS2RetouchMaterialTexData(int n, int mode, u8 *buf, int *tex) {
    int i;
    int step;

    step = material_data_size[mode];
    switch (mode) {
    case 0:
        for (i = 0; i < n; i++) {
            flPS2SetMaterialData_sub(*tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90);
            buf += step;
            tex++;
        }
        break;
    case 1:
    case 4:
        break;
    case 2:
        for (i = 0; i < n; i++) {
            flPS2SetMaterialData_sub(*tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90);
            buf += step;
            tex++;
        }
        break;
    case 3:
        for (i = 0; i < n; i++) {
            flPS2SetMaterialData_sub(*tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90);
            buf += step;
            tex++;
        }
        break;
    case 5:
        for (i = 0; i < n; i++) {
            flPS2SetMaterialData_sub_mult(*tex, buf + 0x40, buf + 0x50, buf + 0x60, buf + 0x70, buf + 0x80, buf + 0x90,
                                          flTextureStage[1], buf + 0xA0, buf + 0xB0, buf + 0xC0, buf + 0xD0, buf + 0xE0);
            buf += step;
            tex++;
        }
        break;
    }
}

void flPS2SetMaterialData_sub(int tex, long *a, long *b, long *c, long *d, long *e, long *f) {
    if (tex != 0) {
        flPS2SetTextureRegister(tex, a, b, c, d, e, f, flSystemRenderOperation);
        a[1] = 0x3B;
        b[1] = 0x14;
        c[1] = 6;
        d[1] = 8;
        e[1] = 0x34;
        f[1] = 0x36;
    } else {
        a[0] = 0;
        a[1] = 0x3B;
        b[0] = 0;
        b[1] = 0x14;
        c[0] = 0;
        c[1] = 6;
        d[0] = 0;
        d[1] = 8;
        e[0] = 0;
        e[1] = 0x34;
        f[0] = 0;
        f[1] = 0x36;
    }
}

void flPS2SetMaterialData_sub_mult(int tex, long *a, long *b, long *c, long *d, long *e, long *f, int tex2, long *q1, long *q2,
                                   long *q3, long *q4, long *q5) {
    if (tex != 0) {
        flPS2SetTextureRegister(tex, a, b, c, d, e, f, flSystemRenderOperation);
        a[1] = 0x3B;
        b[1] = 0x14;
        c[1] = 6;
        d[1] = 8;
        e[1] = 0x34;
        f[1] = 0x36;
    } else {
        a[0] = 0;
        a[1] = 0x3B;
        b[0] = 0;
        b[1] = 0x14;
        c[0] = 0;
        c[1] = 6;
        d[0] = 0;
        d[1] = 8;
        e[0] = 0;
        e[1] = 0x34;
        f[0] = 0;
        f[1] = 0x36;
    }
    if (tex2 != 0) {
        flPS2SetTextureRegister(tex2, a, q1, q2, q3, q4, q5, flSystemRenderOperation);
        a[1] = 0x3B;
        q1[1] = 0x15;
        q2[1] = 7;
        q3[1] = 9;
        q4[1] = 0x34;
        q5[1] = 0x36;
    } else {
        a[0] = 0;
        a[1] = 0x3B;
        q1[0] = 0;
        q1[1] = 0x15;
        q2[0] = 0;
        q2[1] = 7;
        q3[0] = 0;
        q3[1] = 9;
        q4[0] = 0;
        q4[1] = 0x35;
        q5[0] = 0;
        q5[1] = 0x37;
    }
}

void flPS2ClayMakeTextureList(CLAYT *c, int unused) {
    int i;
    int j;
    FLMATR *m;

    c->ntex = 0;
    for (i = 0; i < c->cnt; i++) {
        m = &flMATERIAL[c->mat[i]];
        c->mattex[i] = 0;
        if (m->tex != 0) {
            c->mattex[i] = m->tex;
            for (j = 0; j < c->ntex; j++) {
                if (c->texlist[j] == m->tex) {
                    goto next;
                }
            }
            c->texlist[c->ntex++] = m->tex;
        }
    next:;
    }
}

void flPS2ClayRetouchMaterialTag(void) {
    int i;
    CLAYT *c;

    c = flPS2Clay;
    for (i = 0; i < 0x180; i++, c++) {
        if (c->used != 0 && c->mem2 != 0) {
            flPS2ClayRetouchMaterialTag_sub(c);
        }
    }
}

void flPS2ClayRetouchMaterialTag_sub(CLAYT *c) {
    u8 *old;
    u8 *b;
    u32 i;
    u32 *tbl;
    u8 *tag;
    u8 *r;
    u8 *n;
    u32 t;
    u8 *w;

    old = c->base;
    b = flPS2GetSystemBuffAdrs(c->mem2);
    c->base = b;
    tbl = (u32 *)(b + c->tblOfs);
    tag = b + c->tagOfs;
    for (i = 0; i < (u32)c->cnt; i++, tbl++) {
        r = b + *tbl;
        *(u8 **)(tag + 4) = r;
        w = r;
        tag += (*(u16 *)tag + 1) * 16;
        do {
            t = *(u32 *)w;
            switch (t & 0x70000000) {
            case 0x10000000:
                w += ((t & 0xFFFF) + 1) * 16;
                break;
            case 0x20000000:
                n = (u8 *)(*(int *)(w + 4) - (int)old);
                n += (int)b;
                *(u8 **)(w + 4) = n;
                w = n;
                break;
            case 0x60000000:
                r = 0;
                break;
            }
        } while (r != 0);
    }
}

/* original bytes: build/raw/flExecuteClay.inc (config/c_rawfuncs.txt); the C below is a near-match (12 instructions off,
 * scheduling only: the original copies the temp-buffer result into its register in the delay slot of the next call), used by the PC build */
#ifdef __MWERKS__
asm int flExecuteClay(int h)
{
#include "flExecuteClay.inc"
}
#else
int flExecuteClay(int h) {
    CLAYT *c;
    u8 *buf;
    u8 *base;
    long r;
    int *sz;
    u32 *p;
    u8 *nb;
    int mlc[12];

    c = &flPS2Clay[h - 1];
    base = c->base;
    if (flPs2State.skip != 0) {
        return 1;
    }
    if (!(c->flags & 4)) {
        if (c->rs != flSystemRenderState) {
            flPS2DmaTerminate(c->flags);
            c->rs = flSystemRenderState;
            if (c->mem2 != 0) {
                flPS2ReleaseSystemMemory(c->mem2);
            }
            flPS2GetMLCLAY(c, mlc);
            if (flPS2CreateClay(c, mlc) == 0) {
                return 0;
            }
            flCompact();
            buf = c->base + c->tagOfs;
        } else if (c->flags & 2) {
            buf = base + c->tagOfs;
        } else {
            buf = flPS2GetSystemTmpBuff(c->size, 0x10);
            flPS2MakeMaterialDmaData(c, c->cnt, c->v->mode, (u32 *)buf, c->mat);
            flPS2ClayMakeTextureList(c, 0);
        }
        if (c->ntex != 0) {
            flPS2ReloadTexture(c->ntex, c->texlist);
            flPS2RetouchMaterialTexData(c->cnt, c->v->mode, buf, c->mattex);
        }
    } else if (c->flags & 2) {
        buf = flPS2GetSystemTmpBuff(c->size, 0x10);
        func_0016AD48(base + c->tagOfs, buf, c->size >> 4);
        if (c->ntex != 0) {
            flPS2ReloadTexture(c->ntex, c->texlist);
            flPS2RetouchMaterialTexData(c->cnt, c->v->mode, buf, c->mattex);
        }
    } else {
        buf = flPS2GetSystemTmpBuff(c->size, 0x10);
        flPS2ClayMakeTextureList(c, 0);
        if (c->ntex != 0) {
            flPS2ReloadTexture(c->ntex, c->texlist);
        }
        flPS2MakeMaterialDmaData(c, c->cnt, c->v->mode, (u32 *)buf, c->mat);
    }
    sz = &material_data_size[c->v->mode];
    nb = buf + c->cnt * *sz;
    flPs2State.nprim = (u32)(c->v->c - c->v->b) >> 3;
    r = c->v->fn(c, (u32)(c->v->a - c->v->b) >> 3, buf, sz);
    if (flPs2State.lastKey != c->matkey) {
        flPs2State.lastKey = c->matkey;
        p = (u32 *)flPS2GetSystemTmpBuff(0x40, 0x10);
        p[0] = 0x50000001;
        p[1] = c->v->x00;
        *(long *)(p + 2) = 0;
        p[8] = 0xF0000001;
        p[9] = 0;
        *(long *)(p + 10) = 0;
        *(u128 *)(p + 12) = 0;
        *(u128 *)(p + 4) = 0;
        flPS2DmaAddQueue2(0, (unsigned long)p & 0xFFFFFFFUL, p + 8, flPs2VIF1Control);
    }
    flPS2DmaAddQueue2(0, (unsigned long)r & 0xFFFFFFFUL | 0x50000000, nb, flPs2VIF1Control);
    return 1;
}
#endif
