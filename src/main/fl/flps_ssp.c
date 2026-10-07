/* fl library shader parameter lookup (SLPM_654.95 0x00179DD0-0x0017BE18): flPS2SetShaderParam picks the vertex-shader parameter table for a
 * render state (SHP: two flag words ORed together, a key word whose top byte and low 24 bits select the shader family and variant) and stores the
 * table index and a pointer to the table (param_NNNN_*, data in main) into the request. A pure compare ladder: nested switches, case labels
 * in the order the original ladder tests them reversed. Struct field names are guesses. */
#include "types.h"
typedef long s64;
typedef unsigned long u64;
typedef struct SHP { u8 x0[4]; s32 fa; s32 fb; s32 key; u8 x10[0x48-0x10]; s32 idx; void *tab; } SHP;
extern u8 param_0001_0002_200[];
extern u8 param_0001_0004_204[];
extern u8 param_0001_0006_207[];
extern u8 param_0002_0002_212[];
extern u8 param_0002_0006_215[];
extern u8 param_0003_0002_220[];
extern u8 param_0003_0004_224[];
extern u8 param_0004_0002_229[];
extern u8 param_0004_0004_233[];
extern u8 param_0005_0002_238[];
extern u8 param_0006_0002_243[];
extern u8 param_0007_0002_248[];
extern u8 param_0007_0004_252[];
extern u8 param_0008_0002_257[];
extern u8 param_0008_0006_260[];
extern u8 param_0009_0002_265[];
extern u8 param_0009_0006_268[];
extern u8 param_000a_0002_273[];
extern u8 param_000a_0004_277[];
extern u8 param_000b_0002_282[];
extern u8 param_000b_0004_286[];
extern u8 param_000c_0002_291[];
extern u8 param_000c_0004_295[];
extern u8 param_000d_0002_300[];
extern u8 param_000d_0004_304[];
extern u8 param_000e_0002_309[];
extern u8 param_000e_0004_313[];
extern u8 param_000f_0002_318[];
extern u8 param_000f_0006_321[];
extern u8 param_0010_0002_326[];
extern u8 param_0010_0006_329[];
extern u8 param_0011_0002_334[];
extern u8 param_0011_0006_337[];
extern u8 param_0012_0002_342[];
extern u8 param_0012_0006_345[];
extern u8 param_0013_0002_350[];
extern u8 param_0013_0004_354[];
extern u8 param_0014_0002_359[];
extern u8 param_0014_0004_363[];
extern u8 param_0015_0002_368[];
extern u8 param_0016_0002_373[];
extern u8 param_0016_0004_377[];
extern u8 param_0017_0002_382[];
extern u8 param_0018_0002_387[];
extern u8 param_0019_0002_392[];
extern u8 param_001a_0002_397[];
extern u8 param_001b_0002_402[];
extern u8 param_001c_0002_407[];
extern u8 param_001d_0002_412[];
extern u8 param_001d_0004_416[];
extern u8 param_001e_0002_421[];
extern u8 param_001f_0002_426[];
extern u8 param_0020_0002_431[];
extern u8 param_0021_0002_436[];
extern u8 param_0022_0002_441[];
extern u8 param_0023_0002_446[];
extern u8 param_0024_0002_451[];
extern u8 param_0025_0002_456[];
extern u8 param_0026_0002_461[];
extern u8 param_0026_0004_465[];
extern u8 param_0027_0002_470[];
extern u8 param_0028_0002_475[];
extern u8 param_0029_0002_480[];
extern u8 param_002a_0002_485[];
extern u8 param_002b_0002_490[];
extern u8 param_002c_0002_495[];
extern u8 param_002f_0002_500[];
extern u8 param_0030_0002_505[];
extern u8 param_0031_0002_510[];
extern u8 param_0032_0002_515[];
extern u8 param_0033_0002_520[];
extern u8 param_0034_0002_525[];
extern u8 param_0037_0002_530[];
extern u8 param_0038_0002_535[];
extern u8 param_0038_0004_539[];
extern u8 param_0039_0002_544[];
extern u8 param_0039_0004_548[];
extern u8 param_003a_0004_554[];
extern u8 param_003b_0002_559[];
extern u8 param_003b_0004_563[];
extern u8 param_003c_0002_568[];
extern u8 param_003d_0002_573[];
extern u8 param_003e_0002_578[];
extern u8 param_003f_0002_583[];
extern u8 param_0040_0002_588[];
extern u8 param_0040_0004_592[];
extern u8 param_0041_0002_597[];
extern u8 param_0041_0004_601[];
extern u8 param_0042_0002_606[];
extern u8 param_0042_0004_610[];
extern u8 param_0043_0002_615[];
extern u8 param_0043_0004_619[];
extern u8 param_0044_0002_624[];
extern u8 param_0044_0004_628[];
extern u8 param_0045_0002_633[];
extern u8 param_0045_0004_637[];
extern u8 param_0046_0002_642[];
extern u8 param_0046_0004_646[];
extern u8 param_0047_0002_651[];
extern u8 param_0047_0004_655[];
extern u8 param_0048_0002_660[];
extern u8 param_0048_0004_664[];
extern u8 param_0049_0004_670[];
extern u8 param_004a_0002_675[];
extern u8 param_004d_0002_680[];
extern u8 param_004d_0004_684[];
extern u8 param_004e_0002_689[];
extern u8 param_004e_0004_693[];
extern u8 param_004f_0002_698[];
extern u8 param_004f_0004_702[];
extern u8 param_0050_0002_707[];
extern u8 param_0050_0004_711[];
extern u8 param_0051_0002_716[];
extern u8 param_0051_0004_720[];
extern u8 param_0052_0002_725[];
extern u8 param_0052_0004_729[];
extern u8 param_0053_0002_734[];
extern u8 param_0054_0002_739[];
extern u8 param_0055_0002_744[];
extern u8 param_0056_0002_749[];
extern u8 param_0057_0002_754[];
extern u8 param_0058_0002_759[];
extern u8 param_1001_0006_766[];
extern u8 param_5000_0004_772[];
extern u8 param_7001_0006_779[];

void flPS2SetShaderParam(SHP *p) {
    int c;
    u64 key;
    int f;
    int hi;

    p->idx = -1;
    p->tab = 0;
    c = p->key;
    hi = c & 0xFF000000;
    key = (u64)c & 0xFFFFFF;
    f = p->fa | p->fb;
    switch (hi) {
    case 0x0:
        switch (key) {
        case 0x202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0;
                p->tab = param_0001_0002_200;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 1;
                p->tab = param_0001_0004_204;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 2;
                p->tab = param_0001_0006_207;
                break;
            }
            break;
        case 0x20202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 3;
                p->tab = param_0002_0002_212;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 4;
                p->tab = param_0002_0006_215;
                break;
            }
            break;
        case 0x222:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 5;
                p->tab = param_0003_0002_220;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 6;
                p->tab = param_0003_0004_224;
                break;
            }
            break;
        case 0x20020:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 7;
                p->tab = param_0004_0002_229;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 8;
                p->tab = param_0004_0004_233;
                break;
            }
            break;
        case 0x80:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 9;
                p->tab = param_0005_0002_238;
                break;
            }
            break;
        case 0x20080:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0xA;
                p->tab = param_0006_0002_243;
                break;
            }
            break;
        case 0x20E:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0xB;
                p->tab = param_0007_0002_248;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0xC;
                p->tab = param_0007_0004_252;
                break;
            }
            break;
        case 0x200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0xD;
                p->tab = param_0008_0002_257;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 0xE;
                p->tab = param_0008_0006_260;
                break;
            }
            break;
        case 0x20200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0xF;
                p->tab = param_0009_0002_265;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 0x10;
                p->tab = param_0009_0006_268;
                break;
            }
            break;
        case 0x8802:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x11;
                p->tab = param_000a_0002_273;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x12;
                p->tab = param_000a_0004_277;
                break;
            }
            break;
        case 0x8702:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x13;
                p->tab = param_000b_0002_282;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x14;
                p->tab = param_000b_0004_286;
                break;
            }
            break;
        case 0xC806:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x15;
                p->tab = param_000c_0002_291;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x16;
                p->tab = param_000c_0004_295;
                break;
            }
            break;
        case 0xC70A:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x17;
                p->tab = param_000d_0002_300;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x18;
                p->tab = param_000d_0004_304;
                break;
            }
            break;
        case 0x2C8A0:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x19;
                p->tab = param_000e_0002_309;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x1A;
                p->tab = param_000e_0004_313;
                break;
            }
            break;
        case 0x0:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x1B;
                p->tab = param_000f_0002_318;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 0x1C;
                p->tab = param_000f_0006_321;
                break;
            }
            break;
        case 0x2:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x1D;
                p->tab = param_0010_0002_326;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 0x1E;
                p->tab = param_0010_0006_329;
                break;
            }
            break;
        case 0x20000:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x1F;
                p->tab = param_0011_0002_334;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 0x20;
                p->tab = param_0011_0006_337;
                break;
            }
            break;
        case 0x20002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x21;
                p->tab = param_0012_0002_342;
                break;
            case 0x21025:
            case 0x121025:
                p->idx = 0x22;
                p->tab = param_0012_0006_345;
                break;
            }
            break;
        case 0x8222:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x23;
                p->tab = param_0013_0002_350;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x24;
                p->tab = param_0013_0004_354;
                break;
            }
            break;
        case 0x2A2:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x25;
                p->tab = param_0014_0002_359;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x26;
                p->tab = param_0014_0004_363;
                break;
            }
            break;
        case 0x282:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x27;
                p->tab = param_0015_0002_368;
                break;
            }
            break;
        case 0x8202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x28;
                p->tab = param_0016_0002_373;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x29;
                p->tab = param_0016_0004_377;
                break;
            }
            break;
        case 0x28202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x2A;
                p->tab = param_0017_0002_382;
                break;
            }
            break;
        case 0x8200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x2B;
                p->tab = param_0018_0002_387;
                break;
            }
            break;
        case 0x8000:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x2C;
                p->tab = param_0019_0002_392;
                break;
            }
            break;
        case 0x8002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x2D;
                p->tab = param_001a_0002_397;
                break;
            }
            break;
        case 0x28200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x2E;
                p->tab = param_001b_0002_402;
                break;
            }
            break;
        case 0x28000:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x2F;
                p->tab = param_001c_0002_407;
                break;
            }
            break;
        case 0x28002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x30;
                p->tab = param_001d_0002_412;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x31;
                p->tab = param_001d_0004_416;
                break;
            }
            break;
        case 0x4202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x32;
                p->tab = param_001e_0002_421;
                break;
            }
            break;
        case 0x24202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x33;
                p->tab = param_001f_0002_426;
                break;
            }
            break;
        case 0x4200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x34;
                p->tab = param_0020_0002_431;
                break;
            }
            break;
        case 0x24200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x35;
                p->tab = param_0021_0002_436;
                break;
            }
            break;
        case 0x4002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x36;
                p->tab = param_0022_0002_441;
                break;
            }
            break;
        case 0x24002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x37;
                p->tab = param_0023_0002_446;
                break;
            }
            break;
        case 0x4000:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x38;
                p->tab = param_0024_0002_451;
                break;
            }
            break;
        case 0x24000:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x39;
                p->tab = param_0025_0002_456;
                break;
            }
            break;
        case 0xC202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x3A;
                p->tab = param_0026_0002_461;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x3B;
                p->tab = param_0026_0004_465;
                break;
            }
            break;
        case 0x2C202:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x3C;
                p->tab = param_0027_0002_470;
                break;
            }
            break;
        case 0xC200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x3D;
                p->tab = param_0028_0002_475;
                break;
            }
            break;
        case 0x2C200:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x3E;
                p->tab = param_0029_0002_480;
                break;
            }
            break;
        case 0xC002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x3F;
                p->tab = param_002a_0002_485;
                break;
            }
            break;
        case 0x2C002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x40;
                p->tab = param_002b_0002_490;
                break;
            }
            break;
        case 0xC000:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x41;
                p->tab = param_002c_0002_495;
                break;
            }
            break;
        case 0x2C000:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x43;
                p->tab = param_002f_0002_500;
                break;
            }
            break;
        case 0x28080:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x44;
                p->tab = param_0030_0002_505;
                break;
            }
            break;
        case 0x2C080:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x45;
                p->tab = param_0031_0002_510;
                break;
            }
            break;
        case 0x2C280:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x46;
                p->tab = param_0032_0002_515;
                break;
            }
            break;
        case 0x302:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x47;
                p->tab = param_0033_0002_520;
                break;
            }
            break;
        case 0x402:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x48;
                p->tab = param_0034_0002_525;
                break;
            }
            break;
        case 0x28022:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x49;
                p->tab = param_0037_0002_530;
                break;
            }
            break;
        case 0x4222:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x4A;
                p->tab = param_0038_0002_535;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x4B;
                p->tab = param_0038_0004_539;
                break;
            }
            break;
        case 0x24020:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x4C;
                p->tab = param_0039_0002_544;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x4D;
                p->tab = param_0039_0004_548;
                break;
            }
            break;
        case 0x240A0:
            switch (f) {
            case 0x321135:
            case 0x221135:
                p->idx = 0x4E;
                p->tab = param_003a_0004_554;
                break;
            }
            break;
        case 0x200A0:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x4F;
                p->tab = param_003b_0002_559;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x50;
                p->tab = param_003b_0004_563;
                break;
            }
            break;
        case 0x28020:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x51;
                p->tab = param_003c_0002_568;
                break;
            }
            break;
        case 0x8080:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x52;
                p->tab = param_003d_0002_573;
                break;
            }
            break;
        case 0x4080:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x53;
                p->tab = param_003e_0002_578;
                break;
            }
            break;
        case 0xC080:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x54;
                p->tab = param_003f_0002_583;
                break;
            }
            break;
        case 0x878A:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x55;
                p->tab = param_0040_0002_588;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x56;
                p->tab = param_0040_0004_592;
                break;
            }
            break;
        case 0x8782:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x57;
                p->tab = param_0041_0002_597;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x58;
                p->tab = param_0041_0004_601;
                break;
            }
            break;
        case 0x870A:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x59;
                p->tab = param_0042_0002_606;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x5A;
                p->tab = param_0042_0004_610;
                break;
            }
            break;
        case 0x2C800:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x5B;
                p->tab = param_0043_0002_615;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x5C;
                p->tab = param_0043_0004_619;
                break;
            }
            break;
        case 0xC886:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x5D;
                p->tab = param_0044_0002_624;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x5E;
                p->tab = param_0044_0004_628;
                break;
            }
            break;
        case 0xC78A:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x5F;
                p->tab = param_0045_0002_633;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x60;
                p->tab = param_0045_0004_637;
                break;
            }
            break;
        case 0x28800:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x61;
                p->tab = param_0046_0002_642;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x62;
                p->tab = param_0046_0004_646;
                break;
            }
            break;
        case 0x8882:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x63;
                p->tab = param_0047_0002_651;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x64;
                p->tab = param_0047_0004_655;
                break;
            }
            break;
        case 0x802:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x65;
                p->tab = param_0048_0002_660;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x66;
                p->tab = param_0048_0004_664;
                break;
            }
            break;
        case 0xC802:
            switch (f) {
            case 0x321135:
            case 0x221135:
                p->idx = 0x67;
                p->tab = param_0049_0004_670;
                break;
            }
            break;
        case 0x24300:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x68;
                p->tab = param_004a_0002_675;
                break;
            }
            break;
        case 0x2C8A6:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x69;
                p->tab = param_004d_0002_680;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x6A;
                p->tab = param_004d_0004_684;
                break;
            }
            break;
        case 0x2C822:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x6B;
                p->tab = param_004e_0002_689;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x6C;
                p->tab = param_004e_0004_693;
                break;
            }
            break;
        case 0x2C802:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x6D;
                p->tab = param_004f_0002_698;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x6E;
                p->tab = param_004f_0004_702;
                break;
            }
            break;
        case 0x28802:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x6F;
                p->tab = param_0050_0002_707;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x70;
                p->tab = param_0050_0004_711;
                break;
            }
            break;
        case 0x2C880:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x71;
                p->tab = param_0051_0002_716;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x72;
                p->tab = param_0051_0004_720;
                break;
            }
            break;
        case 0x2C820:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x73;
                p->tab = param_0052_0002_725;
                break;
            case 0x321135:
            case 0x221135:
                p->idx = 0x74;
                p->tab = param_0052_0004_729;
                break;
            }
            break;
        case 0x2C380:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x75;
                p->tab = param_0053_0002_734;
                break;
            }
            break;
        case 0x24380:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x76;
                p->tab = param_0054_0002_739;
                break;
            }
            break;
        case 0x2C480:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x77;
                p->tab = param_0055_0002_744;
                break;
            }
            break;
        case 0x24480:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x78;
                p->tab = param_0056_0002_749;
                break;
            }
            break;
        case 0x24080:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x79;
                p->tab = param_0057_0002_754;
                break;
            }
            break;
        case 0x280A0:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x7A;
                p->tab = param_0058_0002_759;
                break;
            }
            break;
        }
        break;
    case 0x1000000:
        switch (key) {
        case 0x28002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x7B;
                p->tab = param_1001_0006_766;
                break;
            }
            break;
        }
        break;
    case 0x6000000:
        switch (f) {
        case 0x321135:
        case 0x221135:
            p->idx = 0x7C;
            p->tab = param_5000_0004_772;
            break;
        }
        break;
    case 0x8000000:
        switch (key) {
        case 0x28002:
            switch (f) {
            case 0x21035:
            case 0x121035:
                p->idx = 0x7D;
                p->tab = param_7001_0006_779;
                break;
            }
            break;
        }
        break;
    }
}
