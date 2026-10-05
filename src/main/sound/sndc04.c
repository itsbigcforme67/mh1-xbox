/* SLPM_654.95 0x0015A230-0x0015A51C: snd_joint_load_pl .. edit_se_load. See snd_nm.c. */
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















/* Loads the player's weapon/armour sound joint tables, one step per call (emsnd_p[0] is the step);
 * returns 1 when done. w is the player work, +0x34C weapon kind, +0x8D3 armour. */
int snd_joint_load_pl(u8 *top, void *w) {
    int done = 0;
    int id;

    switch (emsnd_p[0]) {
    case 0:
        flSndJointInit(top + 0x40000, top + 0x80000);
        load_bin_req((Snd_weapon_tbl[WB(w, 0x34C)] + 0x10) | 0x10000, top);
        emsnd_p[0]++;
        break;
    case 1:
        if (load_busy_ck() == 0) {
            emsnd_p[0]++;
            flSndJointSet(top);
        }
        break;
    case 2:
        if (WB(w, 0x11) == 0) {
            id = (WB(w, 0x8D3) + 0x73) | 0x10000;
        } else if (WB(w, 0x34C) == 0x6F) {
            id = 0x10087;
        } else {
            id = (WB(w, 0x8D3) + 0x7D) | 0x10000;
        }
        load_bin_req(id, top + 0x20000);
        emsnd_p[0]++;
        break;
    case 3:
        if (load_busy_ck() == 0) {
            flSndJointSet(top + 0x20000);
            done = 1;
            emsnd_p[0]++;
        }
        break;
    default:
        done = 1;
        break;
    }
    return done;
}

void Lbs_se_load(void) {
    void *p = data_load_ptr;

    flSndPortStop(6);
    FlushCache(0);
    load_bin(0x10003, p);
    flSndPackLoad(p, 6);
}

void Menu_snd_load(void) {
    void *p = data_load_ptr;

    flSndPortStop(1);
    FlushCache(0);
    load_bin(0x10002, p);
    flSndPackLoad(p, 1);
    flSndPortStop(7);
    FlushCache(0);
    load_bin(0x10006, p);
    flSndPackLoad(p, 7);
}

int edit_se_load(void) {
    u8 *p = pl_area_top;

    system_w.loading = 1;
    flSndPortStop(1);
    FlushCache(0);
    load_bin(0x10002, p);
    flSndPackLoad(p, 1);
    flSndPortStop(6);
    FlushCache(0);
    load_bin(0x10005, p);
    flSndPackLoad(p, 6);
    system_w.loading = 0;
    return 1;
}
