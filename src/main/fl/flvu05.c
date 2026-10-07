/* fl library VU0 macro-mode routines (SLPM_654.95 0x172D30-0x172FE8): flmatBlend, flvecApplyMat33, flvecApplyMat33_2, flvecApplyMat, flvecApplyMatTrans.
 * Written as MWCC inline assembly (the original is VU0 inline asm), one routine per function. */
#ifdef __MWERKS__

asm void flmatBlend(void)
{
    vmr32.xyzw vf6, vf0
    vmove.xyzw vf7, vf0
    vmr32.xyzw vf5, vf6
    vmr32.xyzw vf4, vf5
    mfc1 t0, f12
    nop
    qmtc2.ni t0, vf8
    mfc1 t0, f13
    nop
    qmtc2.ni t0, vf9
    lqc2 vf10, 0x0(a1)
    lqc2 vf11, 0x0(a2)
    lqc2 vf12, 0x10(a1)
    lqc2 vf13, 0x10(a2)
    lqc2 vf14, 0x30(a1)
    lqc2 vf15, 0x30(a2)
    vmulax.xyz ACC, vf10, vf8x
    vmaddx.xyz vf10, vf11, vf9x
    vmulax.xyz ACC, vf12, vf8x
    vmaddx.xyz vf12, vf13, vf9x
    vmulax.xyz ACC, vf14, vf8x
    vmaddx.xyz vf7, vf15, vf9x
    vmul.xyz vf4, vf10, vf10
    vaddy.x vf4, vf4, vf4y
    vaddz.x vf4, vf4, vf4z
    vrsqrt Q, vf0w, vf4x
    vwaitq
    vmulq.xyz vf4, vf10, Q
    vmul.xyz vf5, vf12, vf12
    vaddy.x vf5, vf5, vf5y
    vaddz.x vf5, vf5, vf5z
    vrsqrt Q, vf0w, vf5x
    vwaitq
    vmulq.xyz vf5, vf12, Q
    vopmula.xyz ACC, vf4, vf5
    vopmsub.xyz vf6, vf5, vf4
    vmul.xyz vf8, vf6, vf6
    vaddy.x vf8, vf8, vf8y
    vaddz.x vf8, vf8, vf8z
    vrsqrt Q, vf0w, vf8x
    vwaitq
    vmulq.xyz vf6, vf6, Q
    vopmula.xyz ACC, vf6, vf4
    vopmsub.xyz vf5, vf4, vf6
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    sqc2 vf6, 0x20(a0)
    jr ra
    sqc2 vf7, 0x30(a0)
}

asm void flvecApplyMat33(void)
{
    lwc1 f2, 0x0(a1)
    addiu sp, sp, -0x10
    lwc1 f1, 0x4(a1)
    addiu v1, sp, 0x0
    lwc1 f0, 0x8(a1)
    swc1 f2, 0x0(v1)
    swc1 f1, 0x4(v1)
    swc1 f0, 0x8(v1)
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x0(a2)
    lqc2 vf6, 0x10(a2)
    lqc2 vf7, 0x20(a2)
    vmulax.xyz ACC, vf5, vf4x
    vmadday.xyz ACC, vf6, vf4y
    vmaddz.xyz vf4, vf7, vf4z
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

asm void flvecApplyMat33_2(void)
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
    lqc2 vf5, 0x0(a1)
    lqc2 vf6, 0x10(a1)
    lqc2 vf7, 0x20(a1)
    vmulax.xyz ACC, vf5, vf4x
    vmadday.xyz ACC, vf6, vf4y
    vmaddz.xyz vf4, vf7, vf4z
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

asm void flvecApplyMat(void)
{
    lwc1 f3, 0x0(a1)
    addiu sp, sp, -0x10
    lwc1 f2, 0x4(a1)
    addiu v1, sp, 0x0
    lwc1 f1, 0x8(a1)
    lwc1 f0, 0xC(a1)
    swc1 f3, 0x0(v1)
    swc1 f2, 0x4(v1)
    swc1 f1, 0x8(v1)
    swc1 f0, 0xC(v1)
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x0(a2)
    lqc2 vf6, 0x10(a2)
    lqc2 vf7, 0x20(a2)
    lqc2 vf8, 0x30(a2)
    vmulax.xyzw ACC, vf5, vf4x
    vmadday.xyzw ACC, vf6, vf4y
    vmaddaz.xyzw ACC, vf7, vf4z
    vmaddw.xyzw vf4, vf8, vf4w
    sqc2 vf4, 0x0(v1)
    addiu v1, sp, 0x0
    lwc1 f3, 0x0(v1)
    lwc1 f2, 0x4(v1)
    lwc1 f1, 0x8(v1)
    lwc1 f0, 0xC(v1)
    swc1 f3, 0x0(a0)
    swc1 f2, 0x4(a0)
    swc1 f1, 0x8(a0)
    swc1 f0, 0xC(a0)
    jr ra
    addiu sp, sp, 0x10
}

asm void flvecApplyMatTrans(void)
{
    lwc1 f0, 0x0(a1)
    addiu sp, sp, -0x10
    lui v1, (0x3F800000 >> 16)
    addiu a3, sp, 0x0
    swc1 f0, 0x0(sp)
    lwc1 f0, 0x4(a1)
    swc1 f0, 0x4(sp)
    lwc1 f0, 0x8(a1)
    swc1 f0, 0x8(sp)
    sw v1, 0xC(sp)
    lqc2 vf4, 0x0(a3)
    lqc2 vf5, 0x0(a2)
    lqc2 vf6, 0x10(a2)
    lqc2 vf7, 0x20(a2)
    lqc2 vf8, 0x30(a2)
    vmulax.xyzw ACC, vf5, vf4x
    vmadday.xyzw ACC, vf6, vf4y
    vmaddaz.xyzw ACC, vf7, vf4z
    vmaddw.xyzw vf4, vf8, vf4w
    vdiv Q, vf0w, vf4w
    vwaitq
    vmulq.xyz vf4, vf4, Q
    sqc2 vf4, 0x0(a3)
    addiu v1, sp, 0x0
    lwc1 f3, 0x0(v1)
    lwc1 f2, 0x4(v1)
    lwc1 f1, 0x8(v1)
    lwc1 f0, 0xC(v1)
    swc1 f3, 0x0(a0)
    swc1 f2, 0x4(a0)
    swc1 f1, 0x8(a0)
    swc1 f0, 0xC(a0)
    jr ra
    addiu sp, sp, 0x10
}

#endif
