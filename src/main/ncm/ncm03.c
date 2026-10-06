/* ncm03 - 0x00270940-0x00270AC4: Ncm_err_mssage_disp. Whole file in ncm_nm.c. */
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
