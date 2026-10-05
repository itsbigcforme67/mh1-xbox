/* Sprite list transfer. SLPM_654.95 0x0015AED0-0x0015B300 (f_trans).
 * Sprites queued by Put_sprite etc. sit in sprite_area (0x50 bytes each); trans_sprite()
 * draws them at the end of the frame, optionally depth sorted (softdip 0xAB). */
#include "types.h"

extern u8 *sprite_area;
extern s16 spr_list_no;
extern u8 sort_no[0x100];
extern u8 spr_sort_num;
extern u8 spr_sort_flag;
extern u32 mem_tex[];
extern u8 view_mat[];

void set_viewproj(int);
void flmatrStore(int, void *);
int softdip_ck(int);
void flmatInit(void *);
void flSetRenderState(int, int);
void InitRenderState(int);
void SetFilterMode(int);
void SetTrnslMode(int, int);
void SetTextureStage(int);
void reload_tex(int, int);
void flps0D00(void *);
void flps1300(void *);
void flps0F00(void *);
void flps1400(void *);
void flps1600(void *);
void trans_spr_sub(u8 *);
void trans_sprite_sort(void);

void trans_sprite(void) {
    u8 m[64];
    int i;
    u8 *spr = sprite_area;

    set_viewproj(0);
    flmatrStore(0x21, view_mat);
    if (softdip_ck(0xAB) != 0) {
        trans_sprite_sort();
        return;
    }
    flmatInit(m);
    flSetRenderState(0x1A, (int)m);
    flSetRenderState(0x60, 0);
    for (i = 0; i < spr_list_no; i++) {
        trans_spr_sub(spr);
        spr += 0x50;
    }
    InitRenderState(1);
}

/* Inserts sprite idx (depth z) into the sort_no order. */
static void sort_sub(f32 z, int idx) {
    int i;
    int n;
    u8 *s;

    if (spr_sort_num == 0x100) {
        return;
    }
    s = sprite_area;
    for (i = 0; i < spr_sort_num; i++) {
        u8 *p = s + sort_no[i] * 0x50;
        if (spr_sort_flag == 0) {
            if (!(*(f32 *)(p + 4) <= z)) {
                break;
            }
        } else if (*(f32 *)(p + 4) < z) {
            break;
        }
    }
    for (n = spr_sort_num; i < n; n--) {
        sort_no[n] = sort_no[n - 1];
    }
    sort_no[i] = idx;
    spr_sort_num++;
}

void trans_sprite_sort(void) {
    u8 m[64];
    u8 *spr;
    u8 *s;
    int i;

    spr_sort_flag = 1;
    spr_sort_num = 0;
    s = spr = sprite_area;
    for (i = 0; i < spr_list_no; i++) {
        if (*s & 1) {
            sort_sub(*(f32 *)(s + 4), i);
        }
        s += 0x50;
    }
    flmatInit(m);
    flSetRenderState(0x1A, (int)m);
    for (i = 0; i < spr_sort_num; i++) {
        trans_spr_sub(spr + sort_no[i] * 0x50);
    }
    s = spr;
    for (i = 0; i < spr_list_no; i++) {
        if (!(*s & 1)) {
            trans_spr_sub(s);
        }
        s += 0x50;
    }
    InitRenderState(1);
}

void trans_spr_sub(u8 *s) {
    u16 tex;

    SetFilterMode((*s & 8) / 8);
    if (s[1] != 0) {
        SetTrnslMode(4, 1);
    } else {
        SetTrnslMode(4, 5);
    }
    switch (*s & 3) {
    case 0:
        flps0D00(s + 8);
        break;
    case 1:
        flps1300(s + 8);
        break;
    case 2:
        tex = *(u16 *)(s + 2);
        if (mem_tex[tex] != 0) {
            reload_tex(1, tex);
            SetTextureStage(*(u16 *)(s + 2));
            flps0F00(s + 8);
        }
        break;
    case 3:
        tex = *(u16 *)(s + 2);
        if (mem_tex[tex] != 0) {
            reload_tex(1, tex);
            SetTextureStage(*(u16 *)(s + 2));
            if (*s & 4) {
                flps1600(s + 8);
            } else {
                flps1400(s + 8);
            }
        }
        break;
    }
}
