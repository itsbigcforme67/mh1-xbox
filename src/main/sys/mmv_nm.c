/* mmv_nm (not built): MakeMediaVersion (SLPM_654.95 0x0011F670-0x0011F894), written new in this pass from an m2c draft. Builds the media version string
   (year digits + month number) from the compile date/time strings and prints it. Matches except the 5-byte delimiter array: the original builds it
   with `lwc1/lbu` from lit_105 through a pointer (`addiu v1,sp,200`), i.e. an initialised local `char delim[5] = "...."`; reading the same literal as an extern gives
   direct sp offsets instead (3 instructions). The stack layout (8-byte arrays declared in the order delim, yr, mon, day, hh, mm, yr2, mo2, dd2, hh2, mm2 after the two
   0x20 arrays) and the loop variable registers (found by a random order of the declarations) match. */
#include "types.h"
extern char MediaVersion[];
extern char lit_105_003876C0[8];
extern char lit_144_00358360[];
extern char lit_145_00358368[];
extern char lit_146_00358370[];
extern char lit_147_00358378[];
extern char lit_148_00358390[];
extern char lit_149_003583B0[];
extern char lit_150_003583C8[];
extern char lit_151_003583D0[];
extern char media_date_tbl[];
extern char media_time_tbl[];
extern char *month_str_tbl[12];
typedef struct S5 { char c[5]; } S5;
char *strcpy();
char *strtok();
int strcmp();
int strlen();
int sprintf();
int scePrintf();

void MakeMediaVersion(void) {
    char delim[8];
    char date[0x20];
    char time[0x20];
    char yr[8];
    char mon[8];
    char day[8];
    char hh[8];
    char mm[8];
    char yr2[8];
    char mo2[8];
    char dd2[8];
    char hh2[8];
    char mm2[8];
    int i;
    int j;
    char **mp;
    char *p;
    *(f32 *)delim = *(f32 *)lit_105_003876C0;
    delim[4] = ((u8 *)lit_105_003876C0)[4];
    strcpy(date, media_date_tbl);
    strcpy(time, media_time_tbl);
    strcpy(mon, strtok(date, delim));
    strcpy(day, strtok(0, delim));
    strcpy(yr, strtok(0, delim));
    strcpy(yr2, yr + 2);
    mp = month_str_tbl;
    i = 0;
    for (;;) {
        if (strcmp(*mp, mon) == 0) {
            sprintf(mo2, lit_144_00358360, i + 1);
            break;
        }
        i++;
        mp++;
        if (i >= 12) {
            break;
        }
    }
    if (strlen(day) == 1) {
        sprintf(dd2, lit_145_00358368, day);
    } else {
        sprintf(dd2, lit_146_00358370, day);
    }
    strcpy(hh, strtok(time, delim));
    strcpy(mm, strtok(0, delim));
    sprintf(hh2, lit_146_00358370, hh);
    sprintf(mm2, lit_146_00358370, mm);
    sprintf(MediaVersion, lit_147_00358378, yr2, mo2, dd2, hh2, mm2);
    scePrintf(lit_148_00358390);
    scePrintf(lit_149_003583B0);
    p = MediaVersion;
    j = 0;
    do {
        scePrintf(lit_150_003583C8, *p);
        j++;
        p++;
    } while (j < 10);
    scePrintf(lit_151_003583D0);
    scePrintf(lit_148_00358390);
}
