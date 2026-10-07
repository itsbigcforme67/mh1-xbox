/* PS2 memory card access state machines, SLPM_654.95 main 0x27EF60-0x27FDF0.
 * MemcardWork (0xB4 bytes): step at +4, result count at +0xC, port at +0x14,
 * sceMcSync results at +0x18/+0x1C, file name at +0x50, file handle at +0x90.
 * Each mc_* function is a small step machine called every frame: it returns -1
 * while busy, 0 when done, 1 on error (mc_check_card returns the card state).
 * Field names are guesses. */
#include "types.h"
#include "mcw.h"
extern u8 keep_rtc[8];
extern u8 info_attr[];
extern u8 mc_dir[];
extern char mc_path[];
extern char lit_421_00384300[], lit_422_00384308[], lit_423_00384310[];
void *memset();
void *memcpy();
int sceCdReadClock();
int sceMcInit();
int sceMcSync();
int sceMcGetInfo();
int sceMcGetDir(int, int, char *, unsigned, int, void *);
int sceMcOpen();
int sceMcRead();
int sceMcWrite();
int sceMcClose();
int sceMcMkdir();
int sceMcSetFileInfo();
int sceMcFormat();
int sceMcDelete();
void sceScfGetLocalTimefromRTC();
int sprintf();
static int mc_sync();
#define BCD(x) (((x) & 0xF) + ((((s32)(x)) >> 4) & 0xF) * 10)
/* Memory card action layer, SLPM_654.95 main 0x27FDF0-0x280EF0.
 * McActXxxSet() arms one of the mc_act_* machines (index stored in MemcardWork.act, run
 * from McActMain every frame through the table mc_act_jmp); the machine advances
 * MemcardWork.astep through the low level mc_* calls and finally writes MemcardWork.ret:
 * 0 = done, -1 = still busy, -0xFF no card, -0xFE unformatted, -0xFD no file,
 * -0xFC not enough free blocks, -0xFB card full/other, -0x100 I/O error.
 * Field names are guesses (see include/mcw.h). */
extern s8 adx_cont_flag;
extern char lit_421_00384300[], lit_422_00384308[], lit_423_00384310[], lit_808_00384318[];
void ADXM_Lock();
void ADXM_Unlock();
char *strcpy();
int strlen();
static int mc_check_card();
static int mc_check_file();
static int mc_read_file();
static int mc_write_file();
static int mc_mkdir();
static int mc_create_file();
static int mc_attr_file();
static int mc_format();
static int mc_unformat();
static int mc_delete_dir();
static int mc_get_dir();
void McActStopSet();
static void mc_act_return();
static void mc_icon_sys_set();
extern void (*mc_act_jmp[])();
#define MCFI(f, i) ((MCF *)((i) * 16 + (u8 *)(f)))
#define MCFJ(f, i) ((MCF *)(((i) << 4) + (int)(f)))

/* out: u8 weekday, second, minute, hour(?), day, month, u16 year (struct of 8 bytes) */

void MemcardInit(void)
{
    memset(&MemcardWork, 0, 0xB4);
    MemcardWork.nports = 2;
    do {
    } while (sceMcInit() < 0);
    sceCdReadClock(keep_rtc);
}

void McReadClock(out)
u8 *out;
{
    u8 clk[8];
    u16 year;
    int mon;

    if (sceCdReadClock(clk) == 0) {
        memcpy(clk, keep_rtc, 8);
    }
    sceScfGetLocalTimefromRTC(clk);
    *(u16 *)(out + 6) = BCD(clk[7]) + 2000;
    out[5] = BCD(clk[6]);
    out[4] = BCD(clk[5]);
    out[3] = BCD(clk[3]);
    out[2] = BCD(clk[2]);
    out[1] = BCD(clk[1]);
    year = *(u16 *)(out + 6);
    mon = out[5];
    out[0] = (out[4] + (year + year / 4 - year / 100 + year / 400 + (mon * 13 + 8) / 5)) % 7;
}

static int mc_sync(w)
MCW *w;
{
    w->cmd = 0;
    w->res = 0;
    return -(sceMcSync(1, &w->cmd, &w->res) == 0);
}

static int mc_check_card(w)
MCW *w;
{
    int a1;

    switch (w->step) {
    case 0:
        if (mc_sync(w) >= 0) {
            w->step++;
            w->retry = 3;
    case 1:
            if (sceMcGetInfo(w->port, 0, &w->type, &w->free, &w->fmt) < 0) {
                if (--w->retry <= 0) {
                fail:
                    w->step = 0;
                    w->state[w->port] = 0;
                    w->info[w->port] = 0;
                    return 0;
                }
            } else {
                w->step++;
            }
        }
        break;
    case 2:
        if (mc_sync(w) >= 0) {
            switch (w->res) {
            case 0:
                a1 = 1;
            common:
                if (w->type == 2) {
                    if (w->fmt != 0) {
                        w->state[w->port] = a1;
                        w->info[w->port] = w->free;
                        goto done;
                    }
                    goto again;
                }
                goto fail;
            case -1:
                a1 = 2;
                w->changed |= 1 << w->port;
                goto common;
            case -2:
                if (w->type == 2) {
                    w->changed |= 1 << w->port;
                again:
                    if (--w->retry > 0) {
                    retry:
                        w->step = 1;
                        return -1;
                    }
                    w->state[w->port] = 3;
                    w->info[w->port] = 8000;
                    goto done;
                }
                goto fail;
            default:
                if (--w->retry > 0) {
                    goto retry;
                }
                goto fail;
            }
        done:
            w->step = 0;
            return w->state[w->port];
        }
        break;
    }
    return -1;
}

/* out: u8 weekday, second, minute, hour(?), day, month, u16 year (struct of 8 bytes) */

static int mc_check_file(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcGetDir(w->port, 0, w->name, 0, 1, mc_dir) < 0) {
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            w->step = 0;
            return w->res != 1;
        }
        break;
    }
    return -1;
}

static int mc_read_file(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcOpen(w->port, 0, w->name, 1) < 0) {
err:
                w->step = 0;
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            int r = w->res;

            if (r >= 0) {
                w->fd = r;
                if (sceMcRead(w->fd, w->buf, w->len) >= 0) {
                    w->step++;
                    break;
                }
            }
            goto err;
        }
        break;
    case 2:
        if (mc_sync() >= 0) {
            if (w->res >= 0 && sceMcClose(w->fd) >= 0) {
                w->step++;
                break;
            }
            goto err;
        }
        break;
    case 3:
        if (mc_sync() >= 0) {
            w->step = 0;
            return w->res < 0;
        }
        break;
    }
    return -1;
}

static int mc_mkdir(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcMkdir(w->port, 0, w->name) < 0) {
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            if (w->res == -4) {
                w->res = 0;
            }
            w->step = 0;
            return w->res < 0;
        }
        break;
    }
    return -1;
}

static int mc_create_file(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcOpen(w->port, 0, w->name, 0x200) < 0) {
err:
                w->step = 0;
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            if (w->res >= 0 && sceMcClose(w->fd) >= 0) {
                w->step++;
                break;
            }
            goto err;
        }
        break;
    case 2:
        if (mc_sync() >= 0) {
            w->step = 0;
            return w->res < 0;
        }
        break;
    }
    return -1;
}

static int mc_write_file(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcOpen(w->port, 0, w->name, 2) < 0) {
err:
                w->step = 0;
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            int r = w->res;

            if (r >= 0) {
                w->fd = r;
                if (sceMcWrite(w->fd, w->buf, w->len) >= 0) {
                    w->step++;
                    break;
                }
            }
            goto err;
        }
        break;
    case 2:
        if (mc_sync() >= 0) {
            if (w->res >= 0 && sceMcClose(w->fd) >= 0) {
                w->step++;
                break;
            }
            goto err;
        }
        break;
    case 3:
        if (mc_sync() >= 0) {
            w->step = 0;
            return w->res < 0;
        }
        break;
    }
    return -1;
}

static int mc_attr_file(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (w->file >= 2) {
                return 0;
            }
            if (sceMcGetDir(w->port, 0, w->name, 0, 1, info_attr) < 0) {
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            if (w->res < 0) {
                return 1;
            }
            *(u16 *)(info_attr + 0x14) |= 8;
            if (sceMcSetFileInfo(w->port, 0, w->name, info_attr, 4) < 0) {
                w->step = 0;
                return 1;
            }
            w->step++;
        }
        break;
    case 2:
        if (mc_sync() >= 0) {
            w->step = 0;
            return w->res < 0;
        }
        break;
    }
    return -1;
}

static int mc_format(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcFormat(w->port, 0) < 0) {
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            w->step = 0;
            return w->res < 0;
        }
        break;
    }
    return -1;
}

static int mc_unformat()
{
    return 1;
}

static int mc_delete_dir(w)
MCW *w;
{
    int r;

    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (w->x9C == 0) {
                goto rm_self;
            }
            w->cnt = 0;
next:
            sprintf(mc_path, lit_421_00384300, w->name);
            w->step = 1;
    case 1:
            if (sceMcGetDir(w->port, 0, mc_path, w->cnt, 1, mc_dir) < 0) {
err:
                w->step = 0;
                return 1;
            }
            w->step++;
            break;
        }
        break;
    case 2:
        if (mc_sync() >= 0) {
            r = w->res;
            if (r < 0) {
                goto err;
            }
            if (r == 1) {
                w->cnt++;
                if (mc_dir[0x20] == 0x2E) {
                    goto next;
                }
                sprintf(mc_path, lit_422_00384308, w->name, mc_dir + 0x20);
                w->xA0 = 0;
                goto rm;
            }
rm_self:
            sprintf(mc_path, lit_423_00384310, w->name);
            w->xA0 = 1;
rm:
            if (sceMcDelete(w->port, 0, mc_path) < 0) {
                goto err;
            }
            w->step = 3;
            break;
        }
        break;
    case 3:
        if (mc_sync() >= 0) {
            if (w->res < 0) {
                goto err;
            }
            w->step = 0;
            if (w->xA0 == 0) {
                goto next;
            }
            return 0;
        }
        break;
    }
    return -1;
}

/* out: u8 weekday, second, minute, hour(?), day, month, u16 year (struct of 8 bytes) */

static int mc_get_dir(w)
MCW *w;
{
    switch (w->step) {
    case 0:
        if (mc_sync() >= 0) {
            if (sceMcGetDir(w->port, 0, w->name, 0, 0x100, w->buf) < 0) {
                return 1;
            }
            w->step++;
        }
        break;
    case 1:
        if (mc_sync() >= 0) {
            int r;

            w->step = 0;
            r = w->res;
            if (r < 0) {
                w->cnt = 0;
                return 2;
            }
            w->cnt = r;
            return 0;
        }
        break;
    }
    return -1;
}

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

static void mc_act_stop(void)
{
}

static void mc_act_return(w)
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

static void mc_act_check(w)
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

static void mc_act_load(w)
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

static void mc_act_save0(w)
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

static void mc_act_save(w)
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

static void mc_act_format(w)
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

static void mc_act_unformat(w)
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

static void mc_act_delete(w)
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

static void mc_act_remove(w)
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

static void mc_act_list(w)
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

/* original bytes: build/raw/McActAvailSet.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
asm void McActAvailSet(s32 *img)
{
#include "McActAvailSet.inc"
}
#endif


static void mc_icon_sys_set(w)
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

