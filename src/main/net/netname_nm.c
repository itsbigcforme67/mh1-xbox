/* netname_nm - Net_disp_net_name (SLPM_654.95 0x0026C100-0x0026C3D4, main.bin): draw one of the network name
 * strings (hunter id, password stars, status words, counters, version) with its font settings from
 * name_disp_tbl_349. Near-match C, not built. */
#include "types.h"

typedef struct NAMEDISP {
    s16 x;
    s16 y;
    u8 size;
    u8 pad;
    s16 pal;
} NAMEDISP;
extern NAMEDISP name_disp_tbl_349[];
extern u8 net_common_w[];
extern u8 CNFile[];
extern u8 D_6E9714[];
extern char lit_409_00370840[];
extern char lit_410_00370848[];
extern char lit_411_00370850[];
extern char lit_412_00370858[];
extern char lit_413_00370860[];
extern char lit_414_00370868[];
extern char lit_415_00370878[];

void *memset(void *, int, int);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
char *strncpy(char *, const char *, int);
unsigned strlen(const char *);
int sprintf(char *, const char *, ...);
int han2zen();
int flfntSetPalette();
int flfntSetSize();
int flfntLocate();
int flfntPrintf();

void Net_disp_net_name(int kind) {
    char t1[0x10];
    char t2[0x10];
    char t3[0x10];
    char t4[0x10];
    char buf[0x80];
    s16 n;
    int i;

    memset(buf, 0, 0x80);
    switch (kind) {
    case 0:
        sprintf(t1, lit_409_00370840, net_common_w + 0x16);
        han2zen(t1, buf);
        break;
    case 1:
        strcpy(buf, (char *)CNFile + 0x960);
        break;
    case 2:
        n = strlen((char *)CNFile + 0x96B);
        if (n != 0) {
            for (i = 0; i < n; i++) {
                buf[i] = '*';
            }
        }
        break;
    case 3:
        if (*(s8 *)(net_common_w + 0x14) == 0) {
            strcpy(buf, lit_410_00370848);
        } else {
            strcpy(buf, lit_411_00370850);
        }
        break;
    case 4:
        if (*(s8 *)(net_common_w + 0x14) != 0) {
            sprintf(t2, lit_409_00370840, net_common_w + 0x23);
            han2zen(t2, buf);
        }
        break;
    case 5:
        sprintf(t3, lit_412_00370858, *(s16 *)(net_common_w + 4));
        han2zen(t3, buf);
        strcat(buf, lit_413_00370860);
        break;
    case 6:
        sprintf(buf, lit_414_00370868, *(s32 *)(net_common_w + 0x80));
        break;
    case 7:
    case 8:
    case 9:
        memset(t4, 0, 10);
        strncpy(t4, (char *)D_6E9714, 1);
        strcat(t4, lit_415_00370878);
        strncpy(t4 + 2, (char *)D_6E9714 + 1, 3);
        han2zen(t4, buf);
        break;
    }
    flfntSetPalette(name_disp_tbl_349[kind].pal);
    flfntSetSize(name_disp_tbl_349[kind].size, name_disp_tbl_349[kind].size);
    flfntLocate(name_disp_tbl_349[kind].x, name_disp_tbl_349[kind].y);
    flfntPrintf(buf);
}
