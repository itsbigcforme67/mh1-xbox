/* cam.h - game camera (f_cam, f_cam_223B50): CameraWork and its five
 * camera slots. Field names are mostly by offset; meanings are guesses from
 * the code that uses them. */
#ifndef CAM_H
#define CAM_H
#include "types.h"
#include "pl.h"

/* One zoom entry of a camera config (32 bytes). */
typedef struct CAMCNFE {
    f32 x00;            /* 0x00 */
    f32 y;              /* 0x04 eye offset up (rotated by the camera yaw) */
    f32 z;              /* 0x08 eye offset back */
    f32 x0C;            /* 0x0C */
    f32 tar_y;          /* 0x10 target height above the player */
    f32 x14;            /* 0x14 */
    f32 gnd;            /* 0x18 eye kept this far above the ground */
    f32 x1C;            /* 0x1C */
} CAMCNFE;

/* Camera config: angle of view and roll, then the zoom levels. */
typedef struct CAMCNF {
    f32 fov;            /* 0x00 */
    f32 roll;           /* 0x04 */
    u8 _pad08[0x20 - 0x08];
    CAMCNFE e[7];       /* 0x20 zoom levels (CameraWork.zoom), [4] = special */
} CAMCNF;

/* Camera area (stage camera data, CameraWork.area). */
typedef struct CAMAREA {
    u8 x00;             /* 0x00 */
    u8 no;              /* 0x01 area number */
    u8 type;            /* 0x02 0 = follow player, 1 fixed, 2 rail, 3 fixed offset */
    u8 _pad03[0x1C - 0x03];
    u8 *blend;          /* 0x1C 16 pairs (from area, frames to blend) */
    union {
        CAMCNF cnf;     /* 0x20 type 0 */
        struct {
            f32 pos[3]; /* 0x20 eye (type 1) or offset from the player (type 3) */
            u8 _pad2C[0x50 - 0x2C];
            f32 fov;    /* 0x50 */
            f32 roll;   /* 0x54 */
        } fix;
    } u;
} CAMAREA;

/* Per-mode work at slot+0x90 (0x70 bytes). */
typedef struct CAMD_STD {
    f32 eye[3];         /* 0x00 */
    f32 tar[3];         /* 0x0C */
    f32 old[3];         /* 0x18 eye last frame */
    f32 gnd;            /* 0x24 ground under the target */
    s8 gflag;           /* 0x28 */
    u8 _pad29[0x30 - 0x29];
    f32 eye_d[3];       /* 0x30 wanted eye */
    f32 tar_d[3];       /* 0x3C wanted target */
    f32 fov;            /* 0x48 */
    f32 fov_d;          /* 0x4C */
    f32 roll;           /* 0x50 */
    f32 roll_d;         /* 0x54 */
    CAMCNF *cnf;        /* 0x58 */
    CAMCNFE *cnfe;      /* 0x5C current zoom entry */
    f32 hit_h;          /* 0x60 eye raised by k_HitWallCamera */
    f32 rate;           /* 0x64 blend step */
    s16 ang;            /* 0x68 wanted yaw */
    u16 on;             /* 0x6A buttons held */
    u16 trg;            /* 0x6C buttons pressed */
    u8 x6E;             /* 0x6E */
    u8 wall;            /* 0x6F GetWallHitLine result */
} CAMD_STD;

typedef struct CAMD_STG {
    f32 eye[3];         /* 0x00 */
    f32 tar[3];         /* 0x0C */
    f32 eye_f[3];       /* 0x18 blend from */
    f32 tar_f[3];       /* 0x24 */
    f32 fov;            /* 0x30 */
    f32 roll;           /* 0x34 */
    f32 fov_f;          /* 0x38 */
    f32 roll_f;         /* 0x3C */
    u8 _pad40[0x42 - 0x40];
    s16 cnt;            /* 0x42 blend frames left */
    f32 rate;           /* 0x44 */
    s16 vx;             /* 0x48 pitch speed */
    s16 vy;             /* 0x4A yaw speed */
} CAMD_STG;

typedef struct CAMD_PCH {
    f32 m[4][4];        /* 0x00 cannon matrix */
    f32 pos[3];         /* 0x40 player position last frame */
    f32 min;            /* 0x4C fov range */
    f32 max;            /* 0x50 */
    f32 range;          /* 0x54 */
    f32 fov0;           /* 0x58 */
    f32 fov1;           /* 0x5C */
    s16 ang;            /* 0x60 */
    s8 type;            /* 0x62 PachiTypeCheck */
    u8 x63;             /* 0x63 */
    u8 zoom;            /* 0x64 */
} CAMD_PCH;

typedef struct CAMFISH {
    u8 step;            /* 0x00 */
    s32 on;             /* 0x04 */
    void *fish;         /* 0x08 */
} CAMFISH;

typedef struct CAMZOOM {
    u8 step;            /* 0x00 */
    PLW *npc;           /* 0x04 */
    f32 pos[3];         /* 0x08 */
    f32 tar[3];         /* 0x14 */
    u16 req;            /* 0x20 */
    s16 cnt;            /* 0x22 */
} CAMZOOM;

typedef struct CAMD_EX {
    u8 _pad00[4];
    CAMFISH fish;       /* 0x04 */
    CAMZOOM zoom;       /* 0x10 */
} CAMD_EX;

typedef struct CAMD_DEMO {
    f32 eye[3];         /* 0x00 */
    f32 eye_o[3];       /* 0x0C */
    f32 tar[3];         /* 0x18 */
    f32 tar_o[3];       /* 0x24 */
    f32 roll;           /* 0x30 */
    f32 roll_o;         /* 0x34 */
    f32 fov;            /* 0x38 */
    f32 fov_o;          /* 0x3C */
    s8 *cmd;            /* 0x40 command stream */
    s8 *loop;           /* 0x44 */
    u8 move;            /* 0x48 */
    u8 pos_mode;        /* 0x49 */
    u8 tar_mode;        /* 0x4A */
    u8 pos_part;        /* 0x4B */
    u8 tar_part;        /* 0x4C */
    u8 x4D;             /* 0x4D */
    u8 no;              /* 0x4E demo camera number */
    s8 state;           /* 0x4F 1 running, -1 failed */
    s32 arg;            /* 0x50 */
    f32 x54;            /* 0x54 */
    f32 x58;            /* 0x58 */
    u8 stop;            /* 0x5C */
} CAMD_DEMO;

/* One camera (0x100 bytes). The head has the same layout as CAMW's. */
typedef struct CAMS {
    f32 eye[3];         /* 0x00 */
    f32 tar[3];         /* 0x0C */
    u8 _pad18[0x24 - 0x18];
    f32 vec[3];         /* 0x24 eye - target (cam_sub_stg) */
    f32 eye_f[3];       /* 0x30 blend from */
    f32 tar_f[3];       /* 0x3C */
    u8 _pad48[0x60 - 0x48];
    f32 roll;           /* 0x60 */
    f32 roll_f;         /* 0x64 */
    f32 fov;            /* 0x68 */
    f32 fov_f;          /* 0x6C */
    u8 no;              /* 0x70 slot number */
    u8 act;             /* 0x71 this camera wants the view */
    u8 req;             /* 0x72 slot to take the yaw from, -1 none */
    u8 _pad73;
    s16 cnt;            /* 0x74 */
    s16 cnt_max;        /* 0x76 */
    union {
        s32 w;
        u8 b;
    } mode;             /* 0x78 */
    union {
        s32 w;
        u8 b;
    } step;             /* 0x7C */
    s16 ax;             /* 0x80 */
    s16 ay;             /* 0x82 */
    s16 ax0;            /* 0x84 */
    s16 ay0;            /* 0x86 */
    s16 ang;            /* 0x88 yaw */
    u8 _pad8A[0x90 - 0x8A];
    union {
        u8 b[0x70];
        CAMD_STD std;
        CAMD_STG stg;
        CAMD_PCH pch;
        CAMD_EX ex;
        CAMD_DEMO demo;
    } d;                /* 0x90 */
} CAMS;

typedef struct CAMQUAKE {
    f32 pos[3];         /* 0x00 */
    u8 on;              /* 0x0C */
    u8 type;            /* 0x0D 0x80 = everywhere */
    s16 time;           /* 0x0E */
} CAMQUAKE;

typedef struct CAMW {
    f32 eye[3];         /* 0x000 resulting view */
    f32 tar[3];         /* 0x00C */
    u8 _pad018[0x60 - 0x18];
    f32 roll;           /* 0x060 */
    u8 _pad064[0x68 - 0x64];
    f32 fov;            /* 0x068 */
    u8 _pad06C[0x80 - 0x6C];
    CAMS sl[5];         /* 0x080 std, stage, pachinger, player EX, demo */
    union {
        s32 w;
        u8 b;
    } init;             /* 0x580 */
    PLW *pl;            /* 0x584 */
    void *data;         /* 0x588 SetCameraData */
    CAMAREA *area;      /* 0x58C */
    u8 x590[0x18];      /* 0x590 Get_cam_grid_XZ */
    s16 gx;             /* 0x5A8 */
    s16 gz;             /* 0x5AA */
    f32 rail_t;         /* 0x5AC */
    f32 rail_u;         /* 0x5B0 */
    u8 rail_no;         /* 0x5B4 */
    u8 _pad5B5[0x5BC - 0x5B5];
    CAMQUAKE qk[2];     /* 0x5BC [1] pachinger */
    s8 grid;            /* 0x5DC */
    u8 area_no;         /* 0x5DD */
    u8 area_old;        /* 0x5DE */
    s8 area_chg;        /* 0x5DF */
    u8 cam_old;         /* 0x5E0 */
    u8 cam_no;          /* 0x5E1 0 = std, 1 = stage */
    u16 sw_on;          /* 0x5E2 */
    u16 sw_trg;         /* 0x5E4 */
    u16 sw_x5E6;        /* 0x5E6 */
    u16 an_ang;         /* 0x5E8 analog stick */
    u16 an_pow;         /* 0x5EA */
    s32 reset;          /* 0x5EC (WyvernFindPlayer stores a pointer) */
    s32 demo_arg;       /* 0x5F0 */
    u8 demo_req;        /* 0x5F4 */
    u8 _pad5F5;
    u8 zoom;            /* 0x5F6 */
    u8 _pad5F7;
} CAMW;

extern CAMW CameraWork;

#endif
