/* fprint01 - printf-style text helpers (SLPM_654.95 0x00161970-0x00161C18): font_print, font_print2, font_print_ex (use include/va.h). Whole file in fprint_nm.c. */
/* fprint_nm - SLPM_654.95 0x00161970-0x00161C18 (font_print.s): printf-style text helpers over flfntPrintf. The
   buffers are 0x400-byte statics (tmpstr_*), "%s" is lit_377_0035BB08. Working file. */
#include "types.h"
#include "va.h"
extern s32 font_reset_flag;
extern char lit_377_0035BB08[];
extern char tmpstr_369[];
extern char tmpstr_388[];
extern char tmpstr_443[];
void flfntLocate();
void flfntPrintf(char *, ...);
void font_set_palette();
int vsprintf(char *, const char *, va_list);
void font_print(char *fmt, ...) {
    va_list ap;

    if (font_reset_flag == 0) {
        va_start(ap, fmt);
        vsprintf(tmpstr_369, fmt, ap);
        flfntPrintf(lit_377_0035BB08, tmpstr_369);
    }
}
/* Like font_print but only the first `limit` characters (double-byte characters count once); 0 prints
   nothing, -1 everything. */
void font_print2(s16 limit, char *fmt, ...) {
    va_list ap;
    s16 i;
    u8 c;

    if (font_reset_flag == 0 && limit != 0) {
        va_start(ap, fmt);
        vsprintf(tmpstr_388, fmt, ap);
        if (limit != -1) {
            i = 0;
            if (tmpstr_388[0] != 0) {
                do {
                    c = tmpstr_388[i];
                    if ((c >= 0x80 && c <= 0x9F) || (c >= 0xE0 && c < 0x100)) {
                        i++;
                    }
                    i++;
                    if (i >= 0x400) {
                        tmpstr_388[0x3FF] = 0;
                        break;
                    }
                    limit--;
                    if (limit == 0) {
                        tmpstr_388[i] = 0;
                        break;
                    }
                } while (tmpstr_388[i] != 0);
            }
        }
        flfntPrintf(lit_377_0035BB08, tmpstr_388);
    }
}
void font_print_ex(int x, int y, int pal, char *fmt, ...) {
    va_list ap;

    if (font_reset_flag == 0) {
        flfntLocate(x, y);
        font_set_palette(pal);
        va_start(ap, fmt);
        vsprintf(tmpstr_443, fmt, ap);
        flfntPrintf(lit_377_0035BB08, tmpstr_443);
    }
}
