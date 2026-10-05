/* Memory card action layer, SLPM_654.95 main 0x27FDF0-0x280EF0.
 * McActXxxSet() arms one of the mc_act_* machines (index stored in MemcardWork.act, run
 * from McActMain every frame through the table mc_act_jmp); the machine advances
 * MemcardWork.step through the low level mc_* calls and finally writes MemcardWork.ret:
 * 0 = done, -1 = still busy, -0xFF no card, -0xFE unformatted, -0xFD no file,
 * -0xFC not enough free blocks, -0xFB card full/other, -0x100 I/O error.
 * Field names are guesses (see include/mcw.h). */
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

void mc_icon_sys_set(w)
MCW *w;
{
    u8 *p;
    MCFILE *f = mc_file_tbl[w->file];

    if (f->icon_on != 0) {
        p = f->icon;
        *(s16 *)(p + 6) = strlen(f->title);
        sprintf(p + 0xC0, lit_808_00384318, f->title, f->title2);
        strcpy(p + 0x104, f->l1);
        strcpy(p + 0x144, f->l2);
        strcpy(p + 0x184, f->l3);
    }
}
