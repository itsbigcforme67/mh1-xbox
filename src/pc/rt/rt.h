/*
 * rt.h - the port runtime: what decompiled game C (src/game, src/main)
 * expects from the PS2 side, implemented natively on top of src/pc/gfx.
 *
 * The game C is compiled unchanged with the game's own include/ headers in
 * a 32-bit build (docs/pc.md "Port runtime"), so its structs keep their PS2
 * offsets and pointers fit in the u32 fields the game uses. This header is
 * the host side (viewer/app) of the runtime and does not pull in any game
 * header.
 */
#ifndef MH_RT_H
#define MH_RT_H

#include <stddef.h>
#include <stdint.h>
#include "../gfx/gfx.h"

/* ------------------------------------------------------------ PS2 memory */
/* Main ELF (SLPM_654.95) and the game.bin overlay (vram 0x533980), kept in
 * memory so game data tables can be read from the user's own disc files. */
int rt_load_elf(const char *path);
void rt_set_overlay(uint8_t *bin, size_t n);   /* takes ownership */
/* Pointer to `n` bytes at PS2 address `va`, or NULL if not in a loaded image. */
const uint8_t *rt_addr(uint32_t va, size_t n);
/* 1 if the range is .bss (zero at start) of the ELF or the overlay. */
int rt_in_bss(uint32_t va, size_t n);
/* Fill the game's data tables (rt_data.c) from the loaded images.
 * Returns the number of tables that could not be found. */
int rt_import_data(void);

/* ------------------------------------------------------------ clays */
/* Register a host clay; the result is the handle the game passes to
 * flExecuteClay. */
int rt_register_clay(gfx_clay *c);
/* Point the game's set_mdlw/stage_work.mdl at a CLAY array made of these
 * clays (the stage's set model, e.g. st04_1). Returns the handle of
 * clays[0]; the others follow in order. attr: CLAY+0x88 word per clay
 * (rt_clay_attr_word) or NULL. */
int rt_bind_set_model(gfx_clay *const *clays, const uint32_t *attr, int n);
/* 1 if game code drew this handle at least once (the host's generic model
 * draw skips such parts so they are not drawn twice). */
int rt_clay_claimed(int handle);

/* CLAY+0x88 attribute word from an AMO part's 0xF0000 chunk (18 words, as
 * amo_part.attr; NULL = no chunk -> 0), like Attribute_from_amo. */
uint32_t rt_clay_attr_word(const int32_t *attr);
/* The game's clay_attr_set / clay_attr_reset, for host draws: blend mode,
 * blend operation, filter and clamp of one part. */
void rt_clay_attr_set(uint32_t attr);
void rt_clay_attr_reset(void);

/* The camera's world matrix (fl layout, rows right/up/back/eye): sets the
 * game's rview_mat and rview_matY like View_move. Call once per frame. */
void rt_set_camera(const float cam_world[16]);

/* Put the host's hunter into player_work[no] (in use, on this stage, at
 * pos), so game code that follows or tests the master player sees it.
 * Call after rt_game_init. */
void rt_set_player(int no, const float pos[3]);

/* ------------------------------------------------------------ game loop */
/* Set up the game globals for a stage and spawn its set objects. */
void rt_game_init(int stage);
/* One game tick (the PS2 game logic runs at 30 per second). */
void rt_game_move(void);
/* Walk the ordering tables queued by the last tick and call each prim's
 * trans(). VIEW/PROJECTION must already be set; render states touched by
 * game code are restored afterwards. */
void rt_game_draw(void);

#endif
