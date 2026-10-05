/* release_texture (SLPM_654.95 0x0011E9D0-0x0011E9D8): tail call into release_tex_sub (still asm, see release_texture_nm.c). */
#include "types.h"

void release_tex_sub(int start, int n);

void release_texture(int start, int n) {
    release_tex_sub(start, n);
}
