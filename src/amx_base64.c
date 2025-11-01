#include "amx_base64.h"
#include "amx_helper.h"
#include <string.h>

static constexpr int reg_shuftbl = 7;
static constexpr int reg_mlshift = 6;
static constexpr int reg_genascii = 5;
static constexpr int reg_lutascii = 4;

static inline int b64len(int n) { return ((n + 2) / 3) * 4; }
static inline int b64pad(int n) { return (3 - (n % 3)) % 3; }

static void loadtbls()
{
	constexpr static u8 shuftbl[64] = {0,  4,   1,  6,  41, 192, 28,  4, 146, 90, 128, 53,  7,
	                                   30, 140, 64, 78, 10, 170, 189, 0, 103, 13, 54,  239, 0,
	                                   0,  0,   0,  0,  0,  0,   0,   0, 0,   0,  0,   0,   0,
	                                   0,  0,   0,  0,  0,  0,   0,   0, 0,   0,  0,   0,   0,
	                                   0,  0,   0,  0,  0,  0,   0,   0, 0,   0,  0,   0};

	constexpr static char mlshift[64] = {
	    0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64,
	    16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,
	    0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4,  0,  64, 16, 4};

	constexpr static u16 genascii[32] = {0, 26, 52, 62, 63, 64, 0, 0, 0, 0,    0,
	                                     0, 0,  0,  0,  0,  0,  0, 0, 0, 0,    0,
	                                     0, 0,  0,  0,  0,  0,  0, 0, 0, 65535};

	constexpr static i16 lutascii[32] = {65, 71, -4, -19, -16, 0, 0, 0, 0, 0,   0,
	                                     0,  0,  0,  0,   0,   0, 0, 0, 0, 0,   0,
	                                     0,  0,  0,  0,   0,   0, 0, 0, 0, -127};

	ldy64(reg_mlshift, mlshift);
	ldy64(reg_shuftbl, shuftbl);
	ldy64(reg_genascii, genascii);
	ldy64(reg_lutascii, lutascii);
}

static inline void readin(const char *s)
{
	// 2 shuffles instead of ldx64x2 extrx?
	for (int i = 0; i < 8; i++) {
		int srco = i * 48;
		ldx64(i, s + srco + 24);
		genlut(lu5i8, reg(reg_shuftbl), i, ztile(i), gnl_srcy | gnl_dstz);
		ldx64(i, s + srco);
		genlut(lu5i8, reg(reg_shuftbl), i, i, gnl_srcy);
		extrx(lndef, ztile(i), reg(i, 32), wrfirst(32));
	}
}

static inline void cvtb64()
{
	for (int i = 0; i < 8; i++) {
		vecint(vimul, ln8p, reg(i), reg(reg_mlshift), ztile(i), 0);
		vecint(vizs, ln8, 0, 0, ztile(i, 0), zshift(2));
		vecint(vizs, ln8, 0, 0, ztile(i, 1), zshift(2));
	}
	for (int i = 0; i < 8; i++) {
		vecint(vimac, ln8p, reg(i, 1), reg(reg_mlshift, 1), ztile(i), zshift(8));
	}
}

static inline void extrtxt()
{
	for (int i = 0; i < 8; i++) {
		extrx(lx8p, ztile(i), reg(i), 0);
	}
}

static inline void cvtascii(int z, int zo)
{
	for (int i = 0; i < 4; i++) {
		extrx(lxdef, ztile(i + z, zo), reg(i), wrclr);
		extrx(lxdef, ztile(i + z, zo), reg(i), wrevn);
	}
	for (int i = 0; i < 4; i++) {
		genlut(gn5u16, reg(i), reg_genascii, i, gnl_dsty | gnl_tbly);
		genlut(lu5i16, reg(i), reg_lutascii, ztile(i + z, zo),
		       gnl_dstz | gnl_srcy | gnl_tbly);
		vecint(vixac, lndef, reg(i), reg(i), ztile(i + z, zo), ysign);
	}
}

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

	int enclen = b64len(len);
	int padlen = b64pad(len);

	memset(buf + enclen - padlen, '=', padlen);
	buf[b64len(len)] = 0;

	amxclr();
}
