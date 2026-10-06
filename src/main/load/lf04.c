/* lf04 - texture list loading 0x0011E9E0-0x0011EAAC: load_texlist (loads a packed texture file and creates up to 50 textures from its link
 * entries into mem_tex[base..]). */
#include "types.h"

extern void *data_load_ptr;
extern u32 mem_tex[];
extern int f_type[2];

int load_file_mdl(void *dst, int id);
int GetLinkFileNum(int *);
void *GetLinkFileAddress(u8 *, int);
void *flCreateTextureFromApx_mem(void *, int);

int load_texlist(int id, int base, int type) {
    int i;
    int n;
    void *p = data_load_ptr;

    load_file_mdl(p, id);
    n = GetLinkFileNum(p);
    if (n > 0x32) {
        n = 0x32;
    }
    for (i = 0; i < n; i++) {
        mem_tex[base++] = (u32)flCreateTextureFromApx_mem(GetLinkFileAddress(p, i), f_type[type]);
    }
    return n;
}
