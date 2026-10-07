/* fl library VU0 macro-mode routines (SLPM_654.95 0x1710E0-0x1715E0): flFCVFcurveInterpolateLinear, flFCVFcurveInterpolateHermite, flPS2SinFast, flPS2CosFast, flPS2SinCosFast.
 * Written as MWCC inline assembly (the original is VU0 inline asm), one routine per function. */
#ifdef __MWERKS__
extern char SS5432[];
extern char at_tbl[];

asm void flFCVFcurveInterpolateLinear(void)
{
    mfc1 t0, f12
    nop
    qmtc2.ni t0, vf3
    mfc1 t0, f14
    nop
    qmtc2.ni t0, vf4
    mfc1 t0, f16
    nop
    qmtc2.ni t0, vf5
    mfc1 t0, f13
    nop
    qmtc2.ni t0, vf6
    mfc1 t0, f15
    nop
    qmtc2.ni t0, vf7
    vsub.x vf8, vf5, vf4
    vdiv Q, vf0w, vf8x
    vsub.x vf9, vf7, vf6
    vmul.x vf10, vf5, vf6
    vmul.x vf11, vf4, vf7
    vsub.x vf10, vf10, vf11
    vwaitq
    vmulq.x vf9, vf9, Q
    vmulq.x vf10, vf10, Q
    vmul.x vf4, vf9, vf3
    vadd.x vf4, vf4, vf10
    qmfc2.ni t0, vf4
    mtc1 t0, f0
    jr ra
    nop
}

asm void flFCVFcurveInterpolateHermite(void)
{
    mfc1 t0, f12
    nop
    qmtc2.ni t0, vf3
    mfc1 t0, f14
    nop
    qmtc2.ni t0, vf4
    mfc1 t0, f17
    nop
    qmtc2.ni t0, vf5
    mfc1 t0, f13
    nop
    qmtc2.ni t0, vf6
    mfc1 t0, f16
    nop
    qmtc2.ni t0, vf7
    mfc1 t0, f15
    nop
    qmtc2.ni t0, vf8
    mfc1 t0, f18
    nop
    qmtc2.ni t0, vf2
    vsub.x vf9, vf3, vf4
    vsub.x vf10, vf5, vf4
    vdiv Q, vf0w, vf10x
    vwaitq
    vaddq.x vf10, vf0, Q
    vmul.x vf11, vf10, vf10
    vmul.x vf12, vf9, vf9
    vmul.x vf13, vf12, vf10
    vmul.x vf14, vf12, vf9
    vmul.x vf14, vf14, vf11
    vaddw.x vf1, vf0, vf0w
    vadd.x vf1, vf1, vf1
    vmul.x vf15, vf1, vf14
    vmul.x vf15, vf15, vf10
    vaddw.x vf1, vf1, vf0w
    vmul.x vf16, vf1, vf12
    vmul.x vf16, vf16, vf11
    vsub.x vf17, vf15, vf16
    vaddw.x vf17, vf17, vf0w
    vmul.x vf17, vf6, vf17
    vsub.x vf18, vf16, vf15
    vmul.x vf18, vf7, vf18
    vadd.x vf17, vf17, vf18
    vsub.x vf18, vf14, vf13
    vsub.x vf18, vf18, vf13
    vadd.x vf18, vf18, vf9
    vmul.x vf18, vf8, vf18
    vadd.x vf17, vf17, vf18
    vsub.x vf18, vf14, vf13
    vmul.x vf18, vf2, vf18
    vadd.x vf17, vf17, vf18
    qmfc2.ni t0, vf17
    mtc1 t0, f0
    jr ra
    nop
}

asm void flPS2SinFast(void)
{
    lui v0, (0xC0490FDB >> 16)
    ori v0, v0, (0xC0490FDB & 0xFFFF)
    mtc1 v0, f0
    nop
    c.le.s f12, f0
    bc1t L00171278
    lui v0, (0x40490FDB >> 16)
    ori v0, v0, (0x40490FDB & 0xFFFF)
    mtc1 v0, f0
    nop
    c.lt.s f12, f0
    bc1t L001712C8
    nop
    nop
L00171278:
    la v0, at_tbl
    mfc1 t0, f12
    lqc2 vf6, 0x0(v0)
    qmtc2.ni t0, vf4
    slt t0, zero, t0
    ctc2.ni t0, vi2
    vmulx.x vf7, vf4, vf6x
    vftoi0.x vf7, vf7
    vmtir vi1, vf7x
    viaddi vi3, vi0, -0x2
    viadd vi1, vi1, vi2
    viand vi1, vi1, vi3
    vmfir.x vf7, vi1
    vitof0.x vf7, vf7
    vmuly.x vf7, vf7, vf6y
    vsub.x vf7, vf4, vf7
    qmfc2.ni t0, vf7
    mtc1 t0, f12
    nop
L001712C8:
    la v1, SS5432
    mtc1 zero, f0
    nop
    c.lt.s f12, f0
    lui at, (0x3FC90FDB >> 16)
    ori v0, at, (0x3FC90FDB & 0xFFFF)
    bc1f L00171300
    mtc1 v0, f0
    vnop
    add.s f12, f0, f12
    j Lfunc_00171308
    addi a3, zero, 0x1
    vnop
L00171300:
    sub.s f12, f0, f12
    addu a3, zero, zero
Lfunc_00171308:
    mfc1 t0, f12
    nop
    qmtc2.ni t0, vf6
    vmulx.x vf7, vf6, vf6x
    lqc2 vf5, 0x0(v1)
    vmulx.xyzw vf5, vf5, vf6x
    vmulaw.x ACC, vf6, vf0w
    vmulx.x vf8, vf7, vf7x
    vsub.zw vf6, vf0, vf0
    vmulx.x vf9, vf7, vf8x
    vmulx.x vf10, vf8, vf8x
    vmaddaw.x ACC, vf7, vf5w
    vmaddaz.x ACC, vf8, vf5z
    vmadday.x ACC, vf9, vf5y
    vmaddx.x vf6, vf10, vf5x
    vmul.x vf7, vf6, vf6
    vsubx.w vf7, vf0, vf7x
    vsqrt Q, vf7w
    bnez a3, L00171368
    vwaitq
    vnop
    b L00171370
    vaddq.x vf6, vf0, Q
    vnop
L00171368:
    vsubq.x vf6, vf0, Q
    nop
L00171370:
    qmfc2.ni t0, vf6
    mtc1 t0, f0
    jr ra
    nop
}

asm void flPS2CosFast(void)
{
    lui v0, (0xC0490FDB >> 16)
    ori v0, v0, (0xC0490FDB & 0xFFFF)
    mtc1 v0, f0
    nop
    c.le.s f12, f0
    bc1t L001713B8
    lui v0, (0x40490FDB >> 16)
    ori v0, v0, (0x40490FDB & 0xFFFF)
    mtc1 v0, f0
    nop
    c.lt.s f12, f0
    bc1t L00171408
    nop
    nop
L001713B8:
    la v0, at_tbl
    mfc1 t0, f12
    lqc2 vf6, 0x0(v0)
    qmtc2.ni t0, vf4
    slt t0, zero, t0
    ctc2.ni t0, vi2
    vmulx.x vf7, vf4, vf6x
    vftoi0.x vf7, vf7
    vmtir vi1, vf7x
    viaddi vi3, vi0, -0x2
    viadd vi1, vi1, vi2
    viand vi1, vi1, vi3
    vmfir.x vf7, vi1
    vitof0.x vf7, vf7
    vmuly.x vf7, vf7, vf6y
    vsub.x vf7, vf4, vf7
    qmfc2.ni t0, vf7
    mtc1 t0, f12
    nop
L00171408:
    la v1, SS5432
    mtc1 zero, f0
    nop
    c.lt.s f12, f0
    lui at, (0x3FC90FDB >> 16)
    ori v0, at, (0x3FC90FDB & 0xFFFF)
    bc1f L00171440
    mtc1 v0, f0
    vnop
    j Lfunc_00171448
    add.s f12, f0, f12
    vnop
    nop
L00171440:
    sub.s f12, f0, f12
    nop
Lfunc_00171448:
    mfc1 t0, f12
    nop
    qmtc2.ni t0, vf6
    vmulx.x vf7, vf6, vf6x
    lqc2 vf5, 0x0(v1)
    vmulx.xyzw vf5, vf5, vf6x
    vmulaw.x ACC, vf6, vf0w
    vmulx.x vf8, vf7, vf7x
    vsub.yzw vf6, vf0, vf0
    vmulx.x vf9, vf7, vf8x
    vmulx.x vf10, vf8, vf8x
    vmaddaw.x ACC, vf7, vf5w
    vmaddaz.x ACC, vf8, vf5z
    vmadday.x ACC, vf9, vf5y
    vmaddx.x vf6, vf10, vf5x
    vnop
    qmfc2.ni t1, vf6
    mtc1 t1, f0
    jr ra
    nop
}

asm void flPS2SinCosFast(void)
{
    lui v1, (0xC0490FDB >> 16)
    ori v1, v1, (0xC0490FDB & 0xFFFF)
    mtc1 v1, f0
    nop
    c.le.s f12, f0
    bc1t L001714D8
    lui v1, (0x40490FDB >> 16)
    ori v1, v1, (0x40490FDB & 0xFFFF)
    mtc1 v1, f0
    nop
    c.lt.s f12, f0
    bc1t L00171528
    nop
    nop
L001714D8:
    la v1, at_tbl
    mfc1 t0, f12
    lqc2 vf6, 0x0(v1)
    qmtc2.ni t0, vf4
    slt t0, zero, t0
    ctc2.ni t0, vi2
    vmulx.x vf7, vf4, vf6x
    vftoi0.x vf7, vf7
    vmtir vi1, vf7x
    viaddi vi3, vi0, -0x2
    viadd vi1, vi1, vi2
    viand vi1, vi1, vi3
    vmfir.x vf7, vi1
    vitof0.x vf7, vf7
    vmuly.x vf7, vf7, vf6y
    vsub.x vf7, vf4, vf7
    qmfc2.ni t0, vf7
    mtc1 t0, f12
    nop
L00171528:
    la a1, SS5432
    mtc1 zero, f0
    nop
    c.lt.s f12, f0
    lui at, (0x3FC90FDB >> 16)
    ori v1, at, (0x3FC90FDB & 0xFFFF)
    bc1f L00171560
    mtc1 v1, f0
    vnop
    add.s f12, f0, f12
    j Lfunc_00171568
    addi a3, zero, 0x1
    vnop
L00171560:
    sub.s f12, f0, f12
    addu a3, zero, zero
Lfunc_00171568:
    mfc1 t0, f12
    nop
    qmtc2.ni t0, vf6
    vmulx.x vf7, vf6, vf6x
    lqc2 vf5, 0x0(a1)
    vmulx.xyzw vf5, vf5, vf6x
    vmulaw.x ACC, vf6, vf0w
    vmulx.x vf8, vf7, vf7x
    vsub.zw vf6, vf0, vf0
    vmulx.x vf9, vf7, vf8x
    vmulx.x vf10, vf8, vf8x
    vmaddaw.x ACC, vf7, vf5w
    vmaddaz.x ACC, vf8, vf5z
    vmadday.x ACC, vf9, vf5y
    vmaddx.x vf6, vf10, vf5x
    vmul.x vf7, vf6, vf6
    vaddx.y vf6, vf0, vf6x
    vsubx.w vf7, vf0, vf7x
    vsqrt Q, vf7w
    bnez a3, L001715D0
    vwaitq
    vnop
    b L001715D8
    vaddq.x vf6, vf0, Q
    vnop
    nop
L001715D0:
    vsubq.x vf6, vf0, Q
    nop
L001715D8:
    jr ra
    sqc2 vf6, 0x0(a0)
}

#endif
