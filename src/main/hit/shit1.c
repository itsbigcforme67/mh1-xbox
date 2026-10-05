/* shit1 - SLPM_654.95 0x00114AE0-0x00114D8C (f_sphr part 1, stage hit data):
 * load_stage_hit loads a stage's wall and ground "HITS" files into their
 * areas (stage_hit_data_w/_f) and turns their file-relative offsets into
 * pointers (WallHitInit / GroundHitInit); diorama_w keeps the grid:
 * cell size x/z, cell counts, cell table and polygon area. Format in
 * docs/formats/stage.md. Names guessed from the code. */
#include "types.h"
#include "hit3.h"

extern s32 stage_hit_area_w;
extern s32 stage_hit_area_f;
extern s32 stage_hit_data_w[];
extern s32 stage_hit_data_f[];
void load_file_mdl(s32, s32);
void GroundHitInit(u32 *);
void WallHitInit(u32 *);

void load_stage_hit(int stg) {
    int off = stg * 4;
    s32 a;

    a = stage_hit_area_w;
    load_file_mdl(a, *(s32 *)((u8 *)stage_hit_data_w + off));
    WallHitInit((u32 *)a);
    a = stage_hit_area_f;
    load_file_mdl(a, *(s32 *)((u8 *)stage_hit_data_f + off));
    GroundHitInit((u32 *)a);
}

void WallHitInit(u32 *h) {
    u8 *base = (u8 *)h + 8;
    u32 i;
    u32 j;
    s32 **cell;
    int k;
    s32 *p;

    diorama_w.wcsx = h[2];
    diorama_w.wcsz = h[3];
    diorama_w.wnx = h[4];
    diorama_w.wnz = h[5];
    diorama_w.wtbl = (s32 **)(base + h[8]);
    diorama_w.warea = base + h[9];
    for (i = 0; i < *(u32 *)(base + 8); i++) {
        for (j = 0; j < *(u32 *)(base + 0xC); j++) {
            cell = &diorama_w.wtbl[j + i * *(u32 *)(base + 0xC)];
            *cell = (s32 *)((u8 *)*cell + (s32)base);
            k = 0;
            if (**cell != -1) {
                do {
                    p = (s32 *)((u8 *)*cell + k);
                    k += 4;
                    *p = (s32)((u8 *)*p + (s32)diorama_w.warea);
                } while (*(s32 *)((u8 *)*cell + k) != -1);
            }
        }
    }
}

void GroundHitInit(u32 *h) {
    u8 *base = (u8 *)h + 8;
    u32 i;
    u32 j;
    s32 **cell;
    int k;
    s32 *p;

    diorama_w.gcsx = h[2];
    diorama_w.gcsz = h[3];
    diorama_w.gnx = h[4];
    diorama_w.gnz = h[5];
    diorama_w.gtbl = (s32 **)(base + h[8]);
    diorama_w.garea = base + h[9];
    for (i = 0; i < *(u32 *)(base + 8); i++) {
        for (j = 0; j < *(u32 *)(base + 0xC); j++) {
            cell = &diorama_w.gtbl[j + i * *(u32 *)(base + 0xC)];
            *cell = (s32 *)((u8 *)*cell + (s32)base);
            k = 0;
            if (**cell != -1) {
                do {
                    p = (s32 *)((u8 *)*cell + k);
                    k += 4;
                    *p = (s32)((u8 *)*p + (s32)diorama_w.garea);
                } while (*(s32 *)((u8 *)*cell + k) != -1);
            }
        }
    }
}
