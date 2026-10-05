/* SLPM_654.95 0x00159410-0x00159524: Snd_init .. se_req_bgm_vol. See snd_nm.c. */
#include "types.h"
#include "sysw.h"

#define WB(w, o) (*(u8 *)((u8 *)(w) + (o)))
#define WH(w, o) (*(u16 *)((u8 *)(w) + (o)))

extern f32 se_cnfvol_tbl[];
extern s8 emsnd_p[7];
extern s16 Snd_weapon_tbl[];
extern s8 Snd_em_id_file_conv_tbl[];
void flSndJointInit();
void flSndJointSet();
void load_bin_req();
int load_busy_ck();
extern s8 Snd_em_id_conv_tbl[];
extern void *data_load_ptr;
extern u8 *pl_area_top;

void flSndInitialize();
void Adx_init();
void Adx_server();
void flSndRequest();
void flSndChange();
void flSndAllStop();
void flSndStatGet();
void flSndPortStop();
void flSndPackLoad();
void FlushCache();
int load_bin();
void *memset(void *, int, int);
u32 ran_suu();
int Pl_stg_ck();
int Em_stg_ck();
void se_req2();






extern f32 vol_dist_tbl[];
extern s32 *vol_tbl[];
extern f32 rview_mat[];
f32 flSqrt(f32);
f32 flCos(f32);
f32 flFloor(f32);
void flmatInit(f32 *);
void flSetRenderState(int, u32);
void flvecrRotTransPers(f32 *, f32 *);
void se_chg_sub();




extern s8 *Snd_armor_tbl[];















void Snd_init(void) {
    flSndInitialize();
    Adx_init();
}

void Snd_server(void) {
    Adx_server();
}

void se_req(int a, int b, int c) {
    if (b != 0xFFFF) {
        int v = 127.0f * se_cnfvol_tbl[((u8 *)&system_w)[0x37]];
        if (v != 0) {
            flSndRequest(a, b, v, 0x40, 0x2000, c);
        }
    }
}

void se_req_bgm_vol(int a, int b, int c) {
    if (b != 0xFFFF) {
        int v = 127.0f * se_cnfvol_tbl[((u8 *)&system_w)[0x36]];
        if (v != 0) {
            flSndRequest(a, b, v, 0x40, 0x2000, c);
        }
    }
}
