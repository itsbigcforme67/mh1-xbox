/* release_tex_sub + release_texture (SLPM_654.95 0x0011E910-0x0011E9D8): releases `n` texture slots of mem_tex starting at `start`.
   mem_tex entries: low 16 bits texture handle, high 16 bits palette handle. */
#include "types.h"

extern u32 mem_tex[0x159];

int flReleasePaletteHandle(u32);
int flReleaseTextureHandle(u16);

void release_tex_sub(int start, int n) {
    int i;
    u32 *p;

    for (i = 0; i < n; i++) {
        p = &mem_tex[start];
        if (*p != 0) {
            if ((*p & 0xFFFF0000) != 0 && flReleasePaletteHandle((*p & 0xFFFF0000) >> 16) == 1) {
                *p = (u16)*p;
            }
            if (flReleaseTextureHandle(*p) == 1) {
                *p &= 0xFFFF0000;
            }
        }
        start++;
    }
}

void release_texture(int start, int n) {
    release_tex_sub(start, n);
}
