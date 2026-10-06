/* getsh_nm - SLPM_654.95 0x001234F8-0x00123C70: first-fit search in the model work heaps (one byte per slot,
   0 = free): get_start_material (0x400 slots), hierarchy (0x800), clay (0x180), mdlw (0x80). Working file. */
#include "types.h"

extern u8 material_heap[0x400];
extern u8 hierarchy_heap[0x800];
extern u8 clay_heap[0x180];
extern u8 mdlw_heap[0x80];

int get_start_material(int n) {
    int pos;
    int j;
    int sum;

    pos = 0;
    if (n == 0) {
        return 0;
    }
    for (;;) {
        while (pos < 0x400 && material_heap[pos] != 0) {
            pos++;
        }
        if (pos >= 0x400) {
            return -1;
        }
        if (pos + n > 0x400) {
            return -1;
        }
        sum = 0;
        for (j = 0; j < n; j++) {
            sum += material_heap[pos + j];
        }
        if (sum == 0) {
            return pos;
        }
        pos += n;
    }
}

int get_start_hierarchy(int n) {
    int pos;
    int j;
    int sum;

    pos = 0;
    if (n == 0) {
        return 0;
    }
    for (;;) {
        while (pos < 0x800 && hierarchy_heap[pos] != 0) {
            pos++;
        }
        if (pos >= 0x800) {
            return -1;
        }
        if (pos + n > 0x800) {
            return -1;
        }
        sum = 0;
        for (j = 0; j < n; j++) {
            sum += hierarchy_heap[pos + j];
        }
        if (sum == 0) {
            return pos;
        }
        pos += n;
    }
}

int get_start_clay(int n) {
    int pos;
    int j;
    int sum;

    pos = 0;
    if (n == 0) {
        return 0;
    }
    for (;;) {
        while (pos < 0x180 && clay_heap[pos] != 0) {
            pos++;
        }
        if (pos >= 0x180) {
            return -1;
        }
        if (pos + n > 0x180) {
            return -1;
        }
        sum = 0;
        for (j = 0; j < n; j++) {
            sum += clay_heap[pos + j];
        }
        if (sum == 0) {
            return pos;
        }
        pos += n;
    }
}

int get_start_mdlw(int n) {
    int pos;
    int j;
    int sum;

    pos = 0;
    if (n == 0) {
        return 0;
    }
    for (;;) {
        while (pos < 0x80 && mdlw_heap[pos] != 0) {
            pos++;
        }
        if (pos >= 0x80) {
            return -1;
        }
        if (pos + n > 0x80) {
            return -1;
        }
        sum = 0;
        for (j = 0; j < n; j++) {
            sum += mdlw_heap[pos + j];
        }
        if (sum == 0) {
            return pos;
        }
        pos += n;
    }
}

