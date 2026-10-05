/* snd_nm - whole file (not built); matching runs are sndc01-04.c. Near-matches: se_req2 (7 diffs: `vol` lands in v1 instead
 * of a3), armor_sd_req (180: original keeps 5 saved registers, mine 6), snd_joint_load (15: the reload of emsnd_p[1]
 * in the 'already loaded' branch).
 * Sound request helpers (SLPM_654.95 0x00159410-0x0015A4F0 in part): Snd_init/Snd_server (thin wrappers over
 * the flSnd/ADX layers), se_req (play a sound effect scaled by the option volume), Code_Make (random pick of
 * two sound ids), the Pl/Em/Npc se_req2 front ends (only when the character is on the player's stage) and
 * the sound-pack loaders. Names of the arguments are guesses from use. */
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

int Code_Make(int a, s16 lo, int b, s16 n) {
    s16 t = (u16)ran_suu(1) & 7;

    if (t < lo) {
        return a;
    }
    if (t < lo + n) {
        return b;
    }
    b = 0xFFFF;
    return b;
}

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

/* Plays or retunes a sound effect at a world position: volume falls off with the distance to the camera
 * (vol_dist_tbl: 3 floats per curve t = near, step, far; vol_tbl[t] = volume per step) and the pan comes from
 * the screen x of the position. flag != 0 changes a playing sound instead of starting one. */
void se_req2(int kind, int id, int c, f32 *pos, int t, int flag) {
    f32 sc[4];
    f32 far_;
    f32 dist;
    f32 step;
    f32 dx, dy, dz;
    int vol;
    int pan;
    s32 *tbl;
    int idx;
    f32 mat[16];

    step = vol_dist_tbl[t * 3 + 1];
    far_ = vol_dist_tbl[t * 3 + 2];
    if (id != 0xFFFF) {
        dx = pos[0] - rview_mat[12];
        dy = pos[1] - rview_mat[13];
        dz = pos[2] - rview_mat[14];
        dist = flSqrt(dx * dx + dy * dy + dz * dz);
        tbl = vol_tbl[t];
        if (t == 4) {
            if (!(dist < far_)) {
                dist = far_;
            }
        } else if (!(dist < far_)) {
            return;
        }
        if (dist <= vol_dist_tbl[t * 3]) {
            pan = 0x40;
            vol = 0x7F;
        } else {
            flmatInit(mat);
            flSetRenderState(0x1A, (u32)mat);
            flvecrRotTransPers(sc, pos);
            if (sc[0] < 0.0f) {
                sc[0] = 0.0f;
            }
            if (!(sc[0] <= 640.0f)) {
                sc[0] = 640.0f;
            }
            if (sc[3] < 0.0f) {
                sc[0] = 640.0f - sc[0];
            }
            pan = 63.0f - 48.0f * flCos(2.0f * (3.1415927f * ((1.4173229f * (f32)(int)(sc[0] / 5.03937f)) / 360.0f)));
            idx = flFloor(dist / step);
            vol = tbl[idx] - (int)((dist - step * (f32)idx) / step * (f32)(tbl[idx] - tbl[idx + 1]));
            if (vol == 0) {
                return;
            }
        }
        vol = (f32)vol * se_cnfvol_tbl[((u8 *)&system_w)[0x37]];
        if (vol != 0) {
            if (flag == 0) {
                flSndRequest(kind, id, vol, pan, 0x2000, c);
            } else {
                se_chg_sub(kind, id, c, vol, pan, 0x2000);
            }
        }
    }
}

void Pl_se_req2_com(int u, int a, int b, f32 *c, int d, int e) {
    if (Pl_stg_ck() & 0xFF) {
        se_req2(1, a, b, c, d, e);
    }
}

void Em_se_req2_com(int u, int a, int b, f32 *c, int d, int e) {
    if (Em_stg_ck() & 0xFF) {
        se_req2(1, a, b, c, d, e);
    }
}

extern s8 *Snd_armor_tbl[];

/* Footstep/armour rustle sounds of the player (w = player work): mode 0 plays the three armour parts that
 * differ, 3, 4 and 5 play one part. Snd_armor_tbl rows are per armour set (+0x11) and part. */
void armor_sd_req(u8 *w, int mode) {
    s8 a = Snd_armor_tbl[WB(w, 0x11)][WB(w, 0x358)];
    s8 b = Snd_armor_tbl[10 + WB(w, 0x11)][WB(w, 0x35D)];
    s8 c = Snd_armor_tbl[6 + WB(w, 0x11)][WB(w, 0x35B)];

    if (Pl_stg_ck(w) & 0xFF) {
        switch (mode) {
        case 0:
            if (a != c && c != 0) {
                se_req2(1, c * 2 + ((u16)ran_suu(1) & 1), 0, (f32 *)(w + 0xAC), 1, 0);
            }
            if (a != b && c != b && b != 0) {
                se_req2(1, b * 2 + ((u16)ran_suu(1) & 1), 0, (f32 *)(w + 0xAC), 1, 0);
            }
            if (a != 0) {
                se_req2(1, a * 2 + ((u16)ran_suu(1) & 1), 0, (f32 *)(w + 0xAC), 1, 0);
            }
            break;
        case 3:
            if (c != 0 && c != a) {
                se_req2(1, c * 2 + ((u16)ran_suu(1) & 1), 0, (f32 *)(w + 0xAC), 1, 0);
            }
            break;
        case 5:
            if (b != 0 && b != c && b != a) {
                se_req2(1, b * 2 + ((u16)ran_suu(1) & 1), 0, (f32 *)(w + 0xAC), 1, 0);
            }
            break;
        case 4:
            se_req2(1, Snd_armor_tbl[WB(w, 0x11) + mode * 2][WB(w + mode, 0x358)] * 2 + ((u16)ran_suu(1) & 1), 0,
                    (f32 *)(w + 0xAC), 1, 0);
            break;
        }
    }
}

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

/* Loads the sound joint tables of the monsters in list (nonzero entries are kinds; emsnd_p[3..6] are
 * the loaded files, emsnd_p[1] the list index, emsnd_p[2] the count); returns 1 when done. */
int snd_joint_load(u8 *top, u8 *list) {
    int done = 0;
    s8 f;
    s8 t;
    int i;
    s8 *q;

    switch (emsnd_p[0]) {
    case 0:
        flSndJointInit(top + 0xB4000, top + 0xD4000);
        load_bin_req(0x10088, top);
        emsnd_p[0]++;
        break;
    case 1:
        if (load_busy_ck() == 0) {
            emsnd_p[0]++;
        }
        break;
    case 2:
        if (list[emsnd_p[1]] == 0) {
            t = emsnd_p[1] + 1;
            emsnd_p[1] = t;
            if (t >= emsnd_p[2]) {
                done = 1;
            }
        } else {
            f = Snd_em_id_file_conv_tbl[list[emsnd_p[1]]];
            if (f == 0) {
                t = emsnd_p[1] + 1;
                emsnd_p[1] = t;
                if (t >= emsnd_p[2]) {
                    done = 1;
                }
            } else {
                i = 0;
                q = emsnd_p;
                do {
                    if (f == q[3]) {
                        break;
                    }
                    i++;
                    q++;
                } while (i < 4);
                if (i < 4) {
                    t = emsnd_p[1] + 1;
                    emsnd_p[1] = t;
                    if (t >= emsnd_p[2]) {
                        done = 1;
                    }
                } else {
                    emsnd_p[3 + emsnd_p[1]] = f;
                    load_bin_req((f + 0x88) | 0x10000, top + 0x4000, f);
                    emsnd_p[0]++;
                }
            }
        }
        break;
    case 3:
        if (load_busy_ck() == 0) {
            flSndJointSet(top + 0x4000);
            t = emsnd_p[1] + 1;
            emsnd_p[1] = t;
            if (t >= emsnd_p[2]) {
                done = 1;
                emsnd_p[0]++;
            } else {
                emsnd_p[0]--;
            }
        }
        break;
    default:
        done = 1;
        break;
    }
    return done;
}
