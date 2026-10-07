/* fl library (SLPM_654.95 0x172CB0-0x172CD4): flmatCopy, a 4x4 matrix copy with four quadword loads/stores.
 * Written as MWCC inline assembly like its neighbours (C with u128 gives the wrong register order). */
#ifdef __MWERKS__

asm void flmatCopy(void)
{
    lq a2, 0x0(a1)
    lq a3, 0x10(a1)
    lq t0, 0x20(a1)
    lq t1, 0x30(a1)
    sq a2, 0x0(a0)
    sq a3, 0x10(a0)
    sq t0, 0x20(a0)
    jr ra
    sq t1, 0x30(a0)
}

#endif
