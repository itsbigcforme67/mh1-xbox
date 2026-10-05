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

void McActInit(file)
int file;
{
    MemcardWork.file = file;
    MemcardWork.step = 0;
    MemcardWork.x04 = 0;
    MemcardWork.act = 0;
}

void McActMain(void)
{
    MCW *w = &MemcardWork;

    if ((adx_cont_flag = MemcardWork.act) != 0) {
        ADXM_Lock();
    }
    mc_act_jmp[w->act](w);
    if (adx_cont_flag != 0) {
        ADXM_Unlock();
    }
}

void McActStopSet(void)
{
    MemcardWork.act = 0;
    MemcardWork.xB0 = 0;
}

void mc_act_stop(void)
{
}

void mc_act_return(w)
MCW *w;
{
    if (w->ret != -1) {
        McActStopSet();
    }
}

void McActCheckSet(void)
{
    MemcardWork.step = 0;
    MemcardWork.x04 = 0;
    MemcardWork.act = 1;
    MemcardWork.ret = -1;
    MemcardWork.port = 0;
}

void mc_act_check(w)
MCW *w;
{
    if (mc_check_card(w) >= 0) {
        if (++w->port >= w->nports) {
            w->port = 0;
        }
    }
}

void McActLoadSet(port, buf)
int port;
u8 *buf;
{
    MCFILE *f = mc_file_tbl[MemcardWork.file];

    MemcardWork.act = 2;
    MemcardWork.ret = -1;
    MemcardWork.step = 0;
    MemcardWork.x04 = 0;
    MemcardWork.port = port;
    MemcardWork.buf = buf;
    sprintf(MemcardWork.name, lit_422_00384308, f->dir, f->dir);
    MemcardWork.len = f->dsize;
}

void mc_act_load(w)
MCW *w;
{
    switch (w->step) {
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
            w->step++;
            break;
        }
        break;
    case 1:
        switch (mc_check_file(w)) {
        case 0:
            w->step++;
            break;
        case 1:
            w->step = 0;
            w->ret = -0xFD;
            break;
        }
        break;
    case 2:
        switch (mc_read_file(w)) {
        case 0:
            w->step = 0;
            w->ret = 0;
            break;
        case 1:
            w->step = 0;
            w->ret = -0x100;
            break;
        }
        break;
    }
    mc_act_return(w);
}

void McActSave0Set(port, buf, flag)
int port;
u8 *buf;
int flag;
{
    MCFILE *f = mc_file_tbl[MemcardWork.file];

    MemcardWork.act = 3;
    MemcardWork.ret = -1;
    MemcardWork.step = 0;
    MemcardWork.x04 = 0;
    MemcardWork.port = port;
    MemcardWork.buf = buf;
    MemcardWork.xA0 = flag;
    sprintf(MemcardWork.name, lit_422_00384308, f->dir, f->dir);
    MemcardWork.len = f->dsize;
}

void mc_act_save0(w)
MCW *w;
{
    MCFILE *f = mc_file_tbl[w->file];

    switch (w->step) {
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
            w->step++;
            break;
        }
        break;
    case 1:
        switch (mc_check_file(w)) {
        case 0:
            if (w->xA0 == 0) {
                w->step = 0;
                w->ret = 0;
            } else {
                w->step++;
            }
            break;
        case 1:
            w->step = 0;
            if (w->info[w->port] >= f->blocks) {
                w->ret = -0xFD;
            } else {
                w->ret = -0xFC;
            }
            break;
        }
        break;
    case 2:
        switch (mc_read_file(w)) {
        case 0:
            w->step = 0;
            w->ret = 0;
            break;
        case 1:
            w->step = 0;
            if (w->res == -2) {
                w->ret = -0xFE;
            } else if (w->res == -3) {
                w->ret = -0xFB;
            } else if (w->res < -10) {
                w->ret = -0xFF;
            } else {
                w->ret = -0x100;
            }
            break;
        }
        break;
    }
    mc_act_return(w);
}

void McActSaveSet(port, buf)
int port;
u8 *buf;
{
    MCFILE *f = mc_file_tbl[MemcardWork.file];

    MemcardWork.act = 4;
    MemcardWork.ret = -1;
    MemcardWork.step = 0;
    MemcardWork.x04 = 0;
    MemcardWork.slot = 0;
    MemcardWork.port = port;
    MemcardWork.buf = buf;
    sprintf(MemcardWork.name, lit_422_00384308, f->dir, f->dir);
    MemcardWork.len = f->dsize;
    MemcardWork.xB0 = 1;
}
