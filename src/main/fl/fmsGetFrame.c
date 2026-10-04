/* fmsGetFrame - read frame `no` from a frame table into *out; returns 1.
 * Matches SLPM_654.95 0x0016AC90 (28 bytes) with mwcps2 3.0b52 -O4,p.
 * FMS layout is a stand-in: frames are u32s from offset 0xC. */
typedef unsigned int u32;

typedef struct FMS {
    char pad[0xC];
    u32 frm[1];
} FMS;

typedef struct FMSOUT {
    u32 frame;
    int no;
} FMSOUT;

int fmsGetFrame(FMS *f, int no, FMSOUT *out) {
    out->frame = f->frm[no];
    out->no = no;
    return 1;
}
