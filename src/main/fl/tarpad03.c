/* tarpad layer: stick / pad helpers (SLPM_654.95 0x00195510-0x001957D0 and 0x001957D0-0x0019583C region): update_pad_stick_dir turns the raw stick (x, y)
 * into angle (radians and 1/360 steps via the soft-float library) and a magnitude capped at 127, zeroing everything below the dead zone;
 * lever_analog_to_digital maps a stick angle to a direction code; PADDeviceInit / PADPortOpen / PADDeviceDestroy open and close the pad2 library sockets.
 * Names are guesses. */
#include "types.h"

typedef struct STICK {
    s16 x;              /* 0x00 */
    s16 y;              /* 0x02 */
    s16 mag;            /* 0x04 */
    s16 deg;            /* 0x06 */
    f32 rad;            /* 0x08 */
} STICK;

double atan2(double, double);
double sqrt(double);

void update_pad_stick_dir(STICK *s, s16 dz) {
    f32 ang;
    int x;
    int y;

    x = s->x;
    y = s->y;
    if ((y | x) == 0) {
        ang = 0.0f;
    } else {
        ang = atan2((double)-y, (double)x);
        if (ang < 0.0f) {
            ang += 6.2831855f;
        }
    }
    s->rad = ang;
    s->deg = 360.0f * (s->rad / 6.2831855f);
    s->mag = (int)sqrt((double)(s->x * s->x) + (double)(s->y * s->y));
    if (s->mag > 127) {
        s->mag = 127;
    }
    if (s->mag < dz) {
        s->x = 0;
        s->y = 0;
        s->rad = 0;
        s->deg = 0;
        s->mag = 0;
    }
}

extern u8 ps2lever_analog_to_digital_604[];

u8 lever_analog_to_digital(STICK *s) {
    u32 i;

    if (s->mag < 0x25) {
        return 0;
    }
    i = s->deg / 22.5f;
    return ps2lever_analog_to_digital_604[i];
}

typedef struct PADSLOT {
    u8 open;            /* 0x00 */
    u8 x1;              /* 0x01 */
    u8 port;            /* 0x02 */
    u8 slot;            /* 0x03 */
    u8 x4[2];
    u8 sock;            /* 0x06 socket id from scePad2CreateSocket */
    u8 x7;
    int buf;            /* 0x08 */
    u8 xC[0x18 - 0xC];
} PADSLOT;

typedef struct PADSOCK {
    int mode;           /* 0x00 */
    int port;           /* 0x04 */
    int slot;           /* 0x08 */
    int x0C;            /* 0x0C */
    int x10[4];
} PADSOCK;

extern PADSLOT ps2slot[2];

int sceDbcInit();
int scePad2Init(int);
int scePad2CreateSocket(PADSOCK *, int);
void scePad2DeleteSocket(int);
void scePad2End();
void sceDbcEnd();

int PADDeviceInit(void) {
    if (sceDbcInit() != 1) {
        return 0;
    }
    if (scePad2Init(0) != 1) {
        return 0;
    }
    return 1;
}

void PADPortOpen(int port, int slot, PADSLOT *p) {
    PADSOCK sk;
    int id;

    sk.x0C = 0;
    sk.port = port;
    sk.slot = slot;
    sk.mode = 2;
    id = scePad2CreateSocket(&sk, p->buf);
    p->sock = id;
    if ((id & 0xFF) >= 0) {
        p->open = 1;
        p->x1 = 0;
        p->port = port;
        p->slot = slot;
    }
}

void PADDeviceDestroy(void) {
    int i;
    PADSLOT *p;

    i = 0;
    p = ps2slot;
    do {
        if (p->open != 0) {
            scePad2DeleteSocket(p->sock);
        }
        i++;
        p++;
    } while (i < 2);
    scePad2End();
    sceDbcEnd();
}
