/* SLPM_654.95 0x00159D30-0x00159FE4: Pl_se_req2 .. snd_joint_load_init. See snd_nm.c. */
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















/* w is the character work (PLW or EMW): +0x02 kind (EMW), +0x0C u16 side, +0x10 u8 flag */

void Pl_se_req2(void *w, int a, int b, f32 *c, int d, int e) {
    if (Pl_stg_ck(w) & 0xFF) {
        int k;
        if (WB(w, 0x10) != 0) {
            k = 6;
        } else {
            k = WH(w, 0xC) + 2;
        }
        se_req2(k, a, b, c, d, e);
    }
}

void Em_se_req2(void *w, int a, int b, f32 *c, int d, int e) {
    if (Em_stg_ck(w) & 0xFF) {
        int k;
        if (WB(w, 0x10) != 0) {
            k = 6;
        } else {
            k = WH(w, 0xC) + 2;
        }
        se_req2(k, a, Snd_em_id_conv_tbl[WB(w, 2)], c, d, e);
    }
}

void Npc_se_req(int u, int a, f32 *b, int c) {
    if (Em_stg_ck() & 0xFF) {
        se_req2(6, a, 0, b, c, 0);
    }
}

void Npc_se_req_com(int u, int a, f32 *b, int c) {
    if (Em_stg_ck() & 0xFF) {
        se_req2(1, a, 0, b, c, 0);
    }
}

void se_chg_sub(int a, int b, int c, int d, int e, int f) {
    if (b != 0xFFFF) {
        flSndChange(a, b, d, e, f, c);
    }
}

void se_stop_all(void) {
    flSndAllStop();
}

void se_stat(void) {
    flSndStatGet();
}

void snd_joint_load_init(s8 kind) {
    memset(emsnd_p, 0, 7);
    emsnd_p[2] = kind;
}
