/* SLPM_654.95 0x0026C3E0-0x0026C400: Ncm_spr_kill_all .. Ncm_spr_kill_all_ex_BG. See netwk_nm.c. */
#include "types.h"

typedef struct NETCW {
    u8 x00;
    u8 step;            /* 0x01 */
    u8 sub;             /* 0x02 */
    u8 x03;
    s16 timer;          /* 0x04 */
    s16 x06;            /* 0x06 */
    s16 x08;            /* 0x08 */
    s16 x0A;            /* 0x0A */
    u8 x0C;
    u8 x0D;
    u8 x0E;
    u8 pad0F;
    u8 x10;
    u8 x11;             /* 0x11 */
    u8 x12;
    u8 x13;
    u8 pad14;
    u8 x15;
    u8 pad16[0x28 - 0x16];
    u8 x28;
    u8 x29;             /* 0x29 yes/no cursor (0 yes) */
    u8 pad2A[2];
    s32 x2C;            /* 0x2C sprite request mask */
    u8 x30;
    u8 pad31[3];
    u8 x34;
    u8 x35;
    u8 x36;
    u8 x37;
    u8 x38;
    u8 x39;
    u8 x3A;
    u8 x3B;
    u8 x3C;
    u8 x3D;
    u8 x3E;
    u8 x3F;
    u8 pad40[0x94 - 0x40];
} NETCW;
extern NETCW net_common_w;
extern u8 system_w[];
extern u8 game_w[];
typedef struct PSW {
    u16 x00, x02, x04, x06, x08, x0A, x0C, x0E, x10, x12, x14, x16, x18, x1A, x1C, x1E, x20;
} PSW;
extern PSW Psw[];
extern u8 SoftKeyWork[];
extern u8 CNFile[];
extern s8 MMBB_LOGIN;

void *memset(void *, int, int);
char *strcpy(char *, const char *);
int se_req();
int net_connect_draw();
int Fade_busy_ck();
int fade_reset();
int fade_set();
int release_texture();
int McActInit();
int Load_overlay();
int kbdExecServer();
int SoftKeyboard_move();
int SoftKeyboard_pos_set();
int SoftKeyboard_set();
int DispSoftkeyboard();
int load_file_mdl();

void SoftKey_onoff(int on);
void Ncm_spr_kill_all(void);
















































void Ncm_spr_kill_all(void) {
    net_common_w.x2C = 0;
}

void Ncm_spr_kill_all_ex_BG(void) {
    net_common_w.x2C = 1;
}
