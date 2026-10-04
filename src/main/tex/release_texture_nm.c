/* NONMATCHING (not built; asm is used). release_tex_sub is ~11 instructions off: the original computes the
 * mem_tex pointer after the n > 0 guard; every loop shape tried either
 * hoists it before the guard or changes register allocation. */
/* Texture handle release. SLPM_654.95 0x0011E910-0x0011E9D8.
 * mem_tex entries: low 16 bits texture handle, high 16 bits palette handle. */
#include "types.h"

extern u32 mem_tex[0x159];

int flReleasePaletteHandle(u32);
int flReleaseTextureHandle(u16);

static void release_tex_sub(int start, int n) {
    int i;

    for (i = 0; i < n; i++) {
        if (mem_tex[start + i] != 0) {
            if ((mem_tex[start + i] & 0xFFFF0000) != 0
                && flReleasePaletteHandle((mem_tex[start + i] & 0xFFFF0000) >> 16) == 1) {
                mem_tex[start + i] = (u16)mem_tex[start + i];
            }
            if (flReleaseTextureHandle(mem_tex[start + i]) == 1) {
                mem_tex[start + i] &= 0xFFFF0000;
            }
        }
    }
}

void release_texture(int start, int n) {
    release_tex_sub(start, n);
}
