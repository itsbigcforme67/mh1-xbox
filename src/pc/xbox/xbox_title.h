/*
 * xbox_title.h - the Xbox save identity, in one place. The title id is a
 * PLACEHOLDER (MH1X): the Xbox dashboard keys saves by the title id in the
 * XBE certificate, so the XBE's certificate title id has to be this value too
 * (nxdk's XBE title id option); the owner may supply a real one.
 */
#ifndef XBOX_TITLE_H
#define XBOX_TITLE_H
#define XBOX_TITLE_ID      0x4D480001u                /* "MH" 0001 */
#define XBOX_TITLE_ID_STR  "4D480001"                 /* E:\UDATA\<this>\ */
#define XBOX_TITLE_NAME    "Monster Hunter"
#define XBOX_SAVE_NAME     "Monster Hunter save"      /* shown under the title in the memory manager */
#define XBOX_SAVE_ID       "4D48000100000001"         /* the one save directory (16 hex digits) */
#define PS2_SAVE_DIR       "BISLPM-65495MH"           /* the PS2 save's directory name (mc_file_tbl[0]) */
#endif
