/* get_clay_ptr - address of clay (model) work entry n.
 * Matches SLPM_654.95 0x00123A40 (32 bytes) with mwcps2 3.0b52 -O4,p.
 * Needs the int-address + sizeof form; `&clay[n]` on a CLAY * and `n * 140`
 * (signed) both schedule differently. Static in the original. */
typedef struct CLAY {
    char pad[140];
} CLAY;

extern int clay_heap_area;  /* gp-relative, holds an address */

CLAY *get_clay_ptr(int n) {
    return (CLAY *)(clay_heap_area + n * sizeof(CLAY));
}
