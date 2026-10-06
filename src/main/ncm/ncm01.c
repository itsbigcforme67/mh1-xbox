/* ncm01 - 0x002702D0-0x002703E8: ncm string helpers (char length, glyph counts, centring). Whole file in ncm_nm.c. */
#include "types.h"

void flfntSetSize(int, int);
void flfntLocate(s16, s16);
void flfntSetPalette(s16);
void flfntPrintf(char *, ...);
void *memset(void *, int, int);
char *strchr(const char *, int);
char *strncat(char *, const char *, int);

extern u8 net_common_w[];
extern char *ncm_slot_str_tbl[];
extern s16 ncm_pos_col_tbl[][6];
extern s16 ncm_err_pos_col_tbl[][6];
extern char **net_con_msg_tbl[];
extern char **ms_err_str_tbl[];
extern char *ncm_br_ex_mess02;
extern char *ncm_br_ex_mess00;
extern char *ncm_br_ex_mess01;
extern s16 ncm_br_mc_pos_col_tbl[][6];
extern char **ncm_br_mc_mess_tbl[];
extern int menu_num_tbl[];
extern s16 *net_menu_pos_tbl[];
extern char **menu_str_tbl[];
extern u8 CNFile[];
int strlen(const char *);
extern char lit_445_00373450[];
extern char lit_446_00373458[];

u8 ncm_char_length(u8 *s);
void ncm_get_char_size_num(u8 *s, int *wide, int *narrow);
s16 ncm_center_x(char *s, int size);
void ncm_str_disp_sub(char *s, int x, int y, int col, int col2, int size);

u8 ncm_char_length(u8 *s) {
    u8 c = *s;
    u8 r = 0;

    if (c != 0) {
        if (c >= 0x80) {
            r = 2;
        } else {
            r = 1;
        }
    }
    return r;
}

void ncm_get_char_size_num(u8 *s, int *wide, int *narrow) {
    int i;

    *narrow = 0;
    *wide = 0;
    for (i = 0; i < 0x80; i++, s++) {
        if (*s == 0) {
            break;
        }
        if (*s >= 0x80) {
            *wide += 1;
            s++;
        } else {
            *narrow += 1;
        }
    }
}

s16 ncm_center_x(char *s, int size) {
    int wide = 0;
    int narrow = 0;
    int half;

    ncm_get_char_size_num((u8 *)s, &wide, &narrow);
    half = size / 2;
    return (640 - size * wide - narrow * half) / 2;
}
