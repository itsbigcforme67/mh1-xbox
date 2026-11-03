#ifndef VERSION_H
#define VERSION_H
/* Per-version constants: MH1 JP (default) or PS2 Monster Hunter G (-DVERSION_MHG).
 * Only values that change C code live here; docs/DECISIONS.md "How G lives in the repo". */
#ifdef VERSION_MHG
#define ITEM_ROW 20 /* sizeof one Item_data row: G added a member */
#define GUN_ROW 0x18 /* sizeof one Gun_data row (G 0x517720: 78 guns; the name moved to +0x10, the ammo mask to +0x14) */
#ifdef __MWERKS__
#define EMW_STRIDE 0xA00
#else
#define EMW_STRIDE 0xA10    /* G on the PC keeps MH1's EMW stride (include/em.h) */
#endif
#define IPREP_LIST 0xC80 /* known-recipe bit list: offset in User_data */
#define IPREP_NUM 108 /* recipe pairs in Item_preparation_tbl2 */ /* sizeof EMW (monster work): G is 8 + 8 bytes smaller */
#else
#define ITEM_ROW 16
#define GUN_ROW 0x14
#define EMW_STRIDE 0xA10
#define IPREP_LIST 0x3D8
#define IPREP_NUM 66
#endif
#ifdef VERSION_MH2
#undef EMW_STRIDE
#define EMW_STRIDE 0x9D0
#endif
/* G: its Em_Sleep_End / Em_Sleep2_End symbols (map names) sit at swapped call targets in the sleep/paralysis code */
#ifdef VERSION_MHG
#define EM_SLEEP_END Em_Sleep2_End
#define EM_SLEEP2_END Em_Sleep_End
#else
#define EM_SLEEP_END Em_Sleep_End
#define EM_SLEEP2_END Em_Sleep2_End
#endif
#ifdef VERSION_MHG
#define STG_POS_N 92 /* stages in the per-stage position tables (MH1 88) */
#define MAHI_MAX 4
#else
#define STG_POS_N 88
#define MAHI_MAX 3
#endif
#ifdef VERSION_MH2
#undef IPREP_LIST
#define IPREP_LIST 0x20DB /* 0x5D6B3B, Item_preparation_list_chk */
#undef ITEM_ROW
#define ITEM_ROW 24 /* one Item_data row in MH2 (UseItemChk: id * 24) */
#define PLW_AT(i) ((PLW *)((u8 *)player_work + (i) * 0x970)) /* player work is 0x970 bytes in MH2 */
#else
#define PLW_AT(i) (&player_work[i])
#endif
#endif
