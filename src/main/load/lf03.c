/* lf03 - file and texture loading 0x0011EF90-0x0011F3EC: load_enemy_model, load_enemy_sub_model, load_npc_model, load_set_model, load_edit_model, debug_model_load, reload_tex, LoadCameraData, Meltw, GetLinkFileNum, GetLinkFileAddress, GetLinkFileSize, amo_ahi_expand. Whole file in loadf_nm.c. */
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






















/* Texture list loading. SLPM_654.95 0x0011E9E0-0x0011EC00 (g_load_texlist). */
extern int f_type[2];
extern int filedef[];
extern s16 skin_tex[];
extern s16 filedef_sys[];

int GetLinkFileNum(int *);
void *flCreateTextureFromApx_mem(void *, int);




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
