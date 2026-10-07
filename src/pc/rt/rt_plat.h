/* rt_plat.h - which platform the host-side C is built for.
 *   MH1_XBOX: the original Xbox (nxdk's clang targets win32 but has no mingw);
 *   MH1_WINDOWS: the Windows build (tools/build_win.sh, llvm-mingw; it passes -DMH1_WIN);
 * else Linux (or another POSIX system). */
#ifndef RT_PLAT_H
#define RT_PLAT_H
#if defined(_WIN32) && defined(MH1_WIN)
#define MH1_WINDOWS 1
#elif defined(_WIN32)
#define MH1_XBOX 1
#endif
#endif
