/* lb_by135 - agent B 0x005C4220-0x005C4324: lb_set_npc (village NPC placed from its 0x44-byte record in npc_dialog_table+0x60[stage]). */
#include "lbnpc_proto.h"
extern u8 *npc_dialog_table[];
typedef struct { f32 x, y, z; } LV3;
typedef struct LB_NPCREC {
    u8 kind;        /* 0x00 */
    u8 type;        /* 0x01 */
    u8 _pad02[2];
    s32 ang_y;      /* 0x04 */
    f32 pos[3];     /* 0x08 */
    f32 scale[3];   /* 0x14 */
    f32 spd;        /* 0x20 */
    s32 x24;        /* 0x24 */
    f32 *x28;       /* 0x28 */
    u8 x2C;         /* 0x2C */
    u8 script[0x44 - 0x2D];
} LB_NPCREC;
void set_event_npc();
void lb_set_npc(EMW *em) {
    LB_NPCREC *tbl;
    LB_NPCREC *r;
    u8 *w;

    tbl = ((LB_NPCREC **)((u8 *)npc_dialog_table + 0x60))[em->stg];
    w = em->ex;
    if (tbl != 0) {
        r = &tbl[em->type];
        em->ang[2] = 0;
        em->ang[0] = 0;
        em->ang[1] = r->ang_y;
        *(LV3 *)em->pos = *(LV3 *)r->pos;
        em->type = r->type;
        em->mdl_no = em->kind = r->x2C;
        *(u8 **)w = r->script;
        w[0xE] = r->kind;
        *(f32 **)(w + 8) = r->x28;
        *(s32 *)(w + 0x14) = r->x24;
        em->act_spd = r->spd;
        *(LV3 *)em->scale = *(LV3 *)r->scale;
        set_event_npc(em);
        if (*(f32 **)(w + 8) != 0) {
            f32 *p = *(f32 **)(w + 8);
            *(LV3 *)em->pos = *(LV3 *)p;
        }
    }
}
