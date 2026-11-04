# Real Xbox: cold power-on black screen (AC97 codec_setup), research 4 Nov 2026

Research agent, logs and code only (no Xbox, no xemu, nothing built). Sources: build/xbox/logs/ (99 logs returned by the
loader, 11 Oct - 1 Nov), build/bench/coldloop.log + coldloop2.log, src/pc/xbox/xbox_ac97.c and its git history
(05a2dc6f, e525db91, cf3c27f8, 9db224d2, fe7b43eb), docs/xbox.md, docs/agents/COORDINATOR.md, docs/agents/agent-F.md,
nxdk's own driver /home/james/xboxdev/nxdk/lib/hal/audio.c and nxdk's SDL2 Xbox audio driver.
Tags: [log] read in a log, [code] read in the source, [guess] my inference, not checked.

## 1. What the logs show

**The hang.** 20261029-201017-mhg (build compiled 29 Oct 19:49) ends at `ac97: init: codec ready, setting up (GLOB_STA
00300100)` [log]. In that build codec_setup was: write reg 26 = 0, then a loop that read reg 26 until its low 4 ready bits
were set, with a 1 ms sleep, up to 200 ms; then read reg 2A, and five writes [code, 9db224d2^]. The next logged step,
`init: bus masters`, never came. So the hang is somewhere between that commit and the next STEP's commit: inside codec_setup
(about 100-200 codec reads plus 6 writes) or, less likely, in the next STEP's own rt_log / disk commit. The log has no
`watchdog:` or `FREEZE` line at all [log]; every good start of that build logs `watchdog: longest wait during the boot`.

**Codec reads always give 0000.** All 48 `ac97 open: codec ...` lines in all logs (13 Oct - 29 Oct, both builds with reads)
show 00, 02, 18, 26, 2A, 2C = 0000, including reg 00 (reset / capabilities) and registers just written (18 = 0808) [log].
In the traced build (20:12) `codec rd 26 = 0000`, `codec rd 2A = 0000`, every semaphore "taken", never timed out [log].
GLOB_STA bit 15 is never set in a good start [log]. That fits Cromwell's note quoted at the top of nxdk's audio.c: the Xbox's
Wolfson WM9709 is a "dumb" codec with no PC-controllable registers (only ID registers). Reads complete, but carry no data.

**Controller state at entry.** All 58 `ac97: init:` lines: GLOB_CNT 00000002, GLOB_STA 00300100 (codec ready already set
by whatever ran before), both DMA engines halted (sr 0001) [log]. So the codec is up before we touch it, on cold and warm
starts alike (when the start succeeded).

**The traced build did not test the same thing.** Each STEP (log line + commit of two files to the disk) costs 78-100 ms
on hardware [log: spacing of consecutive `ac97: init:` lines in every build]. In the 20:12 build every codec access was a
STEP pair, so codec_setup's 200 ms deadline (counted from the function's start) had passed after the first read: the
loop read reg 26 exactly once in all 15 logs of that build (4 `codec rd` lines each = rd 26 + rd 2A) [log]. That build
made about 8 codec reads per start (2 in setup + 6 in the open dump) instead of ~100-200. Its 10/10 (coldloop.log) says
"8 reads per start, slowly spaced, did not hang", not "the loop is safe".

**Reads per start, by build** [code + log]:

| builds | codec reads per start | starts with a log | hung at the audio init |
|---|---|---|---|
| 13 Oct 05a2dc6f - 29 Oct 19:49 | ~50-200 (loop) + 6 (dump) | ~33 | 2 named (21 Oct 190054 warm relaunch, at the dump's first read, GLOB_STA 00308000; 29 Oct 201017 cold, in codec_setup); 2 more on 21 Oct (200347, 200821) end before `audio: driver` in builds without step commits, position unknown |
| 29 Oct 20:12 (trace) | 8 | 15 (10 in coldloop) | 0 |
| 29 Oct 21:01 fe7b43eb and later | 0 (writes only) | 24 (15 in coldloop2, 9 later 30 Oct - 1 Nov) | 0 |

(The 21 Oct 190054 case is a different state: codec ready missing after a Back+Start relaunch, and bit 15 already set. In
the Intel ICH layout, which the MCPX's ACI seems to follow for these registers, bit 15 of GLOB_STA is "codec read timed
out" [guess for the MCPX]. That run shows a read can time out cleanly and a later read still stall.)

20261031-032526 stops 8.8 s in, after `memory card: loaded`, well past the audio init: not this bug (probably switched
off) [log].

## 2. Likely causes, ranked

1. **A codec register READ that never completes stalls the CPU** [guess, best fit]. Evidence: both hangs that name a place
   are at or inside a run of codec reads; the reads return nothing useful anyway; nxdk's driver (which the owner heard
   play correctly on 13 Oct) never reads a codec register; xemu also hangs on a codec register access. A stalled MMIO
   read freezes the whole CPU, so the watchdog thread can't run either, which matches the missing FREEZE line (but see
   test 4: the bench power-off at 45 s comes before the watchdog's 60 s boot limit, so the missing line proves nothing
   yet). With ~100-200 reads per start and 1 hang in ~9-25 starts, the odds per read would be about 1 in 1000-5000.
2. **A codec access issued while the previous one is still in flight** [guess]. cas_wait gives up after 20 ms but the
   access is then made anyway [code: codec_wr ignores cas_wait's result, and codec_rd did too]. In the old loop a read
   followed the reg 26 write (which powers parts of the codec up) within microseconds. The trace never saw a time-out,
   but its accesses were 90 ms apart.
3. **Codec writes** (still done now: 26, 2A, 02, 04, 06, 18 after each cold reset) [guess, lower]. Writes are usually
   posted and less likely to hold the bus, but they use the same AC-link slot mechanism. If the codec really has no
   writable registers they do nothing useful.
4. **The cold reset we add** [guess, low]. The codec is already ready at entry; we drop it into reset for 1 ms (nxdk: about
   1 us) and access it right after GLOB_STA says ready again. If "ready" is reported before the codec's link is fully up,
   the first access lands early. Every log shows ready within one STEP, so the wait itself is short.
5. **The disk commit of the next STEP** [guess, low]. Possible in principle (a FATX write at boot), but commits happen
   dozens of times per start in every build and never hung elsewhere.

## 3. Is the 29 Oct change a real fix or luck?

Probably real, not yet proven. Removing the reads removes cause 1 and the read half of cause 2 entirely, and the logs point
at reads. Numbers: before, 2 named hangs in ~33 logged starts (~6%; the owner's figures were 1 in ~9 that day, 1 in ~25
overall). If nothing had changed, 0 hangs in the 24 later starts has a chance of 0.94^24 = ~23% (at 1 in 25: ~38%);
counting the 15 trace starts as well (they read only 8 times), ~9% (at 1 in 25: ~20%). So the result is encouraging but
within luck. About 60-75 more cold starts with no hang are needed before luck drops under 5% at the 1-in-25 rate.

Also: the warm-relaunch hang (21 Oct, codec ready missing) is handled by a different path (second cold reset, then silent
and no codec access) that has not been seen on hardware yet [log: no `no codec ready` line in any log].

## 4. Code changes to try (not made; for the coordinator)

1. **No codec register access by default**, as nxdk does: skip codec_setup and its 200 ms sleep unless an env switch asks
   for it (RT_AC97_CODEC=1; RT_AC97_NOCODEC=1 already does the skip today). Needs one listening check that sound still
   plays (the owner heard nxdk's no-codec driver on 13 Oct, so expected yes). Saves 0.2 s of boot too.
2. If writes stay: **skip the access when cas_wait times out** (and warn once), instead of making it anyway.
3. Log GLOB_STA once after codec_setup / before `bus masters` (no codec access needed): bit 15 shows whether any access
   timed out.
4. Optional, separately tested: on a start that finds codec ready set and both DMA engines halted (every logged cold start),
   skip our cold reset; keep it for the warm relaunch case (ready missing). nxdk always resets, so this is the riskier one.
5. Watchdog: let a stuck AC97 init be reported quickly (e.g. a 5 s limit from the "audio: AC97 start" mark to the end of
   XAudioInit). If a hang then still shows no FREEZE screen, the CPU itself is stalled (a bus access), which confirms 1/2.

## 5. Bench tests for the owner (in order)

1. Current build, `tools/bench/coldloop.sh 30` twice (different sessions / after the console has cooled): goal 60+ cold
   starts with no black screen. Mix in 5-10 Back+Start relaunches (the warm path).
2. In coldloop.sh, on a black screen wait 75 s before pressing the button and take a second shot: a "froze" screen means a
   spinning wait (watchdog alive), still black means a CPU / bus stall.
3. With RT_AC97_NOCODEC=1 in env.txt: 20-30 cold starts plus one listen to the title music and an effect (the capture card
   carries no sound: by ear or a phone microphone).
4. Only if proof of the mechanism is wanted: a test build whose codec_setup reads reg 26 ~2000 times (no commit per read;
   a counter committed every 200 reads). If cold starts then hang often and the count stops mid-run, cause 1 is confirmed.
   This costs black screens and power cycles, so only if the owner wants the certainty.

## 6. Music static (open on hardware) - what the logs say

- The report (docs/RELEASE_G.md GB5 item 14, owner 17 Oct): entering the house turned the music into static and it stayed
  static after going back out; sound effects fine. The 17 Oct logs (20261017-145135, -150532) show the village <-> house
  stage changes (87 <-> 86) with no warning at those points; the only audio warnings are `audio underrun: 1 late callbacks`
  with gaps of 2-80 s, all next to loads or the bug report's deliberate freeze, and `audio late` stays at 0-5 of thousands
  of callbacks in every session [log]. Nothing in the logs explains it.
- Not the AC97 path [guess, strong]: effects and music are mixed into the same buffers (src/pc/audio/audio_mix.c) and go
  through one DMA channel, and effects stay clean. The fault is in the music path: the ADX stream (rt_snd.c str_start /
  str_feed, fmt/snd.c fmt_adx_header / fmt_adx_row) or the stream ring in audio_mix.c.
- Since it persists across a new stream (back outside = a fresh str_start with the history cleared), something lasting is
  suspect rather than one stream's state [guess]. One candidate: the predictor coefficients c1/c2 are computed at every
  stream start with cos/sqrt in double precision (fmt_adx_header); if the x87 state or nxdk's libm gives a wrong result on
  the Xbox at that point, every later stream decodes as noise, while effects (VAG, fixed filter tables) stay clean. Cheap
  check: log c1/c2, rate and channel count at each str_start on the Xbox (RT_SND trace or one rt_log line) and compare with
  the PC for the same stream. Also confirm the PC plays the house music cleanly.
- Fog and the HDD launch: nothing about either in these logs beyond what docs/xbox.md already says.
