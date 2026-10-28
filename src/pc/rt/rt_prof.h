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
    RTP_GPU_WAIT,       /* waiting for the GPU (idle, vblank, a free back buffer: the Xbox backend) */
    RTP_NET,            /* the CpInet socket calls (net_cpinet.c) */
    RTP_N
};
void rt_prof_begin(int zone);
void rt_prof_end(int zone);
/* counters per drawn frame */
enum { RTPC_SKIN_VERTS, RTPC_DRAW_VERTS, RTPC_DRAW_TRIS, RTPC_DRAWS, RTPC_SKEL_EVALS,
       RTPC_DRAWS_2D,       /* gfx_draw_2d calls (the 2D prims, text) */
       RTPC_KICKS,          /* pushbuffer kicks (pb_end: one PUT register write each, Xbox) */
       RTPC_TEX_NEW,        /* textures created or rewritten */
       RTPC_TEX_KB,         /* their texels, KB */
       RTPC_GPU_WAITS,      /* waits for the GPU (Xbox) */
       RTPC_NET_CALLS,      /* CpInet calls (status, receive, send) */
       RTPC_N };
void rt_prof_count(int counter, long n);
void rt_prof_tick(void);        /* one game tick done */
void rt_prof_frame(void);       /* one frame drawn; prints every 300 ticks */
int  rt_prof_on(void);

/* Load timing (always on, wall clock): the time between two drawn frames
 * split by what ran (exclusive, nested). A pause of RT_LOAD_MS (default 150)
 * or more is logged as one "load:" line when the next quick frame comes, with
 * the share of each zone (rt_prof.c). */
enum {
    RTL_READ,           /* disc reads (AFS entries, sound streams) */
    RTL_MELT,           /* Meltw decompression */
    RTL_TEXDEC,         /* APX -> RGBA */
    RTL_TEXUP,          /* gfx_create_texture (Xbox: palette, swizzle, GPU memory) */
    RTL_MODEL,          /* model set-up (AMO/AHI parse, parts, GPU vertex buffers) */
    RTL_MOTION,         /* motion sets */
    RTL_HIT,            /* collision files */
    RTL_GC,             /* the stage-change clean-ups (motion, 2D, files, host models) */
    RTL_SND,            /* sound packs */
    RTL_FLIP,           /* frames the game shows itself during a load (flFlip: loading screens) */
    RTL_N
};
void rt_lt_begin(int zone);
void rt_set_load_wait(void (*fn)(void));    /* called after each disc read (G loading screen) */
void rt_lt_end(int zone);
void rt_lt_frame(int tick, int stage);   /* after each drawn frame */
void rt_lt_init(void);                   /* hooks the fmt readers */
void rt_lt_span_begin(const char *name); /* a named load (one tick): its own "load: NAME" line */
void rt_lt_span_end(void);
#endif
