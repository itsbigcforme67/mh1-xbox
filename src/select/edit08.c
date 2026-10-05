/* edit08 - select.bin character edit screen 0x005382B0-0x00538538: cmn_mongon_check_filter, cmn_mongon_look, cmn_mongon_look_sub. Whole file in edit_nm.c. */
#include "select.h"


typedef struct { u8 _p[0x24]; s8 name[0x12]; } UDC_SRC;
typedef struct { u8 _p[8]; s8 name[0x12]; } UDC_DST;









int NG_name_chk(u8 *s);







int cmn_mongon_look_sub(s8 *str, s8 *tbl);



typedef struct { s16 x, y, w, h; s32 col; } SPR4;


typedef struct { s16 x, y, w, h; u32 col[4]; } SPR5;


typedef struct { char *a; char *b; } EMSG;
extern EMSG edit_msg[];








typedef struct { u8 _p[0x18]; STASK *work; } TSKH;
void disp_color(u8 *w);
void cont_trans(TSKH *tk);











/* Name filter: copy str to out (n+1 bytes incl. terminator), upper-case it and fold look-alike
   characters (@ -> A, $/5 -> S, </( -> C, !/1 -> I, 2 -> Z, 0 -> O). */
void cmn_mongon_check_filter(s8 *out, s8 *str, int n) {
    int i = 0;
    s8 *src;
    s8 *dst;
    if (0 <= n) {
        do {
            src = str + i;
            dst = out + i;
            *dst = *src;
            if (_ctype_[1 + *src] & 2) {
                *dst -= 0x20;
            }
            if (*dst == 0x40) { *dst = 0x41; }
            if (*dst == 0x24) { *dst = 0x53; }
            if (*dst == 0x35) { *dst = 0x53; }
            if (*dst == 0x3C) { *dst = 0x43; }
            if (*dst == 0x28) { *dst = 0x43; }
            if (*dst == 0x21) { *dst = 0x49; }
            if (*dst == 0x31) { *dst = 0x49; }
            if (*dst == 0x32) { *dst = 0x5A; }
            if (*dst == 0x30) { *dst = 0x4F; }
            i++;
        } while (n >= i);
    }
}

int cmn_mongon_look(s8 *a) {
    s8 c = *a;
    if (c == 0) {
        return 1;
    }
    if (!(_ctype_[1 + c] & 7)) {
        return 1;
    }
    cmn_mongon_look_sub(a, check_mongon);
    return 1;
}

int cmn_mongon_look_sub(s8 *str, s8 *tbl) {
    s8 buf[0x60];
    int len = strlen(str);
    if (*tbl != 0) {
        do {
            int r = cmn_mongon_set(tbl, buf, len);
            if (r != -1 && strncmp(str, buf, r) == 0) {
                return 1;
            }
            tbl += 0x10;
        } while (*tbl != 0);
    }
    return 0;
}
