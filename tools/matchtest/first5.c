/* First matching test, 4 Oct 2026. All five functions byte-match
 * SLPM_654.95 with mwcps2 3.0b52-030722 -O4,p (relocated fields masked by
 * cmp.py). Struct layouts are minimal stand-ins: only the offsets the
 * functions touch are real. Names of globals and types are invented. */
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32;
typedef struct { char pad[0x736]; u8 stg; } PLW;
typedef struct { char pad[0xC]; u16 id; } PLW2;
typedef struct { char pad[140]; } CLAY;
typedef struct { char pad[0x34C1]; u8 master; } SYSW;
extern SYSW sysw;
extern int clay_adr;     /* original name unknown; holds an address */
typedef struct { char pad[0xC]; u32 frm[1]; } FMS;
typedef struct { u32 frame; int no; } FMSOUT;

int Pl_stg_ck_tw(PLW *a, PLW *b) { return a->stg == b->stg; }
int Pl_master_ck(PLW2 *p) { return p->id == sysw.master; }
CLAY *get_clay_ptr(int n) { return (CLAY *)(clay_adr + n * sizeof(CLAY)); }
void flmatSetTrans(float *m, float x, float y, float z) { m[12] = x; m[13] = y; m[14] = z; }
int fmsGetFrame(FMS *f, int no, FMSOUT *o) { o->frame = f->frm[no]; o->no = no; return 1; }
