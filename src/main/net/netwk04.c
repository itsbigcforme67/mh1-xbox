/* SLPM_654.95 0x0026BCC0-0x0026C0F8: net_joy_ok_ck_each .. Net_demo_camera_set. See netwk_nm.c. */
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
















































int net_joy_ok_ck_each(s8 p) {
    return (Psw[game_w[0x20 + p]].x04 & 0x20) != 0;
}

int net_joy_cancel_ck_each(s8 p) {
    return (Psw[game_w[0x20 + p]].x04 & 0x40) != 0;
}

int net_shot_ok_ck(int kind) {
    int r = 0;

    if ((system_w[0] & 1) && net_joy_ok_ck_each(0) != 0) {
        switch (kind) {
        case 0:
            break;
        case 1:
            se_req(7, 0x13, 0);
            break;
        case 2:
            se_req(7, 9, 0);
            break;
        }
        r = 1;
    }
    if ((system_w[0] & 2) && net_joy_ok_ck_each(1) != 0) {
        switch (kind) {
        case 0:
            break;
        case 1:
            se_req(7, 0x13, 0);
            break;
        case 2:
            se_req(7, 9, 0);
            break;
        }
        r = 1;
    }
    return r;
}

int net_shot_ng_ck(void) {
    int r = 0;

    if ((system_w[0] & 1) && net_joy_cancel_ck_each(0) != 0) {
        se_req(7, 0x14, 0);
        r = 1;
    }
    if ((system_w[0] & 2) && net_joy_cancel_ck_each(1) != 0) {
        se_req(7, 0x14, 0);
        r = 1;
    }
    return r;
}

int net_yesno_operation_move(void) {
    int sw = net_swdata();

    if (sw & 0x3000) {
        if ((sw & 0x2000) && net_common_w.x29 != 0) {
            se_req(7, 0x17, 0);
            net_common_w.x29 = 0;
        }
        if ((sw & 0x1000) && net_common_w.x29 == 0) {
            se_req(7, 0x17, 0);
            net_common_w.x29 = 1;
        }
    }
    if (net_shot_ok_ck(1) != 0) {
        return (net_common_w.x29 != 0) ? -1 : 1;
    }
    return -(net_shot_ng_ck() != 0);
}

void Net_work_move(void) {
    /* empty */
}

void Net_trans_set(void) {
    net_connect_draw();
}

int Net_fade_check(void) {
    switch (Fade_busy_ck() & 0xFF) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 0;
    }
}

void Net_fade_kill(void) {
    fade_reset();
}

int Net_fade_execute(int a, int b, int mode) {
    if ((s16)mode == 1) {
        fade_set(1);
    } else {
        fade_set(2);
    }
    return 1;
}

void Net_all_reset(void) {
    Ncm_spr_kill_all();
    release_texture(0x14D, 8);
}

void Net_work_init_all(void) {
    /* empty */
}

void Net_setBGcolor(void) {
    /* empty */
}

void Net_McWorkInit(void) {
    McActInit();
}

void Net_demo_camera_set(void) {
    /* empty */
}
