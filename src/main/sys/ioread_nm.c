/* Main loop and system init. SLPM_654.95 0x0011F3F0-0x00120234 (f_ioread): ACRMain is the
 * per frame entry (first call initialises the library, memory map and the first task), then
 * reads the pads, runs the task scheduler, draws and flips. InitCommonWork lays out the big
 * work areas (offsets from app_mem_top = 0x8A0000). */
#include "types.h"
#include "sysw.h"

extern int xxx;                 /* 0 first call, 1 running */
extern u16 System_flag, System_flag_old, System_timer;
extern u8 game_w[];
extern u8 RecvMailInfo[0x4D0];
extern s8 option_w[];
extern u8 bg_col_tbl[][3];
extern u8 *flpad_adr;
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
void InitCommonWork(void);
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
    flAdjustScreen(option_w[5], option_w[6]);
}

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

void setBGcolor(int n) {
    u8 *t = bg_col_tbl[n];
    u32 r = *t++;
    u32 g = *t++;
    u32 b = *t;

    flSetRenderState(0x14, (r << 16) | 0xFF000000 | (g << 8) | b);
}

static void ioread_sub(int n) {
    int a = 1;
    u8 b = flpad_adr[n * 0x88];
}

void ioRead(void) {
    int i;

    for (i = 0; i < 2; i++) {
        ioread_sub(i);
    }
    ioRead2();
}

u16 pad_pow_fix(u32 v) {
    if (v < 0x2D) {
        v = 0;
    }
    return v;
}

void NetDisp(void) {
    if (CpInetTcpConnected() != 0 && (System_timer & 0x1F) == 0) {
        CpInetInterfaceProblem(CurDevice->x0, MyEtherInitMode == 1);
    }
}

void DebugProgram(void) {
    /* empty in the retail build */
}

void InitCommonWork(void) {
    int i;

    app_mem_top = 0x8A0000;
    flMemset((void *)0x8A0000, 0, 0x560000);
    work_heap_area = app_mem_top;
    clay_heap_area = app_mem_top + 0x20000;
    material_heap_area = app_mem_top + 0x2D200;
    hierarchy_heap_area = app_mem_top + 0x41200;
    mdlw_heap_area = app_mem_top + 0x109200;
    stage_hit_area_f = app_mem_top + 0x112200;
    stage_hit_area_w = app_mem_top + 0x142200;
    sprite_area = app_mem_top + 0x10D200;
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
