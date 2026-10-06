/* lb_by168 - agent B 0x005C3C10-0x005C40DC: lb_npc_init (near-match C kept for the PC build; the PS2 build links the original bytes, see c_rawfuncs.txt). */
/* lb_npc_init near-match (agent B, round 7): typed rewrite from the asm, structure and logic follow the original; still differs in the register choices of the 4x8 byte clear loop (the original recomputes em+i each iteration) and a few nop placements. Not built. */
#include "lbnpc_proto.h"
extern s16 npc_disp_parts_00647910[];
extern u16 *lb_npc_jijii_tbl[];
void lb_set_npc();
void em_work_set();
void lb_npc_init_sub();
void frame_init();
void frame_move();
void lb_npc_chr_sub();
void Lb_World_calc();
void set_se_type();
#define EM_S8(em, o) (*(s8 *)((u8 *)(em) + (o)))
#define EM_S16(em, o) (*(s16 *)((u8 *)(em) + (o)))
/* original bytes: build/raw/lb_npc_init.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm void lb_npc_init(EMW *em)
{
#include "lb_npc_init.inc"
}
#else
void lb_npc_init(EMW *em) {
    LB_NPCMV *mv;
    int i;
    int a;
    int idx;
    u16 *jp;
    int jv;

    mv = (LB_NPCMV *)em->ex;
    lb_set_npc(em);
    em_work_set(em);
    em->x04++;
    em->x10 = 0;
    EM_S8(em, 0x1E) = 1;
    EM_S8(em, 0x412) = 0;
    em->x01 = 1;
    EM_S32(em, 0x1A0) = 0x40000000;
    EM_S32(em, 0x1F0) = 0x40000000;
    EM_S32(em, 0x240) = 0x40000000;
    EM_S32(em, 0x290) = 0x40000000;
    em->x39C = 0;
    em->stg = game_w.stage;
    if (em->kind == 0) {
        em->x300 = 2;
    } else {
        em->x10 = 1;
        em->x300 = 1;
    }
    EM_S32(em, 0x798) = 0x3F800000;
    EM_S8(em, 0x7D6) = 0;
    EM_S8(em, 0x4D4) = 1;
    em->char0 = 0;
    em->x2DE = 0;
    em->x2E0 = 0;
    EM_S16(em, 0x2E2) = 0;
    i = 0;
    do {
        s8 *p = (s8 *)em + i;
        p[0x4E6] = 0;
        p[0x4E7] = 0;
        p[0x4E8] = 0;
        p[0x4E9] = 0;
        p[0x4EA] = 0;
        p[0x4EB] = 0;
        p[0x4EC] = 0;
        p[0x4ED] = 0;
        i += 8;
    } while (i < 0x20);
    switch (em->kind) {
    case 0:
        a = npc_disp_parts_00647910[em->type * 2];
        ((s8 *)((u8 *)em + a))[0x4E6] = 1;
        idx = npc_disp_parts_00647910[em->type * 2 + 1];
        if (idx != 0xFF) {
            ((s8 *)((u8 *)em + idx))[0x4E6] = 1;
        }
        set_se_type(em);
        break;
    case 1:
        jp = lb_npc_jijii_tbl[em->type];
        jv = *jp;
        while (jv != 0xFFFF) {
            ((s8 *)((u8 *)em + (jv & 0xFFFF)))[0x4E6] = 1;
            jp++;
            jv = *jp;
        }
        break;
    }
    lb_npc_init_sub(em);
    if (em->kind == 0) {
        if (mv->route == 0) {
            a = mv->kind == 4 ? 0x2A8 : 1;
        } else {
            switch (mv->route->act) {
            case 0x77:
                a = 0x2A4;
                break;
            case 0x79:
                a = 0x291;
                break;
            case 0x6A:
                a = 0x29E;
                break;
            case 0x6B:
                a = 0x2A0;
                break;
            case 0x6D:
                a = 0x29C;
                break;
            case 0x7D:
            case 0x7C:
                a = 0x295;
                break;
            case 0x7A:
                a = 0x290;
                break;
            case 0x6F:
                a = 0x296;
                break;
            case 0x70:
                a = 0x298;
                break;
            case 0x73:
                a = 0x261;
                break;
            case 0x7E:
                a = 0x2A6;
                break;
            case 0x7F:
                a = 0x27E;
                break;
            case 0x80:
                a = 0x285;
                break;
            case 0x81:
                a = 0x284;
                break;
            default:
                a = 1;
                break;
            }
        }
        Lb_pl_chr_set(em, a, 0, 0);
        frame_init(em, em->act_tm0, em->blend0, 0);
        frame_init(em, em->act_tm1, em->blend1, 1);
    } else {
        if (mv->route == 0) {
            a = mv->kind == 0x48 ? 0x3FF : 0x3E9;
        } else {
            switch (mv->route->act) {
            case 0x82:
                a = 0x426;
                break;
            case 0x86:
                a = 0x432;
                break;
            case 0x8B:
                i = (s16)(game_w.stage - 0x51);
                if (i < 0 || i >= 0x56) {
                    i = 0;
                }
                if (lb_sys.x88[(s16)i] == 2) {
                    Lb_act_set(em, 0, 0x8C);
                    a = 0x3E9;
                } else {
                    a = 0x3E9;
                }
                break;
            default:
                a = 0x3E9;
                break;
            }
        }
        Lb_pl_chr_set0(em, a, 0, 0, 0);
        frame_init(em, em->act_tm0, em->blend0, 0);
    }
    frame_move(em);
    lb_npc_chr_sub(em);
    lb_npc_chr_sub(em);
    Lb_World_calc(em);
}
#endif
