/* SLPM_654.95 0x00190FC0-0x00190FF4: GetIndexListNumAMOModelMesh .. GetIndexListNumAMOModelMesh. See amo2_nm.c. */
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
