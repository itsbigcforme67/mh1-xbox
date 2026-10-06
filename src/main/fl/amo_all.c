/* AMO model file readers + mesh converters (plAMO*, ConvertModelMeshAMO*). SLPM_654.95 0x00190600-0x00192E00 (one source file in the
 * original: every helper is file-static and defined before its users, which is why this is one translation unit). */
#include "types.h"

void *plMemset(void *, int, int);

static int SearchDirectoryAMO(u8 *f, int type);
static u8 *GetSubDataAMO(u8 *f, int type, int idx);
static void *GetFileHeadAMO(void *f);
u8 *plAMOGetModelHead(void *f, u32 idx);
u8 *plAMOGetMaterialHead(void *f, u32 idx);
u8 *plAMOGetMaterialData(void *f, u32 idx);
u8 *plAMOGetTextureHead(void *f, u32 idx);
u8 *plAMOGetTextureData(void *f, u32 idx);
int plAMOGetMaterialListNum(void *f, u32 idx);
static int ConvertModelMeshAMO_NormalModel(void *, void *);
static int ConvertModelMeshAMO_WeightModel(void *, void *);

/* original bytes: build/raw/plAMOGetModelMatrixlist.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int plAMOGetModelMatrixlist(s16 *out, void *f, u32 idx)
{
#include "plAMOGetModelMatrixlist.inc"
}
#else
int plAMOGetModelMatrixlist(s16 *out, void *f, u32 idx) {
    u8 *m = plAMOGetModelHead(f, idx);
    u8 *d;
    s32 n;
    int *src;
    s32 i;

    if (*(int *)m != 4) {
        return -1;
    }
    d = GetSubDataAMO(m, 0x100000, 0);
    if (d == 0) {
        return -1;
    }
    n = *(int *)(d + 4);
    src = (int *)(d + 0xC);
    out[0] = n;
    for (i = 0; i < n; i++) {
        out[i + 1] = src[i];
    }
    return 0;
}
#endif

int plAMOGetModelAttribute(int *out, void *f, u32 idx) {
    u8 *m = plAMOGetModelHead(f, idx);
    u8 *d;
    int *src;

    if (*(int *)m != 4) {
        return -1;
    }
    d = GetSubDataAMO(m, 0xF0000, 0);
    if (d == 0) {
        return -1;
    }
    src = (int *)(d + 0xC);
    out[0] = *(int *)(d + 0xC);
    if ((out[0] & 0xFFFF0000) != 0x10000) {
        return -2;
    }
    out[0] = src[0];
    out[1] = src[1];
    out[2] = src[2];
    out[3] = src[3];
    out[4] = src[4];
    out[5] = src[5];
    out[6] = src[6];
    out[7] = src[7];
    out[8] = src[8];
    out[9] = src[9];
    out[10] = src[10];
    out[11] = src[11];
    out[12] = src[12];
    out[13] = src[13];
    out[14] = src[14];
    out[15] = src[15];
    out[16] = src[16];
    out[17] = src[17];
    return 0;
}

int plAMOGetModelNum(void *f) {
    u8 *h = GetFileHeadAMO(f);
    u8 *d;

    if (SearchDirectoryAMO(h, 2) != 0) {
        return 0;
    }
    d = GetSubDataAMO(h, 2, 0);
    if (d == 0) {
        return 0;
    }
    return *(int *)(d + 4);
}

u8 *plAMOGetModelHead(void *f, u32 idx) {
    u8 *h = GetFileHeadAMO(f);
    u8 *d;

    if (SearchDirectoryAMO(h, 2) != 0) {
        return 0;
    }
    d = GetSubDataAMO(h, 2, 0);
    if (d == 0) {
        return 0;
    }
    if (*(u32 *)(d + 4) < idx) {
        return 0;
    }
    d = GetSubDataAMO(d, -1, idx);
    if (d == 0) {
        return 0;
    }
    return d;
}

int plAMOGetMaterialNum(void *f) {
    u8 *h = GetFileHeadAMO(f);
    u8 *d;

    if (SearchDirectoryAMO(h, 9) != 0) {
        return 0;
    }
    d = GetSubDataAMO(h, 9, 0);
    if (d == 0) {
        return 0;
    }
    return *(int *)(d + 4);
}

u8 *plAMOGetMaterialHead(void *f, u32 idx) {
    u8 *h = GetFileHeadAMO(f);
    u8 *d;

    if (SearchDirectoryAMO(h, 9) != 0) {
        return 0;
    }
    d = GetSubDataAMO(h, 9, 0);
    if (d == 0) {
        return 0;
    }
    if (*(u32 *)(d + 4) < idx) {
        return 0;
    }
    d = GetSubDataAMO(d, -1, idx);
    if (d == 0) {
        return 0;
    }
    return d;
}

u8 *plAMOGetMaterialData(void *f, u32 idx) {
    u8 *p = plAMOGetMaterialHead(f, idx);

    if (p == 0) {
        return 0;
    }
    return p + 0xC;
}

/* Converts material idx of the file into the 0x4C byte render material at out. */
int plAMOSetMaterialData(void *f, u32 idx, f32 *out) {
    f32 *m = (f32 *)plAMOGetMaterialData(f, idx);
    u8 *t;

    plMemset(out, 0, 0x4C);
    if (*(int *)((u8 *)m + 0x34) != 0) {
        t = plAMOGetTextureData(f, *(u32 *)((u8 *)m + 0x100));
        *(int *)((u8 *)out + 0x44) = *(int *)t;
    } else {
        *(int *)((u8 *)out + 0x44) = 0;
    }
    out[1] = m[4];
    out[2] = m[5];
    out[3] = m[6];
    out[4] = m[7];
    out[9] = m[0];
    out[10] = m[1];
    out[11] = m[2];
    out[12] = m[3];
    out[5] = m[8];
    out[6] = m[9];
    out[7] = m[10];
    out[8] = m[11];
    *(int *)((u8 *)out + 0x34) = 0;
    *(int *)((u8 *)out + 0x38) = 0;
    *(int *)((u8 *)out + 0x3C) = 0;
    *(int *)((u8 *)out + 0x40) = 0;
    *(int *)((u8 *)out + 0x48) = (int)m[12];
    return 1;
}

u8 *plAMOGetTextureHead(void *f, u32 idx) {
    u8 *h = GetFileHeadAMO(f);
    u8 *d;

    if (SearchDirectoryAMO(h, 0xA) != 0) {
        return 0;
    }
    d = GetSubDataAMO(h, 0xA, 0);
    if (d == 0) {
        return 0;
    }
    if (*(u32 *)(d + 4) < idx) {
        return 0;
    }
    d = GetSubDataAMO(d, -1, idx);
    if (d == 0) {
        return 0;
    }
    return d;
}

u8 *plAMOGetTextureData(void *f, u32 idx) {
    u8 *p = plAMOGetTextureHead(f, idx);

    if (p == 0) {
        return 0;
    }
    return p + 0xC;
}

int plAMOGetMaterialListNum(void *f, u32 idx) {
    u8 *m = plAMOGetModelHead(f, idx);
    u8 *d;

    if (m == 0) {
        return 0;
    }
    d = GetSubDataAMO(m, 0x50000, 0);
    if (d == 0) {
        return 0;
    }
    return *(int *)(d + 4);
}

int plAMOGetMaterialList(void *f, u32 idx, int *out) {
    u8 *m = plAMOGetModelHead(f, idx);
    u8 *d;
    u32 i;
    u32 n;
    int *src;

    if (m == 0) {
        return 0;
    }
    d = GetSubDataAMO(m, 0x50000, 0);
    if (d == 0) {
        return 0;
    }
    n = *(u32 *)(d + 4);
    src = (int *)(d + 0xC);
    for (i = 0; i < n; i++) {
        out[i] = *src;
        src++;
    }
    return 1;
}

static void *GetFileHeadAMO(void *f) {
    return f;
}

static int SearchDirectoryAMO(u8 *f, int type) {
    u32 i;
    u32 n = *(u32 *)(f + 4);

    f += 0xC;
    for (i = 0; i < n; i++) {
        if (*(int *)f == type) {
            return 0;
        }
        f += *(int *)(f + 8);
    }
    return -1;
}

/* idx-th (0 based) entry of the given type below f, or with type -1 simply the idx-th entry. */
static u8 *GetSubDataAMO(u8 *f, int type, int idx) {
    int i;
    int n;

    if (type != -1) {
        n = *(int *)(f + 4);
        f += 0xC;
        for (i = 0; i < n; i++) {
            if (*(int *)f == type) {
                if (idx == 0) {
                    return f;
                }
                idx--;
            }
            f += *(int *)(f + 8);
        }
    } else if ((u32)idx < *(u32 *)(f + 4)) {
        f += 0xC;
        for (i = 0; i < idx; i++) {
            f += *(int *)(f + 8);
        }
        return f;
    }
    return 0;
}

static int GetIndexListNumAMOModelMesh(u8 *mesh) {
    u8 *d = GetSubDataAMO(mesh, 5, 0);

    if (d == 0) {
        return -1;
    }
    return *(int *)(d + 4);
}

static int GetIndexListDescAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 5, 0);
    int i;

    if (d == 0) {
        return -1;
    }
    if (*(u32 *)(d + 4) <= (u32)idx) {
        return -1;
    }
    d += 0xC;
    for (i = 0; i < idx; i++) {
        d += *(int *)(d + 8);
    }
    return *(int *)d;
}

static int GetPrimitiveNumAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 5, 0);
    int i;

    if (d == 0) {
        return -1;
    }
    if (*(u32 *)(d + 4) <= (u32)idx) {
        return -1;
    }
    d += 0xC;
    for (i = 0; i < idx; i++) {
        d += *(int *)(d + 8);
    }
    return *(int *)(d + 4);
}

static int GetAllPrimitiveNumAMOModelMesh(u8 *mesh) {
    u32 n;
    u32 i;
    int sum;
    int r;
    sum = 0;
    n = r = GetIndexListNumAMOModelMesh(mesh);
    if (r == -1) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        sum += GetPrimitiveNumAMOModelMesh(mesh, i);
    }
    return sum;
}

static int GetVertexNumAMOModelMesh(u8 *mesh) {
    u8 *d = GetSubDataAMO(mesh, 0x70000, 0);

    if (d == 0) {
        return -1;
    }
    return *(int *)(d + 4);
}

static int GetMaterialIndexAMOModelMesh(u8 *mesh, int list, int prim) {
    int sum = 0;
    u8 *d;
    u8 *q;
    int i;

    if (GetIndexListNumAMOModelMesh(mesh) < list) {
        return -1;
    }
    d = GetSubDataAMO(mesh, 5, 0);
    if (d == 0) {
        return -1;
    }
    q = d + 0xC;
    for (i = 0; i < list; i++) {
        sum += *(int *)(q + 4);
        q += *(int *)(q + 8);
    }
    sum += prim;
    d = GetSubDataAMO(mesh, 0x60000, 0);
    if (d == 0) {
        return -1;
    }
    return ((int *)(d + 0xC))[sum];
}

/* original bytes: build/raw/GetPrimVertexNumAMOModelMesh.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm int GetPrimVertexNumAMOModelMesh(u8 *mesh, int list, int prim)
{
#include "GetPrimVertexNumAMOModelMesh.inc"
}
#else
static int GetPrimVertexNumAMOModelMesh(u8 *mesh, int list, int prim) {
    u8 *d;
    u8 *q;
    u8 *p;
    int i;
    u32 n;

    if (GetIndexListNumAMOModelMesh(mesh) < list) {
        return -1;
    }
    d = GetSubDataAMO(mesh, 5, 0);
    if (d == 0) {
        return -1;
    }
    q = d + 0xC;
    for (i = 0; i < list; i++) {
        q += *(int *)(q + 8);
    }
    p = q + 0xC;
    for (i = 0; i <= prim; i++) {
        n = *(u32 *)p & 0x7FFFFFFF;
        p += n * 4 + 4;
    }
    return n;
}
#endif

/* original bytes: build/raw/GetPrimCullTypeAMOModelMesh.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm int GetPrimCullTypeAMOModelMesh(u8 *mesh, int list, int prim)
{
#include "GetPrimCullTypeAMOModelMesh.inc"
}
#else
static int GetPrimCullTypeAMOModelMesh(u8 *mesh, int list, int prim) {
    u8 *d;
    u8 *q;
    u8 *p;
    int i;
    u32 c;
    u32 h;

    if (GetIndexListNumAMOModelMesh(mesh) < list) {
        return -1;
    }
    d = GetSubDataAMO(mesh, 5, 0);
    if (d == 0) {
        return -1;
    }
    q = d + 0xC;
    for (i = 0; i < list; i++) {
        q += *(int *)(q + 8);
    }
    p = q + 0xC;
    for (i = 0; i <= prim; i++) {
        h = *(u32 *)p;
        c = h & 0x80000000;
        p += (h & 0x7FFFFFFF) * 4 + 4;
    }
    return c != 0;
}
#endif

/* original bytes: build/raw/GetPrimVertexIndexAMOModelMesh.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm int GetPrimVertexIndexAMOModelMesh(u8 *mesh, int list, int prim, int vtx)
{
#include "GetPrimVertexIndexAMOModelMesh.inc"
}
#else
static int GetPrimVertexIndexAMOModelMesh(u8 *mesh, int list, int prim, int vtx) {
    u8 *d;
    u8 *q;
    u8 *p;
    int i;

    if (GetIndexListNumAMOModelMesh(mesh) < list) {
        return -1;
    }
    d = GetSubDataAMO(mesh, 5, 0);
    if (d == 0) {
        return -1;
    }
    q = d + 0xC;
    for (i = 0; i < list; i++) {
        q += *(int *)(q + 8);
    }
    p = q + 0xC;
    for (i = 0; i < prim; i++) {
        p += (*(u32 *)p & 0x7FFFFFFF) * 4 + 4;
    }
    if (vtx >= (*(u32 *)p & 0x7FFFFFFF)) {
        return -1;
    }
    return *(int *)(p + vtx * 4 + 4);
}
#endif

static u8 *GetVertexAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0x70000, 0);

    if (d == 0) {
        return 0;
    }
    d += 0xC;
    return d + idx * 12;
}

static u8 *GetNormalAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0x80000, 0);

    if (d == 0) {
        return 0;
    }
    d += 0xC;
    return d + idx * 12;
}

static u8 *GetStAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0xA0000, 0);

    if (d == 0) {
        return 0;
    }
    d += 0xC;
    return d + idx * 8;
}

static u8 *GetVertexColorAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0xB0000, 0);

    if (d == 0) {
        return 0;
    }
    d += 0xC;
    return d + idx * 16;
}

static int GetWeightNumAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0xC0000, 0);
    u8 *q;
    int i;
    int n;

    if (d == 0) {
        return -1;
    }
    q = d + 0xC;
    for (i = 0; i < idx; i++) {
        n = *(int *)q;
        q += 4;
        q += n * 8;
    }
    return *(int *)q;
}

/* original bytes: build/raw/GetWeightAMOModelMesh.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm u8 * GetWeightAMOModelMesh(u8 *mesh, int idx, int w)
{
#include "GetWeightAMOModelMesh.inc"
}
#else
static u8 *GetWeightAMOModelMesh(u8 *mesh, int idx, int w) {
    u8 *d = GetSubDataAMO(mesh, 0xC0000, 0);
    int i;
    int n;

    if (d == 0) {
        return 0;
    }
    d += 0xC;
    for (i = 0; i < idx; i++) {
        n = *(int *)d;
        d += 4;
        d += n * 8;
    }
    if (*(int *)d <= w) {
        return 0;
    }
    d += 4;
    return d + w * 8;
}
#endif

int plAMOCreateClayFromImage(void *clay, void *f, u32 idx) {
    u8 *m;
    u8 *d;

    GetFileHeadAMO(f);
    d = GetSubDataAMO(f, 2, 0);
    if (d == 0) {
        return 0;
    }
    if (*(u32 *)(d + 4) <= idx) {
        return 0;
    }
    m = d = plAMOGetModelHead(f, idx);
    if (d == 0) {
        return 0;
    }
    switch (*(int *)m) {
    case 3:
    case 6:
    case 7:
    case 8:
        return 0;
    case 4:
        *(s16 *)((u8 *)clay + 0x20) = plAMOGetMaterialListNum(f, idx);
        return ConvertModelMeshAMO(clay, m);
    default:
        return 0;
    }
}

static int ConvertModelMeshAMO(void *clay, void *mesh) {
    if (SearchDirectoryAMO(mesh, 0xC0000) != 0) {
        return ConvertModelMeshAMO_NormalModel(clay, mesh);
    }
    return ConvertModelMeshAMO_WeightModel(clay, mesh);
}

static int CheckMaterialChangeAMO(u8 *mesh) {
    int i;
    int j;
    int n;
    int np;
    int first;

    first = GetMaterialIndexAMOModelMesh(mesh, 0, 0);
    n = GetIndexListNumAMOModelMesh(mesh);
    for (i = 0; i < n; i++) {
        np = GetPrimitiveNumAMOModelMesh(mesh, i);
        for (j = 0; j < np; j++) {
            if (first != GetMaterialIndexAMOModelMesh(mesh, i, j)) {
                return 1;
            }
        }
    }
    return 0;
}

/* ConvertModelMeshAMO_NormalModel (0x00192090, 1560 bytes): original bytes, no C yet. */
static asm int ConvertModelMeshAMO_NormalModel(void *clay, void *mesh)
{
#include "ConvertModelMeshAMO_NormalModel.inc"
}

/* ConvertModelMeshAMO_WeightModel (0x001926B0, 1816 bytes): original bytes, no C yet. */
static asm int ConvertModelMeshAMO_WeightModel(void *clay, void *mesh)
{
#include "ConvertModelMeshAMO_WeightModel.inc"
}
