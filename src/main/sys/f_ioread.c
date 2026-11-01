/* f_ioread - main loop, system init and pad reading (SLPM_654.95 main 0x0011F3F0-0x00120234), the whole original file as one
 * translation unit: ACRMain (per frame entry), MakeMediaVersion (version string from the build date), system_w_set, InitSystemData,
 * setBGcolor, ioRead/ioRead_sub/ioRead2 (PS2 pad -> Psw), NetDisp, InitCommonWork (work area layout from app_mem_top = 0x8A0000).
 * Replaces the runs io01-io04 and ior01 and the near-match files ioread_nm.c and mmv_nm.c. Field names are guesses. */
#include "g_alias.h"
#ifdef MHG_PS2 /* G calls another function here (tools/check.py VERSION=mhg) */
#define strtok strtok_r
#endif
#include "types.h"
#include "sysw.h"

#include "ioread.h"

extern int xxx;                 /* 0 first call, 1 running */
extern u16 System_flag, System_flag_old, System_timer;
extern u8 game_w[];
extern u8 RecvMailInfo[0x4D0];
extern s8 option_w[];
extern u8 bg_col_tbl[][3];
extern FPADI *flpad_adr;
extern int quest_price;
extern u8 view_mat[];
extern u32 mem_tex[0x159];
extern u32 motion_set_handle_tbl[0x1004];
extern int app_mem_top, work_heap_area, clay_heap_area, material_heap_area, hierarchy_heap_area;
extern int mdlw_heap_area, stage_hit_area_f, stage_hit_area_w, sprite_area, mission_area, cam_data_area;
extern int pl_area_top, data_load_ptr, common_motion, stage_model, arc_ptr, pit_model;
extern int render_flag, pub_flag, pub_pause;
extern s16 reload_tex_id, reload_tex_num, reload_tex_total;
typedef struct DEV { int x0; } DEV;
extern DEV *CurDevice;
extern s8 MyEtherInitMode;

void *memset(void *, int, unsigned);
int flInitialize(int, int);
void cnNet_ModuleLoad(void);
int usbkbdm_init(void);
void flPADFixedAnalogSelectSwitch(int);
void flPADGetALL(void);
void kbdExecServer_flag_clear(void);
void kbdExecServer(void);
void render_start(void);
void render_end(void);
void flSetFPS(int);
void flFlip(int);
int softreset_ck(void);
void softreset(void);
void ot_init(void);
void font_stack_reset(void);
void Scheduler(void);
void set_viewproj(int);
void flmatrStore(int, void *);
void trans(void);
void fade_draw(void);
void DebugProgram(void);
int Online_ck(void);
void NetDisp(void);
void Snd_init(void);
void font_set(void);
void Zero_rev_set(void);
void LoadAskData(void);
void InitSystemData(void);
void Overlay_reset(void);
void Load_overlay(int, int);
void SchedulerInit(void);
void Tsk_Execute(void *, int);
void func_533A00(void);
void SoftKeyboard_init(void);
void MakeMediaVersion(void);
void Snd_server(void);
static void InitCommonWork(void);
void setBGcolor(int);
void flSetRenderState(int, u32);
void system_w_init(void);
void option_default_set(void);
void User_data_init(void);
void PatchInitCS(void);
void Default_reibun_set(void);
void system_w_set(void);
void all_reset(void);
void str_outmode(int);
void str_master_vol(int);
void flAdjustScreen(int, int);
void flMemset(void *, int, unsigned);
void init_ran_suu(void);
int CpInetTcpConnected(void);
void CpInetInterfaceProblem(int, int);
void ioRead2(void);
void ioRead(void);
static void ioRead_sub(int);

extern char MediaVersion[];
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
char *strcpy();
char *strtok();
int strcmp();
int strlen();
int sprintf();
int scePrintf();

#if !defined(G_PART)
int ACRMain(void) {
    switch (xxx) {
    case 0:
        while (flInitialize(0x280, 0x1C0) == 0) {
        }
        cnNet_ModuleLoad();
        while (usbkbdm_init() != 0) {
        }
        flPADFixedAnalogSelectSwitch(1);
        InitCommonWork();
        setBGcolor(0);
        Snd_init();
        font_set();
        Zero_rev_set();
        LoadAskData();
        memset(RecvMailInfo, 0, 0x4D0);
        InitSystemData();
        Overlay_reset();
        Load_overlay(1, 1);
        SchedulerInit();
        system_w.x0B = 0;
        Tsk_Execute(func_533A00, 0);
        SoftKeyboard_init();
        MakeMediaVersion();
        xxx++;
    case 1:
        flPADGetALL();
        System_flag_old = System_flag;
        ioRead();
        kbdExecServer_flag_clear();
        kbdExecServer();
        render_start();
        flSetFPS(1);
        if (softreset_ck() != 0) {
            flFlip(0);
            softreset();
            flFlip(0);
            flFlip(0);
            flFlip(0);
            flFlip(0);
            flFlip(0);
            flFlip(0);
            flFlip(0);
        } else {
            if (System_flag == 0 && (system_w.x03 == 0 || game_w[0x23] == 0)) {
                ot_init();
                font_stack_reset();
                Scheduler();
            } else {
                Scheduler();
                set_viewproj(0xFF);
                flmatrStore(0x21, view_mat);
                trans();
            }
            fade_draw();
            DebugProgram();
            if (Online_ck() == 1) {
                NetDisp();
            }
        }
        render_end();
        Snd_server();
        flFlip(0);
        break;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 2
void MakeMediaVersion(void) {
    char delim[5] = " :\t\n";
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
#endif


#if !defined(G_PART)
void system_w_set(void) {
    system_w.x1A = 0;
    system_w.x3C = 0;
    system_w.x22 = 1;
    system_w.x39 = 5;
    system_w.x2F = 0xFF;
    system_w.x38 = 0;
    system_w.x3A = 0;
    system_w.x3B = 0;
    system_w.x07 = 1;
    system_w.x04 = 0;
    system_w.x24 = 0;
    system_w.x14 = 1;
    system_w.x31 = 1;
    system_w.x06 = 0;
    system_w.x05 = 0;
    system_w.outmode = option_w[0];
    system_w.se_vol = option_w[1];
    system_w.bgm_vol = option_w[2];
    str_outmode(system_w.outmode);
    str_master_vol(0);
#ifdef VERSION_MHG
    flAdjustScreen(option_w[7], option_w[8]);   /* G: 5 and 6 are quick continue and window size */
#else
    flAdjustScreen(option_w[5], option_w[6]);
#endif
}
#endif

#if !defined(G_PART) || G_PART == 1
void InitSystemData(void) {
    system_w_init();
    option_default_set();
    User_data_init();
    PatchInitCS();
    Default_reibun_set();
    system_w_set();
    quest_price = 0;
    all_reset();
}
#endif

#if !defined(G_PART) || G_PART == 1
void setBGcolor(int n) {
    u8 *t = bg_col_tbl[n];
    u32 r = *t++;
    u32 g = *t++;
    u32 b = *t;

    flSetRenderState(0x14, (r << 16) | 0xFF000000 | (g << 8) | b);
}
#endif

/* PS2 pad state of pad n (flpad_adr[n], 0x88 bytes). The test has no effect in the retail build. */
#if !defined(G_PART) || G_PART == 1
static void ioread_sub(int n) {
    u8 st = flpad_adr[n].state;

    if (st == 1) {
        return;
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
void ioRead(void) {
    int i;

    for (i = 0; i < 2; i++) {
        ioread_sub(i);
    }
    ioRead2();
}
#endif

#if !defined(G_PART) || G_PART == 1
static u16 pad_pow_fix(u32 v) {
    if (v < 0x2D) {
        v = 0;
    }
    return v;
}
#endif

/* Converts pad n's PS2 buttons and sticks into the game's pad word Psw[n]: buttons (on/old/trg/rel), the two analog sticks
   as directions (an) plus angle (0x10000 = full turn) and strength, and the repeat state. */
#if !defined(G_PART) || G_PART == 1
static void ioRead_sub(int n) {
    u32 b;
    u16 v;
    PSWE *p = &((PSWE *)Psw)[n];

    p->x1E = p->x1C;
    if (flpad_adr[n].state == 1) {
        p->x1C = (flpad_adr[n].x02 & 3) != 0;
    } else {
        p->x1C = 0;
    }
    p->x20 = 0;
    if (flpad_adr[n].state == 1) {
        p->old = p->on;
        p->aold = p->an;
        b = flpad_adr[n].btn;
        v = 0;
        if (b & 0x1) v |= 0x2000;
        if (b & 0x2) v |= 0x1000;
        if (b & 0x4) v |= 0x800;
        if (b & 0x8) v |= 0x400;
        if (b & 0x100) v |= 0x200;
        if (b & 0x200) v |= 0x100;
        if (b & 0x400) v |= 0x80;
        if (b & 0x10) v |= 0x40;
        if (b & 0x20) v |= 0x20;
        if (b & 0x40) v |= 0x10;
        if (b & 0x800) v |= 0x8;
        if (b & 0x80) v |= 0x4;
        if (b & 0x1000) v |= 0x2;
        if (b & 0x2000) v |= 0x1;
        if (b & 0x8000) v |= 0x8000;
        if (b & 0x4000) v |= 0x4000;
        p->on = v;
        v = 0;
        if (flpad_adr[n].mode & 0x30) {
            if (b & 0x10000) v |= 0x2000;
            if (b & 0x20000) v |= 0x1000;
            if (b & 0x40000) v |= 0x800;
            if (b & 0x80000) v |= 0x400;
            p->ang0 = (flpad_adr[n].ang0 << 16) / 360;
            p->pow0 = pad_pow_fix(flpad_adr[n].pow0);
        }
        if (flpad_adr[n].mode & 0x50) {
            if (b & 0x100000) v |= 0x20;
            if (b & 0x200000) v |= 0x10;
            if (b & 0x400000) v |= 0x8;
            if (b & 0x800000) v |= 0x4;
            p->ang1 = (flpad_adr[n].ang1 << 16) / 360;
            p->pow1 = pad_pow_fix(flpad_adr[n].pow1);
        }
        p->an = v;
    } else {
        p->on = 0;
        p->an = 0;
        p->ang1 = 0;
        p->ang0 = 0;
        p->pow1 = 0;
        p->pow0 = 0;
    }
    p->trg = ~p->old & p->on;
    p->rel = p->old & ~p->on;
    p->atrg = ~p->aold & p->an;
    p->arel = p->aold & ~p->an;
    p->rep = 0;
    if (p->old == p->on) {
        p->rcnt++;
        if (p->rcnt > 10) {
            if (p->rcnt > 12) {
                p->rcnt = 10;
                p->rep = p->on;
            }
        }
    } else {
        p->rcnt = 0;
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
void ioRead2(void) {
    int i;
    u16 t;

    for (i = 0; i < 4; i++) {
        ioRead_sub(i);
    }
    if (pub_flag != 0) {
        t = ((PSWE *)Psw)[1].trg;
        if (t & 0x8000) {
            pub_pause ^= 1;
        }
        if (pub_pause != 0) {
            if (t & 0x20) {
                System_flag = 0;
            } else if (((PSWE *)Psw)[1].on & 0x40) {
                System_flag = ((System_flag + 1) & 0xFF) % 4;
            } else {
                System_flag = 0x8000;
            }
        } else {
            pub_pause = 0;
            System_flag = 0;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
void NetDisp(void) {
    if (CpInetTcpConnected() != 0 && (System_timer & 0x1F) == 0) {
        CpInetInterfaceProblem(CurDevice->x0, MyEtherInitMode == 1);
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
void DebugProgram(void) {
    /* empty in the retail build */
}
#endif

#if !defined(G_PART)
static void InitCommonWork(void) {
    int i;

    app_mem_top = 0x8A0000;
    flMemset((void *)0x8A0000, 0, 0x560000);
    work_heap_area = app_mem_top;
    clay_heap_area = app_mem_top + 0x20000;
    material_heap_area = app_mem_top + 0x2D200;
    hierarchy_heap_area = app_mem_top + 0x41200;
    mdlw_heap_area = app_mem_top + 0x109200;
    sprite_area = app_mem_top + 0x10D200;
    stage_hit_area_f = app_mem_top + 0x112200;
    stage_hit_area_w = app_mem_top + 0x142200;
    mission_area = app_mem_top + 0x15A200;
    cam_data_area = app_mem_top + 0x162200;
    pl_area_top = 0xA06200;
    data_load_ptr = 0xB3A200;
    common_motion = 0xA06200;
    stage_model = 0xA06200;
    arc_ptr = 0xCBA200;
    pit_model = 0xA06200;
    for (i = 0; i < 0x159; i++) {
        mem_tex[i] = 0;
    }
    for (i = 0; i < 0x1004; i++) {
        motion_set_handle_tbl[i] = 0;
    }
    init_ran_suu();
    render_flag = 0;
    reload_tex_id = -1;
    reload_tex_num = -1;
    reload_tex_total = 0;
    pub_flag = 0;
    pub_pause = 0;
}
#endif
