#ifndef SYSW_H
#define SYSW_H
/* system_w (0x3F3690, 0x80 bytes): system/render state. Offsets from f_font (render state
 * cache: the setters skip the call when the cached value is unchanged), f_ioread (system_w_set)
 * and all_reset. Names guessed. Added by agent C. */
#include "types.h"

typedef struct SYSW {
    u8 _pad00[2];
    u8 x02;             /* 0x02 */
    u8 x03;             /* 0x03 set to 1 when going to the game */
    u8 x04;             /* 0x04 cleared by system_w_set */
    u8 x05;             /* 0x05 */
    u8 x06;             /* 0x06 */
    u8 x07;             /* 0x07 set to 1 by system_w_set */
    u8 _pad08[3];
    u8 x0B;             /* 0x0B cleared by ACRMain after the first task start */
    u8 _pad0C[4];
    u8 online;          /* 0x10 Online_ck */
    u8 _pad11;
    u8 x12;             /* 0x12 set to 1 at the end of all_reset */
    u8 _pad13;
    u8 x14;             /* 0x14 set to 1 by system_w_set */
    u8 _pad15[2];
    u8 outmode;         /* 0x17 sound output mode, from option_w+0 (u8: lbu in system_w_set) */
    u8 _pad18[2];
    u8 x1A;             /* 0x1A */
    u8 softkey;         /* 0x1B soft keyboard in use (softkey_ck) */
    u8 _pad1C[6];
    u8 x22;             /* 0x22 set to 1 by system_w_set */
    u8 _pad23;
    u8 x24;             /* 0x24 */
    u8 _pad25[5];
    s16 tex_stage;      /* 0x2A texture stage of the last SetTextureStage (-1 none) */
    u8 src_mode;        /* 0x2C blend source (src_mode_255 index), SetTrnslMode */
    u8 dst_mode;        /* 0x2D blend destination */
    u8 filter;          /* 0x2E texture filter (filter_mode_265 index) */
    u8 x2F;             /* 0x2F set to 0xFF by system_w_set */
    u8 ope;             /* 0x30 operation mode (ope_mode_287 index), SetOpeMode */
    u8 x31;             /* 0x31 */
    u8 x32;             /* 0x32 cleared by all_reset */
    u8 x33;             /* 0x33 cleared by all_reset */
    u8 _pad34;
    u8 loading;         /* 0x35 set while all_reset runs */
    s8 se_vol;          /* 0x36 from option_w+1 */
    s8 bgm_vol;         /* 0x37 from option_w+2 */
    u8 x38;             /* 0x38 */
    u8 x39;             /* 0x39 set to 5 by system_w_set */
    u8 x3A;             /* 0x3A */
    u8 x3B;             /* 0x3B */
    u8 x3C;             /* 0x3C cleared by all_reset */
    u8 x3D;             /* 0x3D set to 0x80 by InitRenderState */
    u8 x3E;             /* 0x3E set to 1 by InitRenderState */
    u8 _pad3F[0x80 - 0x3F];
} SYSW;
extern SYSW system_w;

#endif
