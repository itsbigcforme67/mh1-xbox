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
/* Zeroed host memory standing in for PS2 .bss at va (pointers in data
 * tables that point there), or NULL if va is not .bss. */
void *rt_bss_shadow(uint32_t va);
/* Fill the game's data tables (rt_data.c) from the loaded images.
 * Returns the number of tables that could not be found. */
int rt_import_data(void);
/* After rt_import_data and rt_set_lobby: the lobby tables (rt_data.c). */
int rt_import_lobby(void);
/* Pointer words (R_MIPS_32 relocations of the ELF) and symbols. */
int rt_load_relocs(void);
int rt_is_pointer(uint32_t va);
void rt_relocate_range(uint32_t va, uint8_t *dst, size_t size, void *(*map)(uint32_t));
void rt_relocate_images(void *(*map)(uint32_t));
const char *rt_sym_at(uint32_t va, uint32_t *off, int *func);
/* After rt_import_data: the host pointer stored at PS2 address va (a
 * pointer word of the images, already translated), or NULL. */
const void *rt_ptr_at(uint32_t va);
/* lobby.bin (village / lobby overlay, same vram as game.bin): its own
 * image, pointer words and symbols (rt_mem.c), its tables (rt_data.c) */
void rt_set_lobby(uint8_t *bin, size_t n);   /* takes ownership */
const uint8_t *rt_lb_addr(uint32_t va, size_t n);
int rt_lb_in_range(uint32_t va);
void *rt_lb_bss_shadow(uint32_t va);
const char *rt_lb_sym_at(uint32_t va, uint32_t *off, int *func);
int rt_lb_is_pointer(uint32_t va);
void rt_lb_relocate_range(uint32_t va, uint8_t *dst, size_t size, void *(*map)(uint32_t));
void rt_lb_relocate_image(void *(*map)(uint32_t));

/* ------------------------------------------------------------ clays */
/* Register a host clay; the result is the handle the game passes to
 * flExecuteClay. */
int rt_register_clay(gfx_clay *c);
/* Point the game's set_mdlw/stage_work.mdl at a CLAY array made of these
 * clays (the stage's set model, e.g. st04_1). Returns the handle of
 * clays[0]; the others follow in order. attr: CLAY+0x88 word per clay
 * (rt_clay_attr_word) or NULL. */
int rt_bind_set_model(gfx_clay *const *clays, const uint32_t *attr, int n);
/* The stage's area model (stage_work.mdl, one clay per AMO part), drawn
 * by the game's trans_stage (rt_stage_draw). Returns the first handle. */
int rt_bind_stage_model(gfx_clay *const *clays, const uint32_t *attr, int n);
/* 1 if game code drew this handle at least once (the host's generic model
 * draw skips such parts so they are not drawn twice). */
int rt_clay_claimed(int handle);

/* Hand effect model k (eft_mdlw[k]: 0 ef_00, 1-3 kage04-06, 4 ef_01) to
 * the game C; attr as for rt_bind_set_model. */
void rt_bind_eft_model(int k, gfx_clay *const *c, const uint32_t *attr, int n);
void rt_bind_eft_skin(int k, int nbone, void (*cb)(int k, const float *mats, int n));
/* Files the game C loads by AFS index (load_file_mdl): fn returns the
 * Meltw-decompressed entry (malloc'd, the caller frees) and its size. */
void rt_set_file_loader(uint8_t *(*fn)(int idx, size_t *n));
/* Load a stage's wall and ground collision (the game's load_stage_hit:
 * stage_hit_data_w/_f files, WallHitInit/GroundHitInit). 0 on success. */
int rt_load_stage_hit(int stage);
/* The game's GetGroundHit at (x, z) from above ymax: 1 and the highest
 * ground y not above ymax, or 0 if there is no ground polygon there. */
int rt_ground_y(float x, float z, float ymax, float *y);

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

/* The game's motion system (decompiled frame_init/frame_move over the
 * native fl motion layer in rt_motion.c) for player no: builds the common
 * hunter motion sets from plcom_tbl.bin (create_plcom_motion, once),
 * starts legs_id on layer 0 and upper_id on layer 1. tick: frame_move.
 * pose: pose an fl_skel (fl_skel *) from the motion player. */
void rt_player_motion_start(int no, const uint8_t *plcom_tbl, int legs_id, int upper_id);
int rt_player_motion_tick(int no);
void rt_player_pose(int no, void *fl_skel_ptr);
void rt_player_get(int no, float pos[3], int *ang_y);

/* The same for monster em_work[no] (model number mdl_no, monster kind
 * kind for em_parts_num): create_em_motion from its *_tbl.bin, ids[g] on
 * layer g (ids >= 1000). */
void rt_monster_motion_start(int no, int mdl_no, const uint8_t *tbl, int kind, const int *ids, int layers);
int rt_monster_motion_tick(int no);
/* Place em_work[no] on the stage (then each tick also runs em_move's wall
 * and ground collision) and read back where it is. */
void rt_monster_place(int no, int kind, const float pos[3], int ang_y);
/* rt_em.c: quest mission data and the game's monster loop (enemy_mv) */
int rt_quest_load(int no);
int rt_quest_monster_stage(int *kind);
int rt_monster_spawn(int kind, const float pos[3], int ang_y);
int rt_monster_tick(int no);
void rt_monster_get(int no, float pos[3], int *ang_y);
void rt_monster_pose(int no, void *fl_skel_ptr);

/* Host pad state for the next ticks (fl pad bits + sticks, see
 * src/pc/pad/pad.h); rt_pad_tick (called by rt_player_tick) runs the PS2
 * pad driver step and the game's swset(). */
void rt_pad_set(uint16_t fl_bits, int lx, int ly, int rx, int ry);
/* The pad driver step alone (Psw from the host pad, no swset). */
void rt_pad_read(void);
/* rt_village.c: the village (lobby.bin Local_main) after a quest */
void rt_village_enter(void);
int rt_village_tick(void);
int rt_village_active(void);
void rt_set_npc_model_loader(void (*fn)(int slot, int amh, int tex));
void rt_set_em_model_loader(void (*fn)(int slot, int kind));
void rt_monster_joints(int no, const float *world, int n);
/* create_em_motion for model slot `slot` from a monster's *_tbl.bin */
void rt_em_motion_create(int slot, int kind, const uint8_t *tbl);
void rt_monster_pose(int no, void *fl_skel_ptr);
void rt_flow_set_village(void (*fn)(void));
void rt_flow_set_mode(int mode);   /* test aid: jump to a game mode */
int rt_game_stage(void);           /* game_w.stage */
/* One tick of player no with the pad: pl_sw_set (game C), then the host
 * stand-in for the normal state (rt_player.c: turn/run/idle with the
 * game's frame_init/frame_move) and ground following. */
void rt_player_tick(int no);
void rt_player_game_init(int no);
void rt_monster_joints(int no, const float *world, int n);  /* em_work[no] joint world matrices */
void rt_hit_check(void);                /* the game's hit_check (shells vs monsters/players) */
int rt_player_weapon(int no, float *root0, float *root1);   /* weapon root matrices (weapon_trans) */
int rt_player_weapon_model(int no);     /* PLW+0x34C */
int rt_weapon_afs(int model, int tex);  /* weapon_model_data / WEAPON_TEX entry */
int rt_player_job(int no);              /* weapon class PLW+2 (0 GS, 1/5 bowgun, 2 hammer, 3 lance, 4 SnS) */
void rt_motion_load_pl(int no, const uint8_t *tbl);   /* create_pl_motion on wNN_tbl.bin */
void rt_player_parts(int no, const float *world, int n);   /* joint world matrices from the host skeleton */   /* equipment + the game's pl_init (rt_player.c) */
int rt_player_uses_game(void);       /* 0 with RT_PL_STANDIN=1 */
void rt_player_set_ang(int no, int ang_y);
/* What the game's sw_set_sub gave player no: buttons, left stick. */
void rt_player_sw(int no, int *now, int *ang, int *pow);

/* Sound (rt_snd.c, docs/pc.md "Sound"): open AFS00/AFS01 in disc and the
 * output device (device = 0: mixer only, for --audio-dump); load a stage's
 * packs and start its stream; one tick per game tick; footsteps of the
 * host player stand-in (call before frame_move). */
int  rt_snd_init(const char *disc, int device);
void rt_snd_stage(int stage, const int *em_kinds, int nem);
void rt_snd_em_add(int kind);        /* a monster kind's sound pack on port 6 */
void rt_snd_tick(void);
void rt_snd_player_motion(int no);
void rt_snd_monster_motion(int no);
void rt_snd_shutdown(void);

/* The game camera (src/main/cam CameraMove, rt_cam.c): init for a stage
 * once the master player is set, one tick per game tick after the player,
 * and the resulting view (eye, target, roll, fov in radians). */
void rt_cam_init(int stage);
void rt_cam_tick(void);
void rt_cam_view(float eye[3], float tar[3], float *roll, float *fov);

/* RT_SPAWN="eft13:N,eft17:N,shell22:N,eft14:N,eft08:N": spawn test effects at pos. */
void rt_debug_spawn(const float pos[3]);

/* ------------------------------------------------------------ game loop */
/* Set up the game globals for a stage and spawn its set objects. */
void rt_game_init(int stage);
/* One game tick (the PS2 game logic runs at 30 per second). */
void rt_game_move(void);
/* Draw the stage like the game's trans_stage: area model parts (sky,
 * per-stage placed/spun/scrolled parts) and the set-model parts it places. */
void rt_stage_draw(void);
/* Walk the ordering tables queued by the last tick and call each prim's
 * trans(). VIEW/PROJECTION must already be set; render states touched by
 * game code are restored afterwards. */
void rt_game_draw(void);
/* the quest HUD (rt_quest.c): set-up after rt_game_init, Pit_mv per tick */
void rt_hud_init(void);
void rt_hud_tick(void);
/* the screen layers (HUD, menus, info banner, text) after the 3D scene */
void rt_game_draw_2d(void);

/* ------------------------------------------------------------ game modes (rt_flow.c) */
/* fn = the host's game tick, called as game_core (swset/move/trans/hit_check) */
void rt_flow_set_core(void (*fn)(void));
/* one tick of game_w.mode 2..5 (game2 quest, game3 clear screen, game5
 * result); returns the mode that ran */
int rt_flow_tick(void);
int rt_flow_mode(void);         /* game_w.mode */
/* mode 6 (after the result screen): the host's "back" (village not ported) */
void rt_flow_set_back(void (*fn)(void));
void rt_monster_clear_all(void);
void rt_quest_free_hunt(void);  /* no --quest: Quest_init only */
/* the host's stage (re)load, called by st_model_load in a stage change */
void rt_set_stage_loader(int (*fn)(int));
int rt_monster_shown(int no);   /* em_work[no] in use and on the current stage */
void rt_pad_tick(void);         /* Psw from the host pad + swset (rt_pad.c) */

#endif
