/* ncm (whole file) 0x002702D0-0x002711F0: network connection messages (ncm). Strings
 * are drawn with the flfnt font; "%s"/"%1"/"%2"/"%y" inside a message are
 * replaced by the slot name, slot 1/2 name and a number from net_common_w. */
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

void ncm_str_disp_sub(char *s, int x, int y, int col, int col2, int size) {
    char buf[0x80];
    char *p;
    int w;
    int len;
    int half = size / 2;
    int guard = 0;
    int quarter = half * 4;

    for (;;) {
        guard++;
        if (!(guard < 0xC8)) {
            return;
        }
        p = strchr(s, '%');
        if (p == 0) {
            break;
        }
        memset(buf, 0, 0x80);
        w = 0;
        while (s < p) {
            len = ncm_char_length((u8 *)s) & 0xFF;
            if (len == 0) {
                break;
            }
            strncat(buf, s, len);
            s += len;
            if (len == 2) {
                w += size;
            } else {
                w += half;
            }
        }
        flfntLocate(x, y);
        flfntSetPalette(col);
        flfntPrintf(buf);
        x += w;
        flfntLocate(x, y);
        flfntSetPalette(col2);
        switch (p[1]) {
        case 0x73:
            flfntPrintf(ncm_slot_str_tbl[*(s8 *)(net_common_w + 0x79) + 1]);
            x += size;
            break;
        case 0x79:
            flfntPrintf(lit_445_00373450, *(s16 *)(net_common_w + 0x7A));
            x += quarter;
            break;
        case 0x31:
            flfntPrintf(ncm_slot_str_tbl[1]);
            x += size;
            break;
        case 0x32:
            flfntPrintf(ncm_slot_str_tbl[2]);
            x += size;
            break;
        default:
            flfntSetPalette(col);
            flfntPrintf(lit_446_00373458);
            x += half;
            p--;
            break;
        }
        s = p + 2;
    }
    flfntLocate(x, y);
    flfntSetPalette(col);
    flfntPrintf(s);
}

void Ncm_mssage_disp(int no) {
    s16 x;
    s16 y;
    s16 size;
    s16 col;
    s16 col2;
    s16 step;
    s16 x0;
    char **lines;

    if (no == 0 || no >= 0x7A) {
        return;
    }
    x = ncm_pos_col_tbl[no][0];
    y = ncm_pos_col_tbl[no][1];
    size = ncm_pos_col_tbl[no][2];
    col = ncm_pos_col_tbl[no][3];
    col2 = ncm_pos_col_tbl[no][4];
    step = ncm_pos_col_tbl[no][5];
    lines = net_con_msg_tbl[no];
    flfntSetSize(size & 0xFF, size & 0xFF);
    if (*lines != 0) {
        x0 = x;
        do {
            if (x0 == -1) {
                x = ncm_center_x(*lines, size);
            }
            ncm_str_disp_sub(*lines, x, y, col, col2, size);
            lines++;
            y += step;
        } while (*lines != 0);
    }
}

void Ncm_mssage_disp_option(int no) {
    s16 y;
    s16 col2;
    s16 size;
    s16 step;
    char **lines;

    if (no == 0 || no >= 0x7A) {
        return;
    }
    y = ncm_pos_col_tbl[no][1];
    size = ncm_pos_col_tbl[no][2];
    col2 = ncm_pos_col_tbl[no][4];
    step = ncm_pos_col_tbl[no][5];
    lines = net_con_msg_tbl[no];
    flfntSetSize(size & 0xFF, size & 0xFF);
    while (*lines != 0) {
        lines++;
        y += step;
    }
    y += (s16)(step * 2);
    ncm_str_disp_sub(ncm_br_ex_mess02, ncm_center_x(ncm_br_ex_mess02, size), y - step, col2, col2, size);
}

void Ncm_err_mssage_disp(int no) {
    s16 x;
    s16 y;
    s16 size;
    s16 col;
    s16 col2;
    s16 step;
    s16 x0;
    char **lines;

    if (no == 0 || no >= 0xF) {
        return;
    }
    x = ncm_err_pos_col_tbl[no][0];
    y = ncm_err_pos_col_tbl[no][1];
    size = ncm_err_pos_col_tbl[no][2];
    col = ncm_err_pos_col_tbl[no][3];
    col2 = ncm_err_pos_col_tbl[no][4];
    step = ncm_err_pos_col_tbl[no][5];
    lines = ms_err_str_tbl[no];
    flfntSetSize(size & 0xFF, size & 0xFF);
    if (*lines != 0) {
        x0 = x;
        do {
            if (x0 == -1) {
                x = ncm_center_x(*lines, size);
            }
            ncm_str_disp_sub(*lines, x, y, col, col2, size);
            lines++;
            y += step;
        } while (*lines != 0);
    }
}

void Ncm_br_mc_mssage_disp(int no) {
    s16 x;
    s16 y;
    s16 col;
    s16 col2;
    s16 size;
    s16 step;
    int x0;
    char **lines;

    if (no == 0 || no >= 7) {
        return;
    }
    x = ncm_br_mc_pos_col_tbl[no][0];
    y = ncm_br_mc_pos_col_tbl[no][1];
    size = ncm_br_mc_pos_col_tbl[no][2];
    col = ncm_br_mc_pos_col_tbl[no][3];
    col2 = ncm_br_mc_pos_col_tbl[no][4];
    step = ncm_br_mc_pos_col_tbl[no][5];
    lines = ncm_br_mc_mess_tbl[no];
    flfntSetSize(size & 0xFF, size & 0xFF);
    if (*lines != 0) {
        x0 = x;
        do {
            if (x0 == -1) {
                x = ncm_center_x(*lines, size);
            }
            ncm_str_disp_sub(*lines, x, y - step, col, col2, size);
            lines++;
            y += step;
        } while (*lines != 0);
    }
    switch (no) {
    case 4:
    case 6:
        {
            s16 sz = size;
            s16 st = step;
            y += (s16)(st * 2);
            ncm_str_disp_sub(ncm_br_ex_mess00, ncm_center_x(ncm_br_ex_mess00, sz), y - st, col2, col2, sz);
        }
        break;
    case 5:
        {
            s16 sz = size;
            s16 st = step;
            y += (s16)(st * 2);
            ncm_str_disp_sub(ncm_br_ex_mess01, ncm_center_x(ncm_br_ex_mess01, sz), y - st, col2, col2, sz);
        }
        break;
    }
    if (*(s8 *)(net_common_w + 0x8C) != 0) {
        *(s8 *)(net_common_w + 0x8C) = 0;
        ncm_str_disp_sub((char *)(net_common_w + 0x8D), (640 - (s16)strlen((char *)(net_common_w + 0x8D)) * 11) / 2, 0x14C, col2, col2, 0x16);
    }
}

void Ncm_menu_disp(u32 no) {
    int i;
    s16 x;
    s16 y;
    s16 size;
    s16 col;
    char *str;

    switch (no) {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        for (i = 0; i < menu_num_tbl[no]; i++) {
            x = net_menu_pos_tbl[no][i * 4];
            y = net_menu_pos_tbl[no][i * 4 + 1];
            size = net_menu_pos_tbl[no][i * 4 + 2];
            col = net_menu_pos_tbl[no][i * 4 + 3];
            str = menu_str_tbl[no][i];
            if (x == -1) {
                x = ncm_center_x(str, size);
            }
            flfntSetSize(size & 0xFF, size & 0xFF);
            flfntLocate(x, y);
            flfntSetPalette(col);
            flfntPrintf(str);
        }
        break;
    case 1:
        for (i = 0; i < menu_num_tbl[no]; i++) {
            x = net_menu_pos_tbl[no][i * 4];
            y = net_menu_pos_tbl[no][i * 4 + 1];
            size = net_menu_pos_tbl[no][i * 4 + 2];
            col = net_menu_pos_tbl[no][i * 4 + 3];
            if (i == 1) {
                if (*(s8 *)(CNFile + 0xC80) == 0) {
                    col = 10;
                }
            } else if (i == 2) {
                if (*(s8 *)(CNFile + 0x960) == 0) {
                    col = 10;
                }
            }
            str = menu_str_tbl[no][i];
            if (x == -1) {
                x = ncm_center_x(str, size);
            }
            flfntSetSize(size & 0xFF, size & 0xFF);
            flfntLocate(x, y);
            flfntSetPalette(col);
            flfntPrintf(str);
        }
        break;
    }
}
