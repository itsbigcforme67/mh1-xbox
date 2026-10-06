/* SLPM_654.95 0x002804D0-0x00280884: mc_act_save .. mc_act_save. See mcact_nm.c. */
#include "types.h"
#include "mcw.h"

extern s8 adx_cont_flag;
extern char lit_421_00384300[], lit_422_00384308[], lit_423_00384310[], lit_808_00384318[];

void ADXM_Lock();
void ADXM_Unlock();
int sprintf();
char *strcpy();
int strlen();
int mc_check_card();
int mc_check_file();
int mc_read_file();
int mc_write_file();
int mc_mkdir();
int mc_create_file();
int mc_attr_file();
int mc_format();
int mc_unformat();
int mc_delete_dir();
int mc_get_dir();
void McActStopSet();
void mc_act_return();
void mc_icon_sys_set();
extern void (*mc_act_jmp[])();

#define MCFI(f, i) ((MCF *)((i) * 16 + (u8 *)(f)))
#define MCFJ(f, i) ((MCF *)(((i) << 4) + (int)(f)))



























void mc_act_save(w)
MCW *w;
{
    MCFILE *f = mc_file_tbl[w->file];
    MCF *e;

    switch (w->astep) {
    case 0:
        switch (mc_check_card(w)) {
        case 0:
            w->ret = -0xFF;
            break;
        case 3:
            w->ret = -0xFE;
            break;
        case 1:
        case 2:
            w->astep++;
            mc_icon_sys_set(w);
            break;
        }
        break;
    case 1:
        switch (mc_check_file(w)) {
        case 0:
            w->astep = 0xD;
            w->xA0 = 0;
            break;
        case 1:
            w->astep++;
            w->xA0 = 1;
            sprintf(w->name, lit_423_00384310, f->dir);
            break;
        }
        break;
    case 2:
        switch (mc_mkdir(w)) {
        case 0:
            w->astep++;
            break;
        case 1:
            w->astep = 0;
            w->ret = -0x100;
            break;
        }
        break;
    case 3:
        switch (mc_attr_file(w)) {
        case 0:
            w->astep = 0xA;
            break;
        case 1:
            w->astep = 0;
            w->ret = -0x100;
            break;
        }
        break;
    case 0xA:
    again:
        e = MCFJ(f, w->slot);
        if (e->on == 0) {
            goto next;
        }
        sprintf(w->name, lit_422_00384308, f->dir, e->name);
        if (w->slot > 0) {
            w->buf = MCFJ(f, w->slot)->data;
        }
        w->len = MCFJ(f, w->slot)->size;
        if (w->xA0 == 0) {
            w->astep++;
        } else {
            w->astep += 2;
            break;
        }
    case 0xB:
        switch (mc_check_file(w)) {
        case 0:
            if (MCFJ(f, w->slot)->on == 1) {
                goto next;
            }
            w->astep += 2;
            break;
        case 1:
            w->astep++;
            break;
        }
        break;
    case 0xC:
        switch (mc_create_file(w)) {
        case 0:
            w->astep++;
            break;
        case 1:
            w->astep = 0;
            w->ret = -0x100;
            break;
        }
        break;
    case 0xD:
        switch (mc_write_file(w)) {
        case 0:
        next:
            if (++w->slot < 5) {
                w->astep = 0xA;
                goto again;
            }
            w->astep = 0;
            w->ret = 0;
            break;
        case 1:
            w->astep = 0;
            w->ret = -0x100;
            break;
        }
        break;
    }
    mc_act_return(w);
}
