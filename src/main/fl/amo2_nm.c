/* AMO mesh readers. SLPM_654.95 0x00190FC0-0x00192090 (f_convertmodelmeshamo, second part).
 * A model entry (type 4) holds sub lists: 5 index lists (each: size, primitive count, ...),
 * 6 material index table, 7 vertices (12 bytes), 8 normals (12), 0xA st (8), 0xB vertex colours (16),
 * 0xC weights (variable: count, then count 8 byte pairs). */
#include "types.h"

u8 *GetSubDataAMO(u8 *f, int type, int idx);
int SearchDirectoryAMO(u8 *f, int type);
void *GetFileHeadAMO(void *f);
u8 *plAMOGetModelHead(void *f, u32 idx);
int plAMOGetMaterialListNum(void *f, u32 idx);
int ConvertModelMeshAMO_NormalModel(void *, void *);
int ConvertModelMeshAMO(void *clay, void *mesh);
int ConvertModelMeshAMO_WeightModel(void *, void *);

int GetIndexListNumAMOModelMesh(u8 *mesh) {
    u8 *d = GetSubDataAMO(mesh, 5, 0);

    if (d == 0) {
        return -1;
    }
    return *(int *)(d + 4);
}

int GetIndexListDescAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 5, 0);
    int i;

    if (d == 0) {
        return -1;
    }
    if (!((u32)idx < *(u32 *)(d + 4))) {
        return -1;
    }
    d += 0xC;
    for (i = 0; i < idx; i++) {
        d += *(int *)(d + 8);
    }
    return *(int *)d;
}

int GetPrimitiveNumAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 5, 0);
    int i;

    if (d == 0) {
        return -1;
    }
    if (!((u32)idx < *(u32 *)(d + 4))) {
        return -1;
    }
    d += 0xC;
    for (i = 0; i < idx; i++) {
        d += *(int *)(d + 8);
    }
    return *(int *)(d + 4);
}

int GetAllPrimitiveNumAMOModelMesh(u8 *mesh) {
    u32 i;
    u32 n;
    int sum = 0;

    n = GetIndexListNumAMOModelMesh(mesh);
    if (n == -1) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        sum += GetPrimitiveNumAMOModelMesh(mesh, i);
    }
    return sum;
}

int GetVertexNumAMOModelMesh(u8 *mesh) {
    u8 *d = GetSubDataAMO(mesh, 0x70000, 0);

    if (d == 0) {
        return -1;
    }
    return *(int *)(d + 4);
}

int GetMaterialIndexAMOModelMesh(u8 *mesh, int list, int prim) {
    int sum = 0;
    u8 *d;
    int i;

    if (GetIndexListNumAMOModelMesh(mesh) < list) {
        return -1;
    }
    d = GetSubDataAMO(mesh, 5, 0);
    if (d == 0) {
        return -1;
    }
    d += 0xC;
    for (i = 0; i < list; i++) {
        sum += *(int *)(d + 4);
        d += *(int *)(d + 8);
    }
    sum += prim;
    d = GetSubDataAMO(mesh, 6, 0);
    if (d == 0) {
        return -1;
    }
    return *(int *)(d + sum * 4 + 0xC);
}

u8 *GetVertexAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 7, 0);

    if (d != 0) {
        return d + idx * 12 + 0xC;
    }
    return 0;
}

u8 *GetNormalAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 8, 0);

    if (d != 0) {
        return d + idx * 12 + 0xC;
    }
    return 0;
}

u8 *GetStAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0xA, 0);

    if (d != 0) {
        return d + idx * 8 + 0xC;
    }
    return 0;
}

u8 *GetVertexColorAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0xB, 0);

    if (d != 0) {
        return d + idx * 16 + 0xC;
    }
    return 0;
}

int GetWeightNumAMOModelMesh(u8 *mesh, int idx) {
    u8 *d = GetSubDataAMO(mesh, 0xC, 0);
    int i;

    if (d == 0) {
        return -1;
    }
    d += 0xC;
    for (i = 0; i < idx; i++) {
        d += *(int *)d * 8 + 4;
    }
    return *(int *)d;
}

u8 *GetWeightAMOModelMesh(u8 *mesh, int idx, int w) {
    u8 *d = GetSubDataAMO(mesh, 0xC, 0);
    int i;

    if (d == 0) {
        return 0;
    }
    d += 0xC;
    for (i = 0; i < idx; i++) {
        d += *(int *)d * 8 + 4;
    }
    if (w < *(int *)d) {
        return d + 4 + w * 8;
    }
    return 0;
}

int plAMOCreateClayFromImage(void *clay, void *f, u32 idx) {
    u8 *m;
    u8 *d;

    GetFileHeadAMO(f);
    d = GetSubDataAMO(f, 2, 0);
    if (d == 0) {
        return 0;
    }
    if (!(idx < *(u32 *)(d + 4))) {
        return 0;
    }
    m = plAMOGetModelHead(f, idx);
    if (m == 0) {
        return 0;
    }
    switch (*(int *)m) {
    case 4:
        *(s16 *)((u8 *)clay + 0x20) = plAMOGetMaterialListNum(f, idx);
        return ConvertModelMeshAMO(clay, m);
    case 8:
    case 7:
    case 6:
    case 3:
        return 0;
    default:
        return 0;
    }
}

int ConvertModelMeshAMO(void *clay, void *mesh) {
    if (SearchDirectoryAMO(mesh, 0xC0000) != 0) {
        return ConvertModelMeshAMO_NormalModel(clay, mesh);
    }
    return ConvertModelMeshAMO_WeightModel(clay, mesh);
}

int CheckMaterialChangeAMO(u8 *mesh) {
    int first = GetMaterialIndexAMOModelMesh(mesh, 0, 0);
    int n = GetIndexListNumAMOModelMesh(mesh);
    int i;
    int j;
    int np;

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
