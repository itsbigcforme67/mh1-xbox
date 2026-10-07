/* fl library VU0 macro-mode routines (SLPM_654.95 0x171F00-0x172808): flmatNormalize33, flmatRotX33, flmatRotY33, flmatRotZ33, flmatSetXYZ33, flmatSetZYX33, flmatRotXYZ33, flmatRotZXY33.
 * Written as MWCC inline assembly (the original is VU0 inline asm), one routine per function. */
#ifdef __MWERKS__
void flPS2SinCosFast();
void flmatInit33();

asm void flmatNormalize33(void)
{
    addiu a1, a0, 0x10
    addiu v1, a0, 0x20
    lqc2 vf4, 0x0(a0)
    vmul.xyz vf5, vf4, vf4
    vaddy.x vf5, vf5, vf5y
    vaddz.x vf5, vf5, vf5z
    vrsqrt Q, vf0w, vf5x
    vwaitq
    vmulq.xyz vf4, vf4, Q
    sqc2 vf4, 0x0(a0)
    lqc2 vf4, 0x0(a1)
    vmul.xyz vf5, vf4, vf4
    vaddy.x vf5, vf5, vf5y
    vaddz.x vf5, vf5, vf5z
    vrsqrt Q, vf0w, vf5x
    vwaitq
    vmulq.xyz vf4, vf4, Q
    sqc2 vf4, 0x0(a1)
    lqc2 vf4, 0x0(v1)
    vmul.xyz vf5, vf4, vf4
    vaddy.x vf5, vf5, vf5y
    vaddz.x vf5, vf5, vf5z
    vrsqrt Q, vf0w, vf5x
    vwaitq
    vmulq.xyz vf4, vf4, Q
    sqc2 vf4, 0x0(v1)
    jr ra
    nop
}

asm void flmatRotX33(void)
{
    addiu sp, sp, -0x20
    daddu t1, a0, zero
    sd ra, 0x0(sp)
    jal flPS2SinCosFast
    addiu a0, sp, 0x10
    lwc1 f1, 0x10(sp)
    lwc1 f0, 0x14(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.yz vf10, vf0, vf0
    vaddw.x vf10, vf0, vf0w
    vadd.x vf11, vf0, vf0
    vaddx.y vf11, vf0, vf21x
    vaddx.z vf11, vf0, vf20x
    vadd.x vf12, vf0, vf0
    vsubx.y vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    ld ra, 0x0(sp)
    jr ra
    addiu sp, sp, 0x20
}

asm void flmatRotY33(void)
{
    addiu sp, sp, -0x20
    daddu t1, a0, zero
    sd ra, 0x0(sp)
    jal flPS2SinCosFast
    addiu a0, sp, 0x10
    lwc1 f1, 0x10(sp)
    lwc1 f0, 0x14(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.xz vf11, vf0, vf0
    vaddw.y vf11, vf0, vf0w
    vadd.y vf10, vf0, vf0
    vaddx.x vf10, vf0, vf21x
    vsubx.z vf10, vf0, vf20x
    vadd.y vf12, vf0, vf0
    vaddx.x vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    ld ra, 0x0(sp)
    jr ra
    addiu sp, sp, 0x20
}

asm void flmatRotZ33(void)
{
    addiu sp, sp, -0x20
    daddu t1, a0, zero
    sd ra, 0x0(sp)
    jal flPS2SinCosFast
    addiu a0, sp, 0x10
    lwc1 f1, 0x10(sp)
    lwc1 f0, 0x14(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.z vf10, vf0, vf0
    vaddx.y vf10, vf0, vf20x
    vaddx.x vf10, vf0, vf21x
    vadd.z vf11, vf0, vf0
    vsubx.x vf11, vf0, vf20x
    vaddx.y vf11, vf0, vf21x
    vmr32.xyz vf12, vf0
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    ld ra, 0x0(sp)
    jr ra
    addiu sp, sp, 0x20
}

asm void flmatSetXYZ33(void)
{
    addiu sp, sp, -0x60
    sd ra, 0x20(sp)
    sq s0, 0x10(sp)
    swc1 f22, 0x8(sp)
    daddu s0, a0, zero
    swc1 f21, 0x4(sp)
    swc1 f20, 0x0(sp)
    mov.s f22, f12
    mov.s f21, f13
    jal flmatInit33
    mov.s f20, f14
    mov.s f12, f22
    jal flPS2SinCosFast
    addiu a0, sp, 0x50
    lwc1 f1, 0x50(sp)
    lwc1 f0, 0x54(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(s0)
    lqc2 vf5, 0x10(s0)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.yz vf10, vf0, vf0
    vaddw.x vf10, vf0, vf0w
    vadd.x vf11, vf0, vf0
    vaddx.y vf11, vf0, vf21x
    vaddx.z vf11, vf0, vf20x
    vadd.x vf12, vf0, vf0
    vsubx.y vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(s0)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(s0)
    sqc2 vf5, 0x10(s0)
    sqc2 vf6, 0x20(s0)
    mov.s f12, f21
    jal flPS2SinCosFast
    addiu a0, sp, 0x40
    lwc1 f1, 0x40(sp)
    lwc1 f0, 0x44(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(s0)
    lqc2 vf5, 0x10(s0)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.xz vf11, vf0, vf0
    vaddw.y vf11, vf0, vf0w
    vadd.y vf10, vf0, vf0
    vaddx.x vf10, vf0, vf21x
    vsubx.z vf10, vf0, vf20x
    vadd.y vf12, vf0, vf0
    vaddx.x vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(s0)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(s0)
    sqc2 vf5, 0x10(s0)
    sqc2 vf6, 0x20(s0)
    mov.s f12, f20
    jal flPS2SinCosFast
    addiu a0, sp, 0x30
    lwc1 f1, 0x30(sp)
    lwc1 f0, 0x34(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(s0)
    lqc2 vf5, 0x10(s0)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.z vf10, vf0, vf0
    vaddx.y vf10, vf0, vf20x
    vaddx.x vf10, vf0, vf21x
    vadd.z vf11, vf0, vf0
    vsubx.x vf11, vf0, vf20x
    vaddx.y vf11, vf0, vf21x
    vmr32.xyz vf12, vf0
    lqc2 vf6, 0x20(s0)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(s0)
    sqc2 vf5, 0x10(s0)
    sqc2 vf6, 0x20(s0)
    ld ra, 0x20(sp)
    lwc1 f22, 0x8(sp)
    lq s0, 0x10(sp)
    lwc1 f21, 0x4(sp)
    lwc1 f20, 0x0(sp)
    jr ra
    addiu sp, sp, 0x60
}

asm void flmatSetZYX33(void)
{
    addiu sp, sp, -0x60
    sd ra, 0x20(sp)
    sq s0, 0x10(sp)
    swc1 f22, 0x8(sp)
    daddu s0, a0, zero
    swc1 f21, 0x4(sp)
    swc1 f20, 0x0(sp)
    mov.s f22, f12
    mov.s f21, f13
    jal flmatInit33
    mov.s f20, f14
    mov.s f12, f20
    jal flPS2SinCosFast
    addiu a0, sp, 0x50
    lwc1 f1, 0x50(sp)
    lwc1 f0, 0x54(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(s0)
    lqc2 vf5, 0x10(s0)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.z vf10, vf0, vf0
    vaddx.y vf10, vf0, vf20x
    vaddx.x vf10, vf0, vf21x
    vadd.z vf11, vf0, vf0
    vsubx.x vf11, vf0, vf20x
    vaddx.y vf11, vf0, vf21x
    vmr32.xyz vf12, vf0
    lqc2 vf6, 0x20(s0)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(s0)
    sqc2 vf5, 0x10(s0)
    sqc2 vf6, 0x20(s0)
    mov.s f12, f21
    jal flPS2SinCosFast
    addiu a0, sp, 0x40
    lwc1 f1, 0x40(sp)
    lwc1 f0, 0x44(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(s0)
    lqc2 vf5, 0x10(s0)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.xz vf11, vf0, vf0
    vaddw.y vf11, vf0, vf0w
    vadd.y vf10, vf0, vf0
    vaddx.x vf10, vf0, vf21x
    vsubx.z vf10, vf0, vf20x
    vadd.y vf12, vf0, vf0
    vaddx.x vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(s0)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(s0)
    sqc2 vf5, 0x10(s0)
    sqc2 vf6, 0x20(s0)
    mov.s f12, f22
    jal flPS2SinCosFast
    addiu a0, sp, 0x30
    lwc1 f1, 0x30(sp)
    lwc1 f0, 0x34(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(s0)
    lqc2 vf5, 0x10(s0)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.yz vf10, vf0, vf0
    vaddw.x vf10, vf0, vf0w
    vadd.x vf11, vf0, vf0
    vaddx.y vf11, vf0, vf21x
    vaddx.z vf11, vf0, vf20x
    vadd.x vf12, vf0, vf0
    vsubx.y vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(s0)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(s0)
    sqc2 vf5, 0x10(s0)
    sqc2 vf6, 0x20(s0)
    ld ra, 0x20(sp)
    lwc1 f22, 0x8(sp)
    lq s0, 0x10(sp)
    lwc1 f21, 0x4(sp)
    lwc1 f20, 0x0(sp)
    jr ra
    addiu sp, sp, 0x60
}

asm void flmatRotXYZ33(void)
{
    addiu sp, sp, -0x40
    daddu t1, a0, zero
    sd ra, 0x0(sp)
    jal flPS2SinCosFast
    addiu a0, sp, 0x30
    lwc1 f1, 0x30(sp)
    lwc1 f0, 0x34(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.yz vf10, vf0, vf0
    vaddw.x vf10, vf0, vf0w
    vadd.x vf11, vf0, vf0
    vaddx.y vf11, vf0, vf21x
    vaddx.z vf11, vf0, vf20x
    vadd.x vf12, vf0, vf0
    vsubx.y vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    mov.s f12, f13
    jal flPS2SinCosFast
    addiu a0, sp, 0x20
    lwc1 f1, 0x20(sp)
    lwc1 f0, 0x24(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.xz vf11, vf0, vf0
    vaddw.y vf11, vf0, vf0w
    vadd.y vf10, vf0, vf0
    vaddx.x vf10, vf0, vf21x
    vsubx.z vf10, vf0, vf20x
    vadd.y vf12, vf0, vf0
    vaddx.x vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    mov.s f12, f14
    jal flPS2SinCosFast
    addiu a0, sp, 0x10
    lwc1 f1, 0x10(sp)
    lwc1 f0, 0x14(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.z vf10, vf0, vf0
    vaddx.y vf10, vf0, vf20x
    vaddx.x vf10, vf0, vf21x
    vadd.z vf11, vf0, vf0
    vsubx.x vf11, vf0, vf20x
    vaddx.y vf11, vf0, vf21x
    vmr32.xyz vf12, vf0
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    ld ra, 0x0(sp)
    jr ra
    addiu sp, sp, 0x40
}

asm void flmatRotZXY33(void)
{
    addiu sp, sp, -0x40
    daddu t1, a0, zero
    mov.s f2, f12
    sd ra, 0x0(sp)
    addiu a0, sp, 0x30
    jal flPS2SinCosFast
    mov.s f12, f14
    lwc1 f1, 0x30(sp)
    lwc1 f0, 0x34(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.z vf10, vf0, vf0
    vaddx.y vf10, vf0, vf20x
    vaddx.x vf10, vf0, vf21x
    vadd.z vf11, vf0, vf0
    vsubx.x vf11, vf0, vf20x
    vaddx.y vf11, vf0, vf21x
    vmr32.xyz vf12, vf0
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    mov.s f12, f2
    jal flPS2SinCosFast
    addiu a0, sp, 0x20
    lwc1 f1, 0x20(sp)
    lwc1 f0, 0x24(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.yz vf10, vf0, vf0
    vaddw.x vf10, vf0, vf0w
    vadd.x vf11, vf0, vf0
    vaddx.y vf11, vf0, vf21x
    vaddx.z vf11, vf0, vf20x
    vadd.x vf12, vf0, vf0
    vsubx.y vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    mov.s f12, f13
    jal flPS2SinCosFast
    addiu a0, sp, 0x10
    lwc1 f1, 0x10(sp)
    lwc1 f0, 0x14(sp)
    mfc1 a2, f1
    mfc1 a3, f0
    lqc2 vf4, 0x0(t1)
    lqc2 vf5, 0x10(t1)
    qmtc2.ni a2, vf20
    qmtc2.ni a3, vf21
    vadd.xz vf11, vf0, vf0
    vaddw.y vf11, vf0, vf0w
    vadd.y vf10, vf0, vf0
    vaddx.x vf10, vf0, vf21x
    vsubx.z vf10, vf0, vf20x
    vadd.y vf12, vf0, vf0
    vaddx.x vf12, vf0, vf20x
    vaddx.z vf12, vf0, vf21x
    lqc2 vf6, 0x20(t1)
    vmulax.xyz ACC, vf10, vf4x
    vmadday.xyz ACC, vf11, vf4y
    vmaddz.xyz vf4, vf12, vf4z
    vmulax.xyz ACC, vf10, vf5x
    vmadday.xyz ACC, vf11, vf5y
    vmaddz.xyz vf5, vf12, vf5z
    vmulax.xyz ACC, vf10, vf6x
    vmadday.xyz ACC, vf11, vf6y
    vmaddz.xyz vf6, vf12, vf6z
    sqc2 vf4, 0x0(t1)
    sqc2 vf5, 0x10(t1)
    sqc2 vf6, 0x20(t1)
    ld ra, 0x0(sp)
    jr ra
    addiu sp, sp, 0x40
}

#endif
