/* File/model loading helpers. SLPM_654.95 0x0011ED20-0x0011F3F0 (f_load): load_file_mdl (read a
 * packed file by id and decompress it with Meltw), per-kind wrappers choosing the file table
 * and destination area, camera data load, texture reload and the link-file (AMO/AHI archive)
 * accessors. Names are from the symbol file; the roles are guessed from use. */
#include "types.h"

extern void *arc_ptr;
extern void *data_load_ptr;
extern void *stage_model;
extern void *pl_area_top;
extern void *cam_data_area;
extern u32 pl_motion_data[], common_motion_data[], em_motion_data[], stage_model_data[], effect_model_data[];
extern u32 weapon_model_data[], em_model_data[], em_sub_model_data[], npc_model_data[], set_model_data[];
extern u32 edit_model_data[2];
extern u32 *armor_model_m[], *armor_model_f[];
extern u32 camera_data_tbl[];
extern u32 mem_tex[];
extern s16 reload_tex_total, reload_tex_id, reload_tex_num;

int load_bin(int id, void *dst);
void Meltw(u16 *src, u16 *dst);
void *memcpy(void *, const void *, unsigned);
void flReloadTexture(int, void *);
void SetCameraData(void *);
void amo_ahi_expand(void *link, void *a, void *b);
long GetLinkFileSize(u8 *, int);
void *GetLinkFileAddress(u8 *, int);

int load_file_mdl(void *dst, int id) {
    if (id < 0) {
        return -1;
    }
    if (load_bin(id | 0x20000, arc_ptr) == 1) {
        Meltw(arc_ptr, dst);
        return 1;
    }
    return 0;
}

int load_pl_motion(int a, int n) {
    return load_file_mdl(pl_area_top, pl_motion_data[n]);
}

int load_plcom_motion(int n) {
    return load_file_mdl(pl_area_top, common_motion_data[n]);
}

int load_em_motion(int a, int n) {
    return load_file_mdl(pl_area_top, em_motion_data[n]);
}

void load_stage_model(int n) {
    u8 *dst = stage_model;

    load_file_mdl(data_load_ptr, stage_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_eft_model(int n) {
    u8 *dst = stage_model;

    load_file_mdl(data_load_ptr, effect_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_weapon_model(int n) {
    u8 *dst = pl_area_top;

    load_file_mdl(data_load_ptr, weapon_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_armor_model(int part, int n, int female) {
    u32 *tbl;
    u32 id;
    u8 *dst;

    if (female == 0) {
        tbl = armor_model_m[n];
    } else {
        tbl = armor_model_f[n];
    }
    id = tbl[part];
    dst = pl_area_top;
    load_file_mdl(data_load_ptr, id);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_enemy_model(int n) {
    u8 *dst = pl_area_top;

    load_file_mdl(data_load_ptr, em_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_enemy_sub_model(int n) {
    u8 *dst = pl_area_top;

    load_file_mdl(data_load_ptr, em_sub_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_npc_model(int n) {
    u8 *dst = pl_area_top;

    load_file_mdl(data_load_ptr, npc_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_set_model(int n) {
    u8 *dst = stage_model;

    load_file_mdl(data_load_ptr, set_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void load_edit_model(int n) {
    u8 *dst = pl_area_top;

    load_file_mdl(data_load_ptr, edit_model_data[n]);
    amo_ahi_expand(data_load_ptr, dst, dst + 0x128000);
}

void debug_model_load(void) {
    /* empty in the retail build */
}

void reload_tex(int num, int id) {
    if (num != 0) {
        flReloadTexture(num, &mem_tex[id]);
        reload_tex_id = id;
        reload_tex_num = num;
        reload_tex_total += (s16)num;
    }
}

void LoadCameraData(int n) {
    u32 id = camera_data_tbl[n];
    void *p = 0;

    if (id != 0) {
        load_file_mdl(cam_data_area, id);
        p = cam_data_area;
    }
    SetCameraData(p);
}

/* LZ style decompressor: 16 bit flag words (bit set = copy token), tokens are u16:
 * top 5 bits = length (0 = a separate u16 length follows), low 11 bits = distance back in u16s
 * (0 = fill with zeros). Clear flag = one literal u16. */
void Meltw(u16 *src, u16 *dst) {
    int flags;
    int bits;

    bits = 0;
    flags = 0;

    for (;;) {
        u32 len;
        u32 dist;

        if (bits == 0) {
            flags = *(s16 *)src;
            bits = 0x8000;
            src++;
        }
        if (flags & bits) {
            u32 tok = *src;

            len = tok >> 11;
            src++;
            if (len != 0) {
                dist = tok & 0x7FF;
            } else {
                len = *src;
                dist = tok;
                src++;
            }
            if (dist == 0) {
                if (len == 0) {
                    return;
                }
                do {
                    *dst++ = 0;
                } while (--len != 0);
            } else {
                u16 *from = dst - dist;

                do {
                    *dst++ = *from++;
                } while (--len != 0);
            }
        } else {
            *dst++ = *src++;
        }
        bits >>= 1;
    }
}

int GetLinkFileNum(int *p) {
    return p[0];
}

void *GetLinkFileAddress(u8 *p, int n) {
    return p + *(int *)(p + n * 8 + 4);
}

long GetLinkFileSize(u8 *p, int n) {
    return *(int *)(p + n * 8 + 8);
}

void amo_ahi_expand(void *link, void *a, void *b) {
    void *src = GetLinkFileAddress(link, 0);

    memcpy(a, src, (s32)GetLinkFileSize(link, 0));
    src = GetLinkFileAddress(link, 1);
    memcpy(b, src, (s32)GetLinkFileSize(link, 1));
}

/* Texture list loading. SLPM_654.95 0x0011E9E0-0x0011EC00 (g_load_texlist). */
extern int f_type[2];
extern int filedef[];
extern s16 skin_tex[];
extern s16 filedef_sys[];

int GetLinkFileNum(int *);
void *flCreateTextureFromApx_mem(void *, int);

/* Loads file `id` (a link of APX textures) and registers up to 0x32 of them from mem_tex[base]. */
int load_texlist(int id, int base, int type) {
    int i;
    int n;
    void *p = data_load_ptr;

    load_file_mdl(p, id);
    n = GetLinkFileNum(p);
    if (n > 0x32) {
        n = 0x32;
    }
    for (i = 0; i < n; i++) {
        mem_tex[base++] = (u32)flCreateTextureFromApx_mem(GetLinkFileAddress(p, i), f_type[type]);
    }
    return n;
}

void load_texlist_pl(int *w, int base, int type, int skin) {
    void *p = data_load_ptr;
    int *ft;

    load_file_mdl(p, filedef[*(int *)w[1]]);
    ft = &f_type[type];
    mem_tex[base] = (u32)flCreateTextureFromApx_mem(p, *ft);
    if (w[0] >= 2) {
        load_file_mdl(p, skin_tex[skin]);
        mem_tex[base + 1] = (u32)flCreateTextureFromApx_mem(p, *ft);
    }
}

void mkTexture(int file, int idx, int type) {
    void *p = data_load_ptr;

    load_file_mdl(p, filedef_sys[file]);
    mem_tex[idx] = (u32)flCreateTextureFromApx_mem(p, f_type[type]);
}
