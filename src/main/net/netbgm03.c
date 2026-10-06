/* netbgm03 - f_netbgm 0x00267350-0x002673D0: Ncm_mmbb_spr_create. Whole file in netbgm_nm.c. */
#include "types.h"

typedef struct NETCW {
    u8 pad00;
    u8 step;            /* 0x01 */
    s8 x02;             /* 0x02 */
    u8 pad03;
    s16 timer;          /* 0x04 */
    s16 x06;            /* 0x06 */
    s16 x08;            /* 0x08 menu cursor */
    s16 sel;            /* 0x0A */
    u8 pad0C[0x11 - 0x0C];
    u8 x11;             /* 0x11 */
    u8 pad12[0x2C - 0x12];
    s32 x2C;            /* 0x2C */
} NETCW;
extern NETCW net_common_w;
typedef struct MEMTEX {
    u8 pad[0x470];
    s32 x470;
} MEMTEX;
extern MEMTEX mem_tex;
extern void *NET_CON_TEX[];

int se_stat();
int se_req_bgm_vol();
int NetLoadWait();
int Net_all_reset();
int Net_work_init_all();
int Net_put_overlay_data();
int Net_demo_camera_set();
int load_texlist();
int SoftkeyLoad();
int Ncm_mssage_disp_req();
int Ncm_mssage_disp_option_req();
int Ncm_menu_disp_req();
int Ncm_spr_set_diarog_m();
int Ncm_spr_set_diarog_b();
int Ncm_spr_set_diarog_s();
int Ncm_spr_D_MENU_set();
int Ncm_spr_kill();
int Ncm_spr_kill2();
int Net_fade_kill();
int Net_fade_check();
int Net_disp_net_name_req();
int Net_McWorkInt();
int Net_McWorkInit();
int net_set_se_cur();
int net_shot_ok_ck();
int net_shot_ng_ck();
int net_swdata();
int SaveNetFile();
void *memcpy(void *, const void *, int);
void *memset(void *, int, int);
extern u8 system_w[];
extern u8 CNFile[];
extern u8 keep_mmbb_id_0052FA10[];
extern u8 keep_mmbb_password[];
extern void *cw;
extern s8 MMBB_LOGIN;
int Net_setBGcolor();
int Ncm_spr_BG_set();
int Ncm_spr_TITLE_set();
int Ncm_spr_MESS_SET();
int Ncm_spr_MENU_SET();
int Net_fade_execute();
extern u8 D_6DD7E0[];
int Ncm_mmbb_spr_load(void);
int Ncm_mmbb_spr_create(void);
void net_bgm_set(void);







int Ncm_spr_kill_all();
int Ncm_spr_kill_all_ex_BG();



int Ncm_mmbb_spr_create(void) {
    int ret = 0;

    if (net_common_w.x11 == 0) {
        return 2;
    }
    if (NetLoadWait() == 0) {
        load_texlist(NET_CON_TEX[0], 0x14D, 0);
        if (mem_tex.x470 == 0) {
            SoftkeyLoad();
        }
        ret = 1;
    }
    return ret;
}
