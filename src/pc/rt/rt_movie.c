/*
 * rt_movie.c - the game's movie_* interface (main f_movie, which drives
 * CRI's Sofdec player on the PS2) on the port's own player: sfd.c (MPEG
 * program stream demux + libmpeg2 + ADX) from the player's AFS00.AFS.
 *
 * The game calls (opening_demo in select/demo.c, the extras menu in
 * main/omake): movie_reset, movie_start(no) (create the player and its
 * texture), movie_request(no, 0) (play), movie_server() once per tick
 * (returns 1 once a frame has arrived), movie_draw(), movie_status_ck()
 * (3 = played to the end) and movie_exit(). sfd_tbl (main 0x348E30, 16
 * bytes per movie: AFS00 entry, format, length, width, height ...) is
 * imported from the player's SLPM_654.95.
 *
 * Sync: the audio is the clock. The mixer counts the frames of the movie
 * stream it has played; the video is decoded up to the frame that time
 * calls for. Without an audio device (headless runs) the clock is the
 * game tick (30 per second). RT_NOMOVIE=1 skips movies (tests).
 */
#include "rt.h"
#include "types.h"
#include "../audio/audio.h"
#include "../gfx/gfx.h"
#include "../movie/sfd.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern u8 sfd_tbl[];
extern u8 system_w[];
extern u8 adx_vol_tbl[], adx_cnfvol_tbl[];
extern u8 demo_w[];
const fmt_afs *rt_snd_afs00(void);
void rt_2d_restore_texture(void);

#define NTEX 3

static struct {
    sfd *s;
    int no, w, h;               /* table number, table size */
    int playing, ended, ready;
    long ticks;                 /* movie_server calls since movie_request */
    double t_audio_end;         /* clock when the audio stopped driving it */
    long ticks_audio_end;
    int audio_active;
    gfx_texture *tex[NTEX];
    int tw, th, ti;
    float vol;
    double wall0;
    int dump_n;                 /* RT_MOVIE_DUMP=dir: frames written */
} mv;

static double wall_s(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
static const u8 *tbl(int no) { return sfd_tbl + 16 * no; }
static u32 rd32(const u8 *p) { return p[0] | p[1] << 8 | p[2] << 16 | (u32)p[3] << 24; }
static u32 rd16(const u8 *p) { return p[0] | p[1] << 8; }

static void free_tex(void)
{
    int i;
    for (i = 0; i < NTEX; i++)
        if (mv.tex[i])
            gfx_release_texture(mv.tex[i]);
    memset(mv.tex, 0, sizeof mv.tex);
}

void rt_movie_stop(void)
{
    if (mv.s) {
        int fr;
        double tot, mx;
        sfd_stats(mv.s, &fr, &tot, &mx);
        if (getenv("RT_TRACE") || getenv("RT_MOVIE_TRACE"))
            fprintf(stderr, "movie: closed after %d frames, decode %.2f ms/frame (worst %.1f ms)\n", fr, fr ? tot / fr : 0.0, mx);
        audio_stream_clear(AUDIO_STREAM_MOVIE);
        sfd_close(mv.s);
    }
    free_tex();
    memset(&mv, 0, sizeof mv);
}

void movie_reset(void) { rt_movie_stop(); }

int movie_start(int no)
{
    const fmt_afs *afs = rt_snd_afs00();
    const u8 *t;
    rt_movie_stop();
    if (getenv("RT_NOMOVIE") || !afs || no < 0 || no > 8)
        return -1;
    t = tbl(no);
    mv.s = sfd_open(afs, (int)rd32(t));
    if (!mv.s) {
        fprintf(stderr, "movie: table entry %d (AFS00 %u) is not a movie\n", no, rd32(t));
        return -1;
    }
    mv.no = no;
    mv.w = (int)rd16(t + 0xA);
    mv.h = (int)rd16(t + 0xC);
    if (getenv("RT_TRACE") || getenv("RT_MOVIE_TRACE"))
        fprintf(stderr, "movie: %d = %s (table %dx%d)\n", no, afs->name[rd32(t)], mv.w, mv.h);
    return 0;
}

static void set_volume(void)
{
    int db = (int)(int16_t)rd16(adx_vol_tbl + 2 * adx_cnfvol_tbl[system_w[0x36]]);
    if (db < -0x3C0)
        db = -0x3C0;
    mv.vol = db <= -999 ? 0.0f : powf(10.0f, db / 200.0f);
    audio_stream_vol(AUDIO_STREAM_MOVIE, mv.vol);
}

void movie_request(int no, int add)
{
    (void)no; (void)add;
    if (!mv.s)
        return;
    audio_stream_clear(AUDIO_STREAM_MOVIE);
    mv.playing = 1;
    mv.ticks = 0;
    mv.wall0 = wall_s();
    set_volume();
}

void movie_exit(void) { rt_movie_stop(); }

int movie_status_ck(void)
{
    return mv.s && !mv.ended ? 2 : 3;
}

/* audio: keep about half a second queued in the mixer */
static void audio_feed(void)
{
    int16_t out[32 * 2 * 64];
    int room, n;
    int live = audio_live();
    sfd_audio_fill(mv.s, 1 << 15);
    if (!sfd_audio_rate(mv.s))
        return;
    if (!live) {                        /* nobody listens: just keep the queue short */
        while (sfd_audio_pull(mv.s, out, 32 * 64) > 0)
            ;
        return;
    }
    for (room = audio_stream_free(AUDIO_STREAM_MOVIE) - AUDIO_RATE / 2; room >= 32 * 64; room -= 32 * 64) {
        sfd_audio_fill(mv.s, 1 << 15);
        n = sfd_audio_pull(mv.s, out, 32 * 64);
        if (n <= 0)
            break;
        audio_stream_write(AUDIO_STREAM_MOVIE, out, n, sfd_audio_rate(mv.s));
    }
}

static double clock_now(void)
{
    int rate = sfd_audio_rate(mv.s);
    if (audio_live() && rate && !sfd_audio_done(mv.s)) {
        double t = (double)audio_stream_consumed(AUDIO_STREAM_MOVIE) / rate;
        mv.audio_active = 1;
        mv.t_audio_end = t;
        mv.ticks_audio_end = mv.ticks;
        return t;
    }
    if (mv.audio_active)                /* the audio ran out: carry on by the tick */
        return mv.t_audio_end + (mv.ticks - mv.ticks_audio_end) / 30.0;
    return mv.ticks / 30.0;
}

static void upload(void)
{
    int w, h;
    const uint8_t *px = sfd_rgba(mv.s, &w, &h);
    if (!px)
        return;
    if (mv.tw != w || mv.th != h) {
        free_tex();
        mv.tw = w;
        mv.th = h;
    }
    mv.ti = (mv.ti + 1) % NTEX;
    if (mv.tex[mv.ti])
        gfx_release_texture(mv.tex[mv.ti]);
    mv.tex[mv.ti] = gfx_create_texture(w, h, px);
    mv.ready = 1;
}

int movie_server(void)
{
    double t;
    int want, n = 0, got = 0;
    if (!mv.s || !mv.playing) {
        /* no movie (RT_NOMOVIE, no AFS00): the opening demo's own timer
         * waits for the movie, so end that wait at once */
        if (!mv.s && rt_boot_active() && demo_w[0] == 5 && demo_w[1] == 2)
            *(s16 *)(demo_w + 0xA) = 0;
        return 0;
    }
    mv.ticks++;
    if (getenv("RT_MOVIE_TRACE") && mv.ticks % 600 == 0) {
        int fr;
        double tot, mx;
        sfd_stats(mv.s, &fr, &tot, &mx);
        fprintf(stderr, "movie: tick %ld frame %d, audio %.2f s, wall %.2f s, decode %.2f ms/frame (worst %.1f ms)\n", mv.ticks, fr,
                (double)audio_stream_consumed(AUDIO_STREAM_MOVIE) / AUDIO_RATE, wall_s() - mv.wall0, tot / (fr ? fr : 1), mx);
    }
    audio_feed();
    t = clock_now();
    want = (int)(t * sfd_fps(mv.s)) + 1;
    if (mv.ticks == 1)
        want = 1;
    while (!mv.ended && sfd_frames(mv.s) < want && n < 4) {
        if (!sfd_next_frame(mv.s))
            mv.ended = 1;
        else
            got = 1;
        n++;
    }
    if (got) {
        upload();
        if (getenv("RT_MOVIE_DUMP")) {          /* RT_MOVIE_DUMP=dir: every 100th frame as PPM */
            int w, h;
            if (sfd_frames(mv.s) % 100 == 1) {
                const uint8_t *px = sfd_rgba(mv.s, &w, &h);
                char name[600];
                FILE *f;
                int i;
                snprintf(name, sizeof name, "%s/m%d_%05d.ppm", getenv("RT_MOVIE_DUMP"), mv.no, sfd_frames(mv.s));
                f = fopen(name, "wb");
                if (f) {
                    fprintf(f, "P6 %d %d 255\n", w, h);
                    for (i = 0; i < w * h; i++)
                        fwrite(px + 4 * i, 1, 3, f);
                    fclose(f);
                }
            }
        }
    }
    return mv.ready;
}

void movie_draw(void)
{
    float pos[12], st[12];
    float x0, y0, x1, y1, u0, u1, v0, v1;
    int i;
    static const int order[6] = { 0, 1, 2, 2, 1, 3 };
    float cx[4], cy[4], cu[4], cv[4];
    if (!mv.s || !mv.ready || !mv.tex[mv.ti])
        return;
    /* f_movie's movie_draw: the picture fills the 512 x 448 screen (the
     * 256-wide texture is stretched 2x); the sp_mh movie (8) is drawn
     * at 512/320 of its own size */
    if (mv.no == 8) {
        x0 = -1; y0 = 1;
        x1 = x0 + (float)((mv.tw << 9) / 320); y1 = y0 + (float)(mv.th - 1);
        u0 = 0; u1 = (float)mv.tw; v0 = 1; v1 = (float)(mv.th - 1);
    } else {
        x0 = -1; y0 = 0;
        x1 = x0 + (float)(mv.tw * 2); y1 = 448;
        u0 = 0; u1 = (float)mv.tw; v0 = 32; v1 = 480;
    }
    cx[0] = x0; cy[0] = y0; cu[0] = u0 / mv.tw; cv[0] = v0 / mv.th;
    cx[1] = x1; cy[1] = y0; cu[1] = u1 / mv.tw; cv[1] = v0 / mv.th;
    cx[2] = x0; cy[2] = y1; cu[2] = u0 / mv.tw; cv[2] = v1 / mv.th;
    cx[3] = x1; cy[3] = y1; cu[3] = u1 / mv.tw; cv[3] = v1 / mv.th;
    for (i = 0; i < 6; i++) {
        pos[2 * i] = cx[order[i]];
        pos[2 * i + 1] = cy[order[i]];
        st[2 * i] = cu[order[i]];
        st[2 * i + 1] = cv[order[i]];
    }
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)mv.tex[mv.ti]);
    gfx_set_render_state(GFX_RS_FILTER, 0);
    gfx_set_render_state(GFX_RS_BLEND, 0);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 1);
    gfx_draw_2d(512, 448, 6, pos, st, NULL);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 0);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    rt_2d_restore_texture();
}
