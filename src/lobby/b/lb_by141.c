/* lb_by141 - agent B 0x0059A760-0x0059AA4C: Put_page_num (page n / m text and the two page arrows; the arrows take the (s16) of x and y per use, Lb_put_icon_free is declared with s16 coordinates). */
#define Lb_put_icon_free Lb_put_icon_free_hdr   /* lobby_a.h declares it K&R */
#include "lobby_a.h"
#undef Lb_put_icon_free
extern char lit_2418[];
extern char lit_2419[];
extern char lit_2316[];
void Lb_put_icon_free(s16 x, s16 y, int z, int col, int n);
void Lb_put_icon(s16 x, s16 y, int n, int col);
void Put_page_num(int x, int y, int page, s16 pages, int flag) {
    char buf[0x20];
    char buf2[0x20];
    int col;

    col = Lb_get_cursor_col();
    font_set_palette(0);
    if (pages < 10) {
        sprintf(buf, lit_2418, (s16)page + 1, ((char **)lb_num_str)[11], pages);
    } else {
        sprintf(buf, lit_2419, (s16)page + 1, ((char **)lb_num_str)[11], pages);
    }
    han2zen(buf, buf2);
    flfntSetSize(0x12, 0x12);
    flfntLocate(x, y);
    font_set_palette(0);
    if ((flag & 0xFF) == 0) {
        font_print(lit_2316, buf2);
    } else {
        font_print(lit_2316, buf);
    }
    if (pages > 1) {
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        if (flag != 0) {
            if (pages < 10) {
                Lb_put_icon_free((s16)x - 0x18, (s16)y - 1, 0x14, col, 0);
                Lb_put_icon_free((s16)x + 0x28, (s16)y - 1, 0x14, col, 1);
            } else {
                Lb_put_icon_free((s16)x - 0x18, (s16)y - 1, 0x14, col, 0);
                Lb_put_icon_free((s16)x + 0x3A, (s16)y - 1, 0x14, col, 1);
            }
        } else {
            if (pages < 10) {
                Lb_put_icon((s16)x - 0x1A, (s16)y - 3, 0, col);
                Lb_put_icon((s16)x + 0x36, (s16)y - 3, 1, col);
            } else {
                Lb_put_icon((s16)x - 0x1A, (s16)y - 3, 0, col);
                Lb_put_icon((s16)x + 0x5A, (s16)y - 3, 1, col);
            }
        }
    }
}
