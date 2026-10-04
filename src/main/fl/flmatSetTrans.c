/* flmatSetTrans - set the translation row of a 4x4 float matrix.
 * Matches SLPM_654.95 0x00171E90 (16 bytes) with mwcps2 3.0b52 -O4,p. */
void flmatSetTrans(float *m, float x, float y, float z) {
    m[12] = x;
    m[13] = y;
    m[14] = z;
}
