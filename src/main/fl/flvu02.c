/* fl library VU0 macro-mode routines (SLPM_654.95 0x171CE0-0x171E50): flmatInit, flmatInit33, flmatMakeScale, flmatMakeScale33, flmatMakeTrans.
 * Written as MWCC inline assembly (the original is VU0 inline asm), one routine per function. */
#ifdef __MWERKS__
extern char flPS2INITMATRIX[];

asm void flmatInit(void)
{
    la v1, flPS2INITMATRIX
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x10(v1)
    lqc2 vf6, 0x20(v1)
    lqc2 vf7, 0x30(v1)
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    sqc2 vf6, 0x20(a0)
    jr ra
    sqc2 vf7, 0x30(a0)
}

asm void flmatInit33(void)
{
    la v1, flPS2INITMATRIX
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x10(v1)
    lqc2 vf6, 0x20(v1)
    lqc2 vf8, 0x0(a0)
    lqc2 vf9, 0x10(a0)
    lqc2 vf10, 0x20(a0)
    vadd.xyz vf8, vf0, vf4
    vadd.xyz vf9, vf0, vf5
    vadd.xyz vf10, vf0, vf6
    sqc2 vf8, 0x0(a0)
    sqc2 vf9, 0x10(a0)
    jr ra
    sqc2 vf10, 0x20(a0)
}

asm void flmatMakeScale(void)
{
    la v1, flPS2INITMATRIX
    mfc1 a2, f12
    mfc1 a3, f13
    mfc1 t0, f14
    lqc2 vf4, 0x0(v1)
    qmtc2.ni a2, vf8
    qmtc2.ni a3, vf9
    qmtc2.ni t0, vf10
    lqc2 vf5, 0x10(v1)
    lqc2 vf6, 0x20(v1)
    lqc2 vf7, 0x30(v1)
    vaddx.x vf4, vf0, vf8x
    vaddx.y vf5, vf0, vf9x
    vaddx.z vf6, vf0, vf10x
    sqc2 vf7, 0x30(a0)
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    jr ra
    sqc2 vf6, 0x20(a0)
}

asm void flmatMakeScale33(void)
{
    la v1, flPS2INITMATRIX
    mfc1 a2, f12
    mfc1 a3, f13
    mfc1 t0, f14
    lqc2 vf4, 0x0(v1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    qmtc2.ni t0, vf22
    lqc2 vf5, 0x10(v1)
    lqc2 vf6, 0x20(v1)
    lqc2 vf8, 0x0(a0)
    lqc2 vf9, 0x10(a0)
    lqc2 vf10, 0x20(a0)
    vaddx.x vf4, vf0, vf20x
    vaddx.y vf5, vf0, vf21x
    vaddx.z vf6, vf0, vf22x
    vaddx.w vf4, vf8, vf0x
    vaddx.w vf5, vf9, vf0x
    vaddx.w vf6, vf10, vf0x
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    jr ra
    sqc2 vf6, 0x20(a0)
}

asm void flmatMakeTrans(void)
{
    la v1, flPS2INITMATRIX
    mfc1 a2, f12
    mfc1 a3, f13
    mfc1 t0, f14
    lqc2 vf7, 0x30(v1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    qmtc2.ni t0, vf22
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x10(v1)
    lqc2 vf6, 0x20(v1)
    vaddx.x vf7, vf0, vf20x
    vaddx.y vf7, vf0, vf21x
    vaddx.z vf7, vf0, vf22x
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    sqc2 vf6, 0x20(a0)
    jr ra
    sqc2 vf7, 0x30(a0)
}

#endif
