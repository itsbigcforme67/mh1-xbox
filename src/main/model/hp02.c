/* model slot heaps: hierarchy (0x00123800-0x00123968). Whole file in heap_nm.c. */
#include "types.h"

extern u8 material_heap[0x400];
extern u8 hierarchy_heap[0x800];
extern u8 clay_heap[0x180];
extern u8 mdlw_heap[0x80];
extern int material_heap_area;     /* gp-relative, holds an address */
extern int hierarchy_heap_area;

typedef struct HIER { char pad[400]; } HIER;

HIER *get_hierarchy_ptr(int n) {
    return (HIER *)(hierarchy_heap_area + n * sizeof(HIER));
}

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

