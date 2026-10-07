/*
 * rt_prof.h - RT_PROF=1: CPU time per subsystem (rt_prof.c). Zones nest;
 * time goes to the innermost open zone (exclusive), so the zones add up
 * to the whole. Logic zones are reported per game tick, draw zones per
 * drawn frame, every 300 ticks.
 */
#ifndef RT_PROF_H
#define RT_PROF_H
enum {
    RTP_LOGIC,          /* game tick: everything not in a zone below (AI, players, quest, menus) */
    RTP_SETS,           /* set objects' move (stage gimmicks) */
    RTP_EFT_MOVE,       /* move_shell / move_eft */
    RTP_SND,            /* rt_snd_tick: sound requests, ADX stream decode */
    RTP_MIX,            /* audio_mix (headless runs with --audio-dump: in the tick) */
    RTP_MOVIE,          /* movie decode (MPEG-2 + colour conversion + texture) */
    RTP_MOVIE_AUDIO,    /* movie ADX decode */
    RTP_DRAW,           /* frame: everything not in a zone below (posing set-up, game draw code) */
    RTP_SKIN,           /* fl_model_pose: CPU skinning + lighting */
    RTP_EFT_DRAW,       /* trans_shell / trans_eft (effect prims) */
    RTP_GFX,            /* inside the graphics backend (GL driver on the PC) */
    RTP_MOTION,         /* skeleton motion evaluation (keys -> channels -> bone matrices) */
    RTP_JOINTS,         /* host joint matrices for the game C (sync_joints, monsters_sync) */
    RTP_STAGE_DRAW,     /* trans_stage: area + set models (game C draw code) */
    RTP_PRIMS,          /* the game's prims (ordering tables) */
    RTP_2D,             /* HUD, text, menus, fade */
    RTP_N
};
void rt_prof_begin(int zone);
void rt_prof_end(int zone);
/* counters per drawn frame */
enum { RTPC_SKIN_VERTS, RTPC_DRAW_VERTS, RTPC_DRAW_TRIS, RTPC_DRAWS, RTPC_SKEL_EVALS, RTPC_N };
void rt_prof_count(int counter, long n);
void rt_prof_tick(void);        /* one game tick done */
void rt_prof_frame(void);       /* one frame drawn; prints every 300 ticks */
int  rt_prof_on(void);
#endif
