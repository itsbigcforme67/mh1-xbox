/* amo01 - AMO readers 0x00190720-0x00190FB8: plAMOGetModelAttribute, plAMOGetModelNum, plAMOGetModelHead, plAMOGetMaterialNum, plAMOGetMaterialHead, plAMOGetMaterialData, plAMOSetMaterialData, plAMOGetTextureHead, plAMOGetTextureData, plAMOGetMaterialListNum, plAMOGetMaterialList, GetFileHeadAMO, SearchDirectoryAMO, GetSubDataAMO. Whole file in amo_nm.c. */
#include "types.h"

int SearchDirectoryAMO(u8 *f, int type);
u8 *GetSubDataAMO(u8 *f, int type, int idx);
void *GetFileHeadAMO(void *f);
u8 *plAMOGetModelHead(void *f, u32 idx);
u8 *plAMOGetMaterialHead(void *f, u32 idx);
u8 *plAMOGetMaterialData(void *f, u32 idx);
u8 *plAMOGetTextureData(void *f, u32 idx);
void *plMemset(void *, int, int);
















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

void *GetFileHeadAMO(void *f) {
    return f;
}

int SearchDirectoryAMO(u8 *f, int type) {
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
u8 *GetSubDataAMO(u8 *f, int type, int idx) {
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
