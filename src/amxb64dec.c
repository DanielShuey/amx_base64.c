#include "amx.h"
#include "amx_base64.h"

// Load
// LUT (Reverse)
// RShift/Sum
// Shufflejoin
// Extrjoin
// Store

static void loadtbls()
{
	constexpr static u8 shuftbl[64] = {
	    0,  4,  1,   6,   41, 192, 28, 4,  146, 90, 128, 53, 7, 30, 140, 64,
	    78, 10, 170, 189, 0,  103, 13, 54, 239, 0,  0,   0,  0, 0,  0,   0,
	    0,  0,  0,   0,   0,  0,   0,  0,  0,   0,  0,   0,  0, 0,  0,   0,
	    0,  0,  0,   0,   0,  0,   0,  0,  0,   0,  0,   0,  0, 0,  0,   0};

	constexpr static char mlshift[64] = {
	    0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4,
	    0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4,
	    0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4,
	    0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4};

	constexpr static u16 genascii[32] = {
	    0, 26, 52, 62, 63, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	    0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 65535};

	constexpr static i16 lutascii[32] = {
	    65, 71, -4, -19, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	    0,  0,  0,  0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -127};

	ldy64(reg_mlshift, mlshift);
	ldy64(reg_shuftbl, shuftbl);
	ldy64(reg_genascii, genascii);
	ldy64(reg_lutascii, lutascii);
}

static inline void readin(const char *s)
{
	for (int i = 0; i < 8; i++) {
		ldx64(i, s + (i * 64));
	}
}

static inline void cvtb64()
{
	// Build reverse table
}

static inline void extrtxt() {}

static inline void cvtascii(int z, int zo) {}

static inline void bufwrite(const char *buf)
{
	stx128(0, buf + 0);
	stx128(2, buf + 128);
	stx128(4, buf + 256);
	stx128(6, buf + 384);
}

void amx_base64_encode(const char *s, int len, char *buf)
{
	int memptr = 0;
	int srcptr = 0;
	amxset();
	loadtbls();

	while (srcptr < len) {
		readin(s + srcptr);
		// cvtascii
		// cvtb64
		cvtb64();
		cvtascii(0, 0);
		cvtascii(0, 1);
		cvtascii(4, 0);
		cvtascii(4, 1);
		extrtxt();
		bufwrite(buf + memptr);
		srcptr += 48 * 8;
		memptr += 512;
	}
	amxclr();
}
