#include "amx_base64.h"
#include <string.h>

// clang-format off
#define nop_op_imm5(op, imm5)                                                                      \
	__asm("nop\nnop\nnop\n.word (0x201000 + (%0 << 5) + %1)" : : "i"(op), "i"(imm5) : "memory")

#define amx(op, gpr) \
	__asm(".word (0x201000 + (%0 << 5) + 0%1 - ((0%1 >> 4) * 6))": : "i"(op), "r"((u64)(gpr)) : "memory");
// clang-format on

typedef uint64_t u64;
typedef uint16_t u16;
typedef int16_t  i16;
typedef uint8_t  u8;

constexpr int reg_shuftbl  = 7;
constexpr int reg_mlshift  = 6;
constexpr int reg_genascii = 5;
constexpr int reg_lutascii = 4;

typedef enum : u64 { wrevn = 2ull << 32, wrclr = 3ull << 32 } wrmode;
typedef enum : u64 { ysign = 1ull << 26 } viopt;

typedef enum
    : u64 { gnl_dstz = 2ull << 25,
	    gnl_dsty = 1ull << 25,
	    gnl_srcy = 1ull << 10,
	    gnl_tbly = 1ull << 59 } gnlopt;

typedef enum
    : u64 { vimac = 0ull << 47,  // z += (x*y) >> s
	    vizs  = 4ull << 47,  // z >>= s
	    vimul = 10ull << 47, // (x*y) >> s
	    vixac = 11ull << 47, // z += (x >> s)
    } aluvi;

typedef enum {
	lu5i8  = 15ull << 53,
	lu5i16 = 14ull << 53,
	gn5u16 = 6ull << 53
} alugnl;

typedef enum
    : u64 { ln8p = 11ull << 42,
	    ln8  = 9ull << 42,
	    lx8p = 13ull << 11 } lnwidth;

#define zreg(tile, ...)  (((u64)(tile) * 8) __VA_OPT__(+(u64)(__VA_ARGS__)))
#define ztile(tile, ...) (((u64)(tile) * 8) __VA_OPT__(+(u64)(__VA_ARGS__)))
#define reg(idx, ...)    (((u64)(idx) * 64) __VA_OPT__(+(u64)(__VA_ARGS__)))

static inline void   amxset() { nop_op_imm5(17, 0); }
static inline void   amxclr() { nop_op_imm5(17, 1); }
static inline u64    zshift(u64 n) { return n << 58; }
static inline wrmode wrfirst(u64 n) { return 4ull << 38 | n << 32; }

static inline void ldx64(u64 reg, const void *ptr)
{
	amx(0, reg << 56 | (u64)ptr);
}

static inline void ldy64(u64 reg, const void *ptr)
{
	amx(1, reg << 56 | (u64)ptr);
}

static inline void stx128(u64 reg, const void *ptr)
{
	amx(2, 1ull << 62 | reg << 56 | (u64)ptr);
}

static inline void genlut(alugnl alu, u64 srco, u64 tbl, u64 dst, u64 opt)
{
	amx(22, alu | srco | tbl << 60 | dst << 20 | opt);
}

static inline void extrx(lnwidth lx, u64 z, u64 dsto, u64 opt)
{
	amx(8, 1ull << 26 | lx | z << 20 | dsto | opt);
}

static inline void vecint(aluvi alu, lnwidth ln, u64 xo, u64 yo, u64 z, u64 opt)
{
	amx(18, alu | ln | xo << 10 | yo | z << 20 | opt);
}

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
	// 2 shuffles instead of ldx64x2 extrx?
	for (int i = 0; i < 8; i++) {
		int srco = i * 48;
		ldx64(i, s + srco + 24);
		genlut(lu5i8, reg(reg_shuftbl), i, ztile(i),
		       gnl_srcy | gnl_dstz);
		ldx64(i, s + srco);
		genlut(lu5i8, reg(reg_shuftbl), i, i, gnl_srcy);
		extrx(0, ztile(i), reg(i, 32), wrfirst(32));
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
		vecint(vimac, ln8p, reg(i, 1), reg(reg_mlshift, 1), ztile(i),
		       zshift(8));
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
		extrx(0, ztile(i + z, zo), reg(i), wrclr);
		extrx(0, ztile(i + z, zo), reg(i), wrevn);
	}
	for (int i = 0; i < 4; i++) {
		genlut(gn5u16, reg(i), reg_genascii, i, gnl_dsty | gnl_tbly);
		genlut(lu5i16, reg(i), reg_lutascii, ztile(i + z, zo),
		       gnl_dstz | gnl_srcy | gnl_tbly);
		vecint(vixac, 0, reg(i), reg(i), ztile(i + z, zo), ysign);
	}
}

static inline void bufwrite(const char *buf)
{
	stx128(0, buf + 0);
	stx128(2, buf + 128);
	stx128(4, buf + 256);
	stx128(6, buf + 384);
}

static inline void b64pad(char *buf, int len)
{
	int enclen = ((len + 2) / 3) * 4;
	int padlen = (3 - (len % 3)) % 3;
	memset(buf + enclen - padlen, '=', padlen);
	buf[enclen] = 0;
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
	b64pad(buf, len);
	amxclr();
}
