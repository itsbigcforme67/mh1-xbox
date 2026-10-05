/* model slot heaps: material (SLPM_654.95 0x001235D0-0x00123728). Whole file in heap_nm.c. */
#include "types.h"

extern u8 material_heap[0x400];
extern u8 hierarchy_heap[0x800];
extern u8 clay_heap[0x180];
extern u8 mdlw_heap[0x80];
extern int material_heap_area;     /* gp-relative, holds an address */
extern int hierarchy_heap_area;

typedef struct MATERIAL { char pad[0x50]; } MATERIAL;

MATERIAL *get_material_ptr(int n) {
    return (MATERIAL *)(material_heap_area + n * sizeof(MATERIAL));
}

#define SET_CLR(NAME, HEAP)                                           \
void set_used_##NAME(int start, int n) {                              \
    int i;                                                            \
                                                                      \
    for (i = 0; i < n; i++) {                                         \
        HEAP[start + i] = 1;                                          \
    }                                                                 \
}                                                                     \
                                                                      \
void clr_used_##NAME(int start, int n) {                              \
    int i;                                                            \
                                                                      \
    for (i = 0; i < n; i++) {                                         \
        HEAP[start + i] = 0;                                          \
    }                                                                 \
}

SET_CLR(material, material_heap)
