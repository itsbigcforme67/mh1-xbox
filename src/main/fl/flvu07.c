/* fl library VU0 macro-mode routines (SLPM_654.95 0x173680-0x1736A8): flSqrt.
 * Written as MWCC inline assembly (the original is VU0 inline asm), one routine per function. */
#ifdef __MWERKS__

asm void flSqrt(void)
{
    mfc1 t0, f12
    nop
    qmtc2.ni t0, vf4
    vsqrt Q, vf4x
    vwaitq
    vaddq.x vf4, vf0, Q
    qmfc2.ni t0, vf4
    mtc1 t0, f0
    jr ra
    nop
}

#endif
