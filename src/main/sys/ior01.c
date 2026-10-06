/* ior01 - background colour (SLPM_654.95 0x0011F9E0-0x0011FA24): setBGcolor. Whole file in ioread_nm.c. */
/* Main loop and system init. SLPM_654.95 0x0011F3F0-0x00120234 (f_ioread): ACRMain is the
 * per frame entry (first call initialises the library, memory map and the first task), then
 * reads the pads, runs the task scheduler, draws and flips. InitCommonWork lays out the big
 * work areas (offsets from app_mem_top = 0x8A0000). */
#include "types.h"
#include "sysw.h"
extern int xxx;
/* 0 first call, 1 running */
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
void setBGcolor(int n) {
    u8 *t = bg_col_tbl[n];
    u32 r = *t++;
    u32 g = *t++;
    u32 b = *t;

    flSetRenderState(0x14, (r << 16) | 0xFF000000 | (g << 8) | b);
}
