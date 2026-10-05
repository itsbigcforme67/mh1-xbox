/* lf01 - file and texture loading 0x0011EBA0-0x0011EC20: mkTexture. Whole file in loadf_nm.c. */
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




void mkTexture(int file, int idx, int type) {
    void *p = data_load_ptr;

    load_file_mdl(p, filedef_sys[file]);
    mem_tex[idx] = (u32)flCreateTextureFromApx_mem(p, f_type[type]);
}
