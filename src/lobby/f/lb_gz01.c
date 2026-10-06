/* lb_gz01 - browser table/tag handlers 0x005E9DA0-0x005E9DE8: BsTextureGet (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 Bs_tex_handle_ptr;
extern char Bs_tex_handle[];

s32 BsTextureGet(int arg0) {
    int temp_v1;

    temp_v1 = (s8)arg0;
    if ((temp_v1 < 0) || (Bs_tex_handle_ptr <= temp_v1)) {
        return -1;
    }
    return *(int *)((u8 *)&Bs_tex_handle + (temp_v1 * 4));
}
