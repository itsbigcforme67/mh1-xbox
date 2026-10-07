/*
 * rt_log.h - the automatic debug log (rt_log.c): one file per run in a logs
 * folder next to the save directory, nothing sent anywhere.
 */
#ifndef RT_LOG_H
#define RT_LOG_H

#ifdef __GNUC__
#define RT_LOG_PRINTF(a, b) __attribute__((format(printf, a, b)))
#else
#define RT_LOG_PRINTF(a, b)
#endif

/* Open the log (after the platform's drives are mounted), write the header's
 * first lines and install the crash handler. disc = the disc directory. */
void rt_log_init(const char *disc, int argc, char **argv);
void rt_log_shutdown(void);
int rt_log_started(void);       /* rt_log_init has run */

/* an event / a header line */
void rt_log(const char *fmt, ...) RT_LOG_PRINTF(1, 2);
/* a warning (flushed at once) */
void rt_warn(const char *fmt, ...) RT_LOG_PRINTF(1, 2);
/* a warning that is written the first time only per key (same key text) */
void rt_warn_once(const char *key, const char *fmt, ...) RT_LOG_PRINTF(2, 3);
/* a no-op stand-in function was called: one line the first time per name */
void rt_log_standin(const char *name);
/* a host path for the log: $HOME / the user's profile folder replaced by ~ */
const char *rt_log_path(const char *p);

/* the game's state, called every game tick: logs mode / stage / quest changes
 * (quest start, clear, failure) and keeps the crash report's context */
void rt_log_game(int mode, int step, int stage, int quest, int result);
void rt_log_boot_tick(void);
/* every drawn frame: timed flush, the once-a-minute fps / CPU line */
void rt_log_frame(void);
/* the audio device callback measures itself (any thread); the main loop prints it */
void rt_log_audio_cb(double gap_ms, double work_ms, int period_ms);

#endif
