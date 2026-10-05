#ifndef MDLW_H
#define MDLW_H
/* Model work (0x80 bytes per entry, mdlw_heap/mdlw_heap_area; get_mdlw_ptr(n)).
 * Built by model_work_set*, mkMaterial, mkModel*; freed by model_work_free. Offsets are exact,
 * names guessed. Added by agent C (f_get 0x00121B70). */
#include "types.h"

typedef struct MDLW {
    u8 flag;                    /* 0x00 in use */
    u8 x01;                     /* 0x01 */
    u8 _pad02[2];
    s32 mat_set;                /* 0x04 material set number given to model_work_set */
    s16 tex_n;                  /* 0x08 textures loaded (mem_tex[tex_start..]) */
    s16 tex_start;              /* 0x0A */
    s16 mat_n;                  /* 0x0C materials used */
    s16 mat_start;              /* 0x0E first material slot */
    u8 _pad10[0x20 - 0x10];
    s16 hier_n;                 /* 0x20 hierarchy entries */
    s16 hier_no;                /* 0x22 first hierarchy slot */
    u8 *hier0;                  /* 0x24 hierarchy set A */
    u8 *hier1;                  /* 0x28 hierarchy set B (second half of the slots) */
    s16 clay_n;                 /* 0x2C clay entries */
    s16 clay_start;             /* 0x2E first clay slot */
    u8 *clay;                   /* 0x30 CLAY array (0x8C bytes each) */
    s32 num;                    /* 0x34 parts */
    u8 _pad38[0x44 - 0x38];
    u8 *a[4];                   /* 0x44 */
    u8 *b[4];                   /* 0x54 */
    s32 si[4];                  /* 0x64 init motion set handles */
    u8 type;                    /* 0x74 */
    u8 x75;                     /* 0x75 */
    u8 _pad76[0x80 - 0x76];
} MDLW;

MDLW *get_mdlw_ptr(int);

#endif
