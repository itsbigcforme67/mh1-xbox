/* shit1 - SLPM_654.95 0x00114AE0-0x00114B4C (f_sphr part 1, stage hit data):
 * load_stage_hit loads a stage's wall and ground "HITS" files into their
 * areas (stage_hit_data_w/_f) and turns their file-relative offsets into
 * pointers (WallHitInit / GroundHitInit, in shit1_nm.c); diorama_w keeps the grid:
 * cell size x/z, cell counts, cell table and polygon area. Format in
 * docs/formats/stage.md. Names guessed from the code. */
#include "types.h"

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
