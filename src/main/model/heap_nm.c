/* Slot heaps for model data: used-flag bytes per entry (material 0x400, hierarchy 0x800,
 * clay 0x180, mdlw 0x80), find a free run (get_start_*), mark/clear a run (set_used_*,
 * clr_used_*) and entry address (get_*_ptr). SLPM_654.95: f_get 0x00123500-0x00123A60 and
 * set_used_clay.s 0x00123A60... */
#include "types.h"

extern u8 material_heap[0x400];
extern u8 hierarchy_heap[0x800];
extern u8 clay_heap[0x180];
extern u8 mdlw_heap[0x80];
extern int material_heap_area;     /* gp-relative, holds an address */
extern int hierarchy_heap_area;

#define HEAP_FUNCS(NAME, HEAP, SIZE)                                  \
int get_start_##NAME(int n) {                                         \
    int i = 0;                                                        \
    int j;                                                            \
    int sum;                                                          \
                                                                      \
    if (n == 0) {                                                     \
        return 0;                                                     \
    }                                                                 \
    for (;;) {                                                        \
        if (i < SIZE) {                                               \
            u8 *p = &HEAP[i];                                         \
            do {                                                      \
                if (*p == 0) {                                        \
                    break;                                            \
                }                                                     \
                i++;                                                  \
                p++;                                                  \
            } while (i < SIZE);                                       \
        }                                                             \
        if (i >= SIZE) {                                              \
            return -1;                                                \
        }                                                             \
        if (i + n > SIZE) {                                           \
            return -1;                                                \
        }                                                             \
        sum = 0;                                                      \
        j = 0;                                                        \
        if (j < n) {                                                  \
            do {                                                      \
                sum += HEAP[i + j];                                   \
                j++;                                                  \
            } while (j < n);                                          \
        }                                                             \
        if (sum == 0) {                                               \
            return i;                                                 \
        }                                                             \
        i += n;                                                       \
    }                                                                 \
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

HEAP_FUNCS(material, material_heap, 0x400)
SET_CLR(material, material_heap)
HEAP_FUNCS(hierarchy, hierarchy_heap, 0x800)

void set_used_hierarchy(int start, int n) {
    int i;

    if (n != 0) {
        for (i = 0; i < n; i++) {
            hierarchy_heap[start + i] = 1;
        }
    }
}

void clr_used_hierarchy(int start, int n) {
    int i;

    if (n != 0) {
        for (i = 0; i < n; i++) {
            hierarchy_heap[start + i] = 0;
        }
    }
}

HEAP_FUNCS(clay, clay_heap, 0x180)
SET_CLR(clay, clay_heap)
HEAP_FUNCS(mdlw, mdlw_heap, 0x80)
SET_CLR(mdlw, mdlw_heap)

typedef struct MATERIAL { char pad[0x50]; } MATERIAL;
typedef struct HIER { char pad[400]; } HIER;

MATERIAL *get_material_ptr(int n) {
    return (MATERIAL *)(material_heap_area + n * sizeof(MATERIAL));
}

HIER *get_hierarchy_ptr(int n) {
    return (HIER *)(hierarchy_heap_area + n * sizeof(HIER));
}
