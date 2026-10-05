/* Memory card action layer, SLPM_654.95 main 0x27FDF0-0x280EF0.
 * McActXxxSet() arms one of the mc_act_* machines (index stored in MemcardWork.act, run
 * from McActMain every frame through the table mc_act_jmp); the machine advances
 * MemcardWork.astep through the low level mc_* calls and finally writes MemcardWork.ret:
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
    MemcardWork.astep = 0;
    MemcardWork.step = 0;
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
    MemcardWork.astep = 0;
    MemcardWork.step = 0;
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
    MemcardWork.astep = 0;
    MemcardWork.step = 0;
    MemcardWork.port = port;
    MemcardWork.buf = buf;
    sprintf(MemcardWork.name, lit_422_00384308, f->dir, f->dir);
    MemcardWork.len = f->dsize;
}

void mc_act_load(w)
MCW *w;
{
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
            break;
        }
        break;
    case 1:
        switch (mc_check_file(w)) {
        case 0:
            w->astep++;
            break;
        case 1:
            w->astep = 0;
            w->ret = -0xFD;
            break;
        }
        break;
    case 2:
        switch (mc_read_file(w)) {
        case 0:
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

void McActSave0Set(port, buf, flag)
int port;
u8 *buf;
int flag;
{
    MCFILE *f = mc_file_tbl[MemcardWork.file];

    MemcardWork.act = 3;
    MemcardWork.ret = -1;
    MemcardWork.astep = 0;
    MemcardWork.step = 0;
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
            break;
        }
        break;
    case 1:
        switch (mc_check_file(w)) {
        case 0:
            if (w->xA0 == 0) {
                w->astep = 0;
                w->ret = 0;
            } else {
                w->astep++;
            }
            break;
        case 1:
            w->astep = 0;
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
            w->astep = 0;
            w->ret = 0;
            break;
        case 1:
            w->astep = 0;
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
    MemcardWork.astep = 0;
    MemcardWork.step = 0;
    MemcardWork.slot = 0;
    MemcardWork.port = port;
    MemcardWork.buf = buf;
    sprintf(MemcardWork.name, lit_422_00384308, f->dir, f->dir);
    MemcardWork.len = f->dsize;
    MemcardWork.xB0 = 1;
}

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
        e = MCFI(f, w->slot);
        if (e->on == 0) {
            goto next;
        }
        sprintf(w->name, lit_422_00384308, f->dir, e->name);
        if (w->slot > 0) {
            w->buf = MCFI(f, w->slot)->data;
        }
        w->len = MCFI(f, w->slot)->size;
        if (w->xA0 == 0) {
            w->astep++;
        } else {
            w->astep += 2;
            break;
        }
    case 0xB:
        switch (mc_check_file(w)) {
        case 0:
            if (MCFI(f, w->slot)->on == 1) {
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

void McActFormatSet(port)
int port;
{
    MemcardWork.port = port;
    MemcardWork.astep = 0;
    MemcardWork.act = 5;
    MemcardWork.ret = -1;
    MemcardWork.xB0 = 1;
    MemcardWork.step = 0;
}

void mc_act_format(w)
MCW *w;
{
    switch (w->astep) {
    case 0:
        switch (mc_format(w)) {
        case 0:
            w->ret = 0;
            break;
        case 1:
            w->ret = -0x100;
            break;
        }
        break;
    }
    mc_act_return(w);
}

void mc_act_unformat(w)
MCW *w;
{
    switch (w->astep) {
    case 0:
        switch (mc_unformat(w)) {
        case 0:
            w->ret = 0;
            break;
        case 1:
            w->ret = -0x100;
            break;
        }
        break;
    }
    mc_act_return(w);
}

void mc_act_delete(w)
MCW *w;
{
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
            break;
        }
        break;
    case 1:
        switch (mc_check_file(w)) {
        case 0:
            w->astep++;
            break;
        case 1:
            w->astep = 0;
            w->ret = -0xFD;
            break;
        }
        break;
    case 2:
        switch (mc_delete_dir(w)) {
        case 0:
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

void mc_act_remove(w)
MCW *w;
{
    switch (w->astep) {
    case 0:
        switch (mc_delete_dir(w)) {
        case 0:
            w->ret = 0;
            break;
        case 1:
            w->ret = -0x100;
            break;
        }
        break;
    }
    mc_act_return(w);
}

void McActListSet(port, pattern, buf)
int port;
char *pattern;
u8 *buf;
{
    MemcardWork.astep = 0;
    MemcardWork.step = 0;
    MemcardWork.port = port;
    MemcardWork.act = 9;
    MemcardWork.ret = -1;
    sprintf(MemcardWork.name, lit_421_00384300, pattern);
    MemcardWork.buf = buf;
}

void mc_act_list(w)
MCW *w;
{
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
            break;
        }
        break;
    case 1:
        switch (mc_get_dir(w)) {
        case 0:
            w->astep = 0;
            w->ret = w->cnt;
            break;
        case 1:
            w->astep = 0;
            w->ret = -0x100;
            break;
        case 2:
            w->astep = 0;
            w->ret = -0xFD;
            break;
        }
        break;
    }
    mc_act_return(w);
}

int McActResult(void)
{
    return MemcardWork.ret;
}

int McActConChk(port)
int port;
{
    return MemcardWork.state[port];
}

void McActNewClr(void)
{
    MemcardWork.changed = 0;
}

int McActNewChk(port)
int port;
{
    return MemcardWork.changed & (1 << port);
}

void McActAvailSet(s32 *img)
{
    MCFILE *f = mc_file_tbl[MemcardWork.file];
    int n;
    int sz;
    int k;
    int i;
    int d;
    u8 *base = (u8 *)img;

    if (img != 0) {
        n = img[0];
        k = 1;
        img++;
        for (i = 0; i < n; i++) {
            d = i + 4;
            if (d >= 5) {
                d -= 4;
            }
            MCFI(f, d)->data = base + (k << 11);
            k += *img++;
        }
    }
    n = 0;
    sz = 0;
    for (i = 0; i < 5; i++) {
        if (MCFI(f, i)->on != 0) {
            sz += (MCFI(f, i)->size + 0x3FF) / 1024;
            n++;
        }
    }
    f->blocks = sz + (n + 1) / 2 + 2;
}

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
