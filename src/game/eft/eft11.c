/* eft11 - game.bin 0x00545E90-0x005468E8; eft11_i is still assembly (see
 * eft11_nm.c), the rest is in eft11b.c. A 13-part burst on a monster
 * (scale, fade and spin of each part follow keyframe tables). The effect
 * work doubles as its own draw primitive: work14 (PRIM+0x14, trans) holds
 * eft11_t0 and work (PRIM+0x18, owner) the part list. */
#include "eft.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

/* One part (0x24 bytes). */
typedef struct EFT11_PART {
    VEC3 pos;           /* 0x00 */
    VEC3 scale;         /* 0x0C */
    f32 alpha;          /* 0x18 */
    u16 roty;           /* 0x1C */
    u8 _pad1E[2];
    PRIM *prim;         /* 0x20 */
} EFT11_PART;

typedef struct KEY3 {
    s32 time;
    f32 v[3];
} KEY3;

typedef struct KEY1 {
    s32 time;
    f32 v;
} KEY1;

typedef struct KEYA {
    s32 time;
    u16 v;
    u8 _pad06[2];
} KEYA;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern FLMAT rview_matY;
extern EFT11_PART eft11_def[13];
extern KEY3 *scale_tbl[13];
extern KEY1 *alpha_tbl[13];
extern KEYA *roty_tbl[13];
extern u16 rotz_tbl[13];
extern s32 mdl_tbl_00645EB0[13];

u8 Em_stg_ck(EMW *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatRotZ33(FLMAT *, f32);

void eft11_i(EFTW *ew);
void eft11_m(EFTW *ew);
void eft11_d(EFTW *ew);
void eft11_e(EFTW *ew);

void eft11_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft11_i(ew);
        break;
    case 1:
        eft11_m(ew);
        break;
    case 2:
        eft11_d(ew);
        break;
    case 3:
        eft11_e(ew);
        break;
    }
}
