/* fl library VU0 macro-mode routines (SLPM_654.95 0x172970-0x172CA8): flmatTranspose, flmatTranspose33, flmatMul, flmatMul2, flmatMul33, flmatMul33_2, flmatInvert.
 * Written as MWCC inline assembly (the original is VU0 inline asm), one routine per function. */
#ifdef __MWERKS__
extern char flPS2INITMATRIX[];

asm void flmatTranspose(void)
{
    lqc2 vf8, 0x0(a0)
    lqc2 vf9, 0x10(a0)
    lqc2 vf10, 0x20(a0)
    lqc2 vf11, 0x30(a0)
    vsub.xyzw vf1, vf0, vf0
    vaddx.x vf4, vf1, vf8x
    vaddy.x vf5, vf1, vf8y
    vaddz.x vf6, vf1, vf8z
    vaddw.x vf7, vf1, vf8w
    vaddx.y vf4, vf1, vf9x
    vaddy.y vf5, vf1, vf9y
    vaddz.y vf6, vf1, vf9z
    vaddw.y vf7, vf1, vf9w
    vaddx.z vf4, vf1, vf10x
    vaddy.z vf5, vf1, vf10y
    vaddz.z vf6, vf1, vf10z
    vaddw.z vf7, vf1, vf10w
    vaddx.w vf4, vf1, vf11x
    vaddy.w vf5, vf1, vf11y
    vaddz.w vf6, vf1, vf11z
    vaddw.w vf7, vf1, vf11w
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    sqc2 vf6, 0x20(a0)
    jr ra
    sqc2 vf7, 0x30(a0)
}

asm void flmatTranspose33(void)
{
    lqc2 vf8, 0x0(a0)
    lqc2 vf9, 0x10(a0)
    lqc2 vf10, 0x20(a0)
    vaddx.x vf4, vf0, vf8x
    vaddy.x vf5, vf0, vf8y
    vaddz.x vf6, vf0, vf8z
    vaddx.y vf4, vf0, vf9x
    vaddy.y vf5, vf0, vf9y
    vaddz.y vf6, vf0, vf9z
    vaddx.z vf4, vf0, vf10x
    vaddy.z vf5, vf0, vf10y
    vaddz.z vf6, vf0, vf10z
    vaddx.w vf4, vf8, vf0x
    vaddx.w vf5, vf9, vf0x
    vaddx.w vf6, vf10, vf0x
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    jr ra
    sqc2 vf6, 0x20(a0)
}

asm void flmatMul(void)
{
    lqc2 vf4, 0x0(a2)
    lqc2 vf5, 0x10(a2)
    lqc2 vf6, 0x20(a2)
    lqc2 vf7, 0x30(a2)
    lqc2 vf8, 0x0(a1)
    lqc2 vf9, 0x10(a1)
    lqc2 vf10, 0x20(a1)
    lqc2 vf11, 0x30(a1)
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf8, vf7, vf8w
    vmulax.xyzw ACC, vf4, vf9x
    vmadday.xyzw ACC, vf5, vf9y
    vmaddaz.xyzw ACC, vf6, vf9z
    vmaddw.xyzw vf9, vf7, vf9w
    vmulax.xyzw ACC, vf4, vf10x
    vmadday.xyzw ACC, vf5, vf10y
    vmaddaz.xyzw ACC, vf6, vf10z
    vmaddw.xyzw vf10, vf7, vf10w
    vmulax.xyzw ACC, vf4, vf11x
    vmadday.xyzw ACC, vf5, vf11y
    vmaddaz.xyzw ACC, vf6, vf11z
    vmaddw.xyzw vf11, vf7, vf11w
    sqc2 vf8, 0x0(a0)
    sqc2 vf9, 0x10(a0)
    sqc2 vf10, 0x20(a0)
    sqc2 vf11, 0x30(a0)
    jr ra
    nop
}

asm void flmatMul2(void)
{
    lqc2 vf4, 0x0(a1)
    lqc2 vf5, 0x10(a1)
    lqc2 vf6, 0x20(a1)
    lqc2 vf7, 0x30(a1)
    lqc2 vf8, 0x0(a0)
    lqc2 vf9, 0x10(a0)
    lqc2 vf10, 0x20(a0)
    lqc2 vf11, 0x30(a0)
    vmulax.xyzw ACC, vf4, vf8x
    vmadday.xyzw ACC, vf5, vf8y
    vmaddaz.xyzw ACC, vf6, vf8z
    vmaddw.xyzw vf8, vf7, vf8w
    vmulax.xyzw ACC, vf4, vf9x
    vmadday.xyzw ACC, vf5, vf9y
    vmaddaz.xyzw ACC, vf6, vf9z
    vmaddw.xyzw vf9, vf7, vf9w
    vmulax.xyzw ACC, vf4, vf10x
    vmadday.xyzw ACC, vf5, vf10y
    vmaddaz.xyzw ACC, vf6, vf10z
    vmaddw.xyzw vf10, vf7, vf10w
    vmulax.xyzw ACC, vf4, vf11x
    vmadday.xyzw ACC, vf5, vf11y
    vmaddaz.xyzw ACC, vf6, vf11z
    vmaddw.xyzw vf11, vf7, vf11w
    sqc2 vf8, 0x0(a0)
    sqc2 vf9, 0x10(a0)
    sqc2 vf10, 0x20(a0)
    sqc2 vf11, 0x30(a0)
    jr ra
    nop
}

asm void flmatMul33(void)
{
    lqc2 vf4, 0x0(a2)
    lqc2 vf5, 0x10(a2)
    lqc2 vf6, 0x20(a2)
    lqc2 vf8, 0x0(a1)
    lqc2 vf9, 0x10(a1)
    lqc2 vf10, 0x20(a1)
    lqc2 vf11, 0x0(a0)
    lqc2 vf12, 0x10(a0)
    lqc2 vf13, 0x20(a0)
    vmulax.xyz ACC, vf4, vf8x
    vmadday.xyz ACC, vf5, vf8y
    vmaddz.xyz vf11, vf6, vf8z
    vmulax.xyz ACC, vf4, vf9x
    vmadday.xyz ACC, vf5, vf9y
    vmaddz.xyz vf12, vf6, vf9z
    vmulax.xyz ACC, vf4, vf10x
    vmadday.xyz ACC, vf5, vf10y
    vmaddz.xyz vf13, vf6, vf10z
    sqc2 vf11, 0x0(a0)
    sqc2 vf12, 0x10(a0)
    sqc2 vf13, 0x20(a0)
    jr ra
    nop
}

asm void flmatMul33_2(void)
{
    lqc2 vf4, 0x0(a1)
    lqc2 vf5, 0x10(a1)
    lqc2 vf6, 0x20(a1)
    lqc2 vf8, 0x0(a0)
    lqc2 vf9, 0x10(a0)
    lqc2 vf10, 0x20(a0)
    vmulax.xyz ACC, vf4, vf8x
    vmadday.xyz ACC, vf5, vf8y
    vmaddz.xyz vf8, vf6, vf8z
    vmulax.xyz ACC, vf4, vf9x
    vmadday.xyz ACC, vf5, vf9y
    vmaddz.xyz vf9, vf6, vf9z
    vmulax.xyz ACC, vf4, vf10x
    vmadday.xyz ACC, vf5, vf10y
    vmaddz.xyz vf10, vf6, vf10z
    sqc2 vf8, 0x0(a0)
    sqc2 vf9, 0x10(a0)
    sqc2 vf10, 0x20(a0)
    jr ra
    nop
}

asm void flmatInvert(void)
{
    la v1, flPS2INITMATRIX
    lqc2 vf8, 0x0(a1)
    lqc2 vf9, 0x10(a1)
    lqc2 vf10, 0x20(a1)
    lqc2 vf11, 0x30(a1)
    vmul.xyz vf1, vf8, vf8
    vaddy.x vf1, vf1, vf1y
    vaddz.x vf1, vf1, vf1z
    vrsqrt Q, vf0w, vf1x
    lqc2 vf4, 0x0(v1)
    lqc2 vf5, 0x10(v1)
    lqc2 vf6, 0x20(v1)
    lqc2 vf7, 0x30(v1)
    vmul.xyz vf2, vf9, vf9
    vaddy.x vf2, vf2, vf2y
    vaddz.x vf2, vf2, vf2z
    vmul.xyz vf3, vf10, vf10
    vaddy.x vf3, vf3, vf3y
    vaddz.x vf3, vf3, vf3z
    vsub.xyz vf21, vf0, vf11
    vwaitq
    vaddq.x vf20, vf0, Q
    vrsqrt Q, vf0w, vf2x
    vmulx.x vf4, vf20, vf8x
    vmuly.x vf5, vf20, vf8y
    vmulz.x vf6, vf20, vf8z
    vwaitq
    vaddq.y vf20, vf0, Q
    vrsqrt Q, vf0w, vf3x
    vmulx.y vf4, vf20, vf9x
    vmuly.y vf5, vf20, vf9y
    vmulz.y vf6, vf20, vf9z
    vwaitq
    vaddq.z vf20, vf0, Q
    vmulx.z vf4, vf20, vf10x
    vmuly.z vf5, vf20, vf10y
    vmulz.z vf6, vf20, vf10z
    vmulax.xyz ACC, vf4, vf21x
    vmadday.xyz ACC, vf5, vf21y
    vmaddz.xyz vf7, vf6, vf21z
    vmul.xyz vf4, vf4, vf20
    vmul.xyz vf5, vf5, vf20
    vmul.xyz vf6, vf6, vf20
    vmul.xyz vf7, vf7, vf20
    sqc2 vf4, 0x0(a0)
    sqc2 vf5, 0x10(a0)
    sqc2 vf6, 0x20(a0)
    jr ra
    sqc2 vf7, 0x30(a0)
}

#endif
