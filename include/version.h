#ifndef VERSION_H
#define VERSION_H
/* Per-version constants: MH1 JP (default), PS2 Monster Hunter G (-DVERSION_MHG) or MH2 (-DVERSION_MH2, layered on below).
 * Only values that change C code live here; docs/DECISIONS.md "How G lives in the repo". Which build defines what:
 *   PS2 MH1     none
 *   PS2 G       VERSION_MHG; with __MWERKS__ also MHG_PS2 (g_alias.h)
 *   PS2 MH2     VERSION_MH2 only (tools/build.py); files retried in G form get VERSION_MHG too (tools/mh2_carry.py wrappers)
 *   PC MH2      VERSION_MH2 + VERSION_MHG (tools/build_pc.sh)
 *   PC G online VERSION_MHG + MHG_ONLINE, which sets MHG_NET (lbnet.h) */
#ifdef VERSION_MHG
#define ITEM_ROW 20 /* sizeof one Item_data row: G added a member */
#define GUN_ROW 0x18 /* sizeof one Gun_data row (G 0x517720: 78 guns; the name moved to +0x10, the ammo mask to +0x14) */
/* sizeof EMW (monster work): G is 8 + 8 bytes smaller */
#ifdef __MWERKS__
#define EMW_STRIDE 0xA00
#else
#define EMW_STRIDE 0xA10    /* G on the PC keeps MH1's EMW stride (include/em.h) */
#endif
#define IPREP_LIST 0xC80 /* known-recipe bit list: offset in User_data */
#define IPREP_NUM 108 /* recipe pairs in Item_preparation_tbl2 */
#else
#define ITEM_ROW 16
#define GUN_ROW 0x14
#define EMW_STRIDE 0xA10
#define IPREP_LIST 0x3D8
#define IPREP_NUM 66
#endif
/* MH2 values sit on top of the MH1/G blocks. The PS2 MH2 build has no VERSION_MHG, so it gets MH1's GUN_ROW (0x14), STG_POS_N,
 * MAHI_MAX, IPREP_NUM and EM_SLEEP_END, while the PC MH2 build gets G's. MH2's EQ_GUN is 0x18 bytes (plequip.h): no MH2-carried
 * file uses GUN_ROW yet; check it before one does. */
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
#define PLW_AT(i) ((PLW *)((u8 *)player_work + (i) * 0x970)) /* player work is 0x970 bytes in MH2; the u8 * cast because lbui.h and netsyn.h declare player_work as u8[], pl.h / plf.h as PLW[] */
#define PLW_STRIDE 0x970 /* sizeof one player_work entry (MH2 hit_chk_init 0x3C2DC0: addiu 0x970) */
#else
#define PLW_STRIDE 0xA00
#define PLW_AT(i) (&player_work[i])
#endif
#endif
