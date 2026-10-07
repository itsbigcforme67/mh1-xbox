/* fl library VU0 macro-mode routines (SLPM_654.95 0x1730F0-0x1732F4): flvecCalcLength, flvecCalcDistance, flvecNormalize, flvecInnerProduct, flvecOuterProduct.
 * Written as MWCC inline assembly (the original is VU0 inline asm), one routine per function. */
#ifdef __MWERKS__

asm void flvecCalcLength(void)
{
    lwc1 f2, 0x0(a0)
    addiu sp, sp, -0x10
    lwc1 f1, 0x4(a0)
    addiu v0, sp, 0x0
    lwc1 f0, 0x8(a0)
    swc1 f2, 0x0(v0)
    swc1 f1, 0x4(v0)
    swc1 f0, 0x8(v0)
    lqc2 vf4, 0x0(v0)
    vmul.xyz vf4, vf4, vf4
    vaddy.x vf4, vf4, vf4y
    vaddz.x vf4, vf4, vf4z
    vsqrt Q, vf4x
    vwaitq
    vaddq.x vf4, vf0, Q
    qmfc2.ni v0, vf4
    mtc1 v0, f0
    jr ra
    addiu sp, sp, 0x10
}

asm void flvecCalcDistance(void)
{
    lwc1 f2, 0x0(a0)
    addiu sp, sp, -0x20
    lwc1 f1, 0x4(a0)
    addiu v1, sp, 0x10
    lwc1 f0, 0x8(a0)
    addiu v0, sp, 0x0
    swc1 f2, 0x0(v1)
    swc1 f1, 0x4(v1)
    swc1 f0, 0x8(v1)
    lwc1 f2, 0x0(a1)
    lwc1 f1, 0x4(a1)
    lwc1 f0, 0x8(a1)
    swc1 f2, 0x0(v0)
    swc1 f1, 0x4(v0)
    swc1 f0, 0x8(v0)
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x0(v0)
    vsub.xyz vf4, vf4, vf5
    vmul.xyz vf4, vf4, vf4
    vaddy.x vf4, vf4, vf4y
    vaddz.x vf4, vf4, vf4z
    vsqrt Q, vf4x
    vwaitq
    vaddq.x vf4, vf0, Q
    qmfc2.ni v1, vf4
    mtc1 v1, f0
    jr ra
    addiu sp, sp, 0x20
}

asm void flvecNormalize(void)
{
    lwc1 f2, 0x0(a0)
    addiu sp, sp, -0x10
    lwc1 f1, 0x4(a0)
    addiu v1, sp, 0x0
    lwc1 f0, 0x8(a0)
    swc1 f2, 0x0(v1)
    swc1 f1, 0x4(v1)
    swc1 f0, 0x8(v1)
    lqc2 vf4, 0x0(v1)
    vmul.xyz vf5, vf4, vf4
    vaddy.x vf5, vf5, vf5y
    vaddz.x vf5, vf5, vf5z
    vrsqrt Q, vf0w, vf5x
    vwaitq
    vmulq.xyz vf4, vf4, Q
    sqc2 vf4, 0x0(v1)
    addiu v1, sp, 0x0
    lwc1 f2, 0x0(v1)
    lwc1 f1, 0x4(v1)
    lwc1 f0, 0x8(v1)
    swc1 f2, 0x0(a0)
    swc1 f1, 0x4(a0)
    swc1 f0, 0x8(a0)
    jr ra
    addiu sp, sp, 0x10
}

asm void flvecInnerProduct(void)
{
    lwc1 f2, 0x0(a0)
    addiu sp, sp, -0x20
    lwc1 f1, 0x4(a0)
    addiu v1, sp, 0x10
    lwc1 f0, 0x8(a0)
    addiu v0, sp, 0x0
    swc1 f2, 0x0(v1)
    swc1 f1, 0x4(v1)
    swc1 f0, 0x8(v1)
    lwc1 f2, 0x0(a1)
    lwc1 f1, 0x4(a1)
    lwc1 f0, 0x8(a1)
    swc1 f2, 0x0(v0)
    swc1 f1, 0x4(v0)
    swc1 f0, 0x8(v0)
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x0(v0)
    vmul.xyz vf4, vf4, vf5
    vaddy.x vf4, vf4, vf4y
    vaddz.x vf4, vf4, vf4z
    qmfc2.ni v1, vf4
    mtc1 v1, f0
    jr ra
    addiu sp, sp, 0x20
}

asm void flvecOuterProduct(void)
{
    lwc1 f2, 0x0(a1)
    addiu sp, sp, -0x20
    lwc1 f1, 0x4(a1)
    addiu a3, sp, 0x10
    lwc1 f0, 0x8(a1)
    addiu v1, sp, 0x0
    swc1 f2, 0x0(a3)
    swc1 f1, 0x4(a3)
    swc1 f0, 0x8(a3)
    lwc1 f2, 0x0(a2)
    lwc1 f1, 0x4(a2)
    lwc1 f0, 0x8(a2)
    swc1 f2, 0x0(v1)
    swc1 f1, 0x4(v1)
    swc1 f0, 0x8(v1)
    lqc2 vf4, 0x0(a3)
    lqc2 vf5, 0x0(v1)
    vopmula.xyz ACC, vf4, vf5
    vopmsub.xyz vf6, vf5, vf4
    sqc2 vf6, 0x0(a3)
    addiu v1, sp, 0x10
    lwc1 f2, 0x0(v1)
    lwc1 f1, 0x4(v1)
    lwc1 f0, 0x8(v1)
    swc1 f2, 0x0(a0)
    swc1 f1, 0x4(a0)
    swc1 f0, 0x8(a0)
    jr ra
    addiu sp, sp, 0x20
}

#endif
