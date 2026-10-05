#ifndef SYSW_H
#define SYSW_H
/* system_w (0x3F3690, 0x80 bytes): system/render state. Offsets from f_font (render state
 * cache: the setters skip the call when the cached value is unchanged). Added by agent C. */
#include "types.h"

typedef struct SYSW {
    u8 _pad00[0x10];
    u8 online;          /* 0x10 Online_ck */
    u8 _pad11;
    u8 x12;             /* 0x12 set to 1 at the end of all_reset */
    u8 _pad13[0x1B - 0x13];
    u8 softkey;         /* 0x1B soft keyboard in use (softkey_ck) */
    u8 _pad1C[0x2A - 0x1C];
    s16 tex_stage;      /* 0x2A texture stage of the last SetTextureStage (-1 none) */
    u8 src_mode;        /* 0x2C blend source (src_mode_255 index), SetTrnslMode */
    u8 dst_mode;        /* 0x2D blend destination */
    u8 filter;          /* 0x2E texture filter (filter_mode_265 index) */
    u8 _pad2F;
    u8 ope;             /* 0x30 operation mode (ope_mode_287 index), SetOpeMode */
    u8 _pad31;
    u8 x32;             /* 0x32 cleared by all_reset */
    u8 x33;             /* 0x33 cleared by all_reset */
    u8 _pad34;
    u8 loading;         /* 0x35 set while all_reset runs */
    u8 _pad36[0x3C - 0x36];
    u8 x3C;             /* 0x3C cleared by all_reset */
    u8 x3D;             /* 0x3D set to 0x80 by InitRenderState */
    u8 x3E;             /* 0x3E set to 1 by InitRenderState */
    u8 _pad3F[0x80 - 0x3F];
} SYSW;
extern SYSW system_w;

#endif
