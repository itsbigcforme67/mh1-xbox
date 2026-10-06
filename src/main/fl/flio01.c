/*  */
/* flio_nm - SLPM_654.95 0x0016F290-0x0016F600 (g_flPS2DmaAddQueue.s): host-file helpers of the fl library: build the
   path "<prefix>NAME<suffix>" (name upper-cased, prefix lit_269_0035BE00, suffix lit_270_0035BE08) in a 0x800 byte stack
   buffer, open it under the ADXM lock and read / write / append / measure. Working file. */
#include "types.h"
extern char lit_269_0035BE00[];
extern char lit_270_0035BE08[];
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
u32 strlen(const char *);
char *strupr(char *);
void ADXM_Lock(void);
void ADXM_Unlock(void);
int sceOpen();
int sceRead();
int sceWrite();
int sceLseek();
int sceClose();
int flFileRead(char *name, void *buf, int size) {
    char path[0x800];
    char *p;
    int fd;

    strcpy(path, lit_269_0035BE00);
    p = path + strlen(path);
    strcat(path, name);
    strupr(p);
    strcat(path, lit_270_0035BE08);
    ADXM_Lock();
    fd = sceOpen(path, 1);
    if (fd < 0) {
        ADXM_Unlock();
        return 0;
    }
    sceRead(fd, buf, size);
    sceClose(fd);
    ADXM_Unlock();
    return 1;
}
int flFileWrite(char *name, void *buf, int size) {
    char path[0x800];
    char *p;
    int fd;

    strcpy(path, lit_269_0035BE00);
    p = path + strlen(path);
    strcat(path, name);
    strupr(p);
    strcat(path, lit_270_0035BE08);
    ADXM_Lock();
    if ((fd = sceOpen(path, 0x602)) < 0) {
        ADXM_Unlock();
        return 0;
    }
    sceWrite(fd, buf, size);
    sceClose(fd);
    ADXM_Unlock();
    return 1;
}
int flFileAppend(char *name, void *buf, int size) {
    char path[0x800];
    char *p;
    int fd;

    strcpy(path, lit_269_0035BE00);
    p = path + strlen(path);
    strcat(path, name);
    strupr(p);
    strcat(path, lit_270_0035BE08);
    ADXM_Lock();
    if ((fd = sceOpen(path, 2)) < 0) {
        ADXM_Unlock();
        return 0;
    }
    sceLseek(fd, 0, 2);
    sceWrite(fd, buf, size);
    sceClose(fd);
    ADXM_Unlock();
    return 1;
}
int flFileLength(char *name) {
    char path[0x800];
    char *p;
    int fd;
    int len;

    strcpy(path, lit_269_0035BE00);
    p = path + strlen(path);
    strcat(path, name);
    strupr(p);
    strcat(path, lit_270_0035BE08);
    ADXM_Lock();
    if ((fd = sceOpen(path, 1)) < 0) {
        ADXM_Unlock();
        return 0;
    }
    len = sceLseek(fd, 0, 2);
    sceClose(fd);
    ADXM_Unlock();
    return len;
}
