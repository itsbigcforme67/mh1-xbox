/* rt_vu.h - the PS2's VU0 math rounds differently from its FPU: the referee (PCSX2, docs/referee.md) shows VU0 results
 * rounded to nearest while the game's FPU code rounds toward zero (RT_FPU_PS2, rt_ref.c). The host functions that
 * stand in for VU0 library code (the fl matrix / vector helpers, the sine / cosine, the motion curves) switch the SSE
 * rounding to nearest for their duration with these. No-ops off x86. Agent D, 20 Oct 2026. */
#ifndef RT_VU_H
#define RT_VU_H
#if defined(__i386__) || defined(__x86_64__)
static inline unsigned int rt_vu_begin(void)
{
    unsigned int o, n;
    __asm__ volatile("stmxcsr %0" : "=m"(o));
    n = o & ~0x6000u;
    if (n != o)
        __asm__ volatile("ldmxcsr %0" : : "m"(n));
    return o;
}
static inline void rt_vu_end(unsigned int o)
{
    unsigned int n;
    __asm__ volatile("stmxcsr %0" : "=m"(n));
    if ((n & 0x6000u) != (o & 0x6000u)) {
        n = (n & ~0x6000u) | (o & 0x6000u);     /* keep the flag bits the code set */
        __asm__ volatile("ldmxcsr %0" : : "m"(n));
    }
}
#else
static inline unsigned int rt_vu_begin(void) { return 0; }
static inline void rt_vu_end(unsigned int o) { (void)o; }
#endif
#endif
