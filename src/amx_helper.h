#include <stdlib.h>

// clang-format off
#define nop_op_imm5(op, imm5)                                                                      \
	__asm("nop\nnop\nnop\n.word (0x201000 + (%0 << 5) + %1)" : : "i"(op), "i"(imm5) : "memory")

#define amx(op, gpr) \
	__asm(".word (0x201000 + (%0 << 5) + 0%1 - ((0%1 >> 4) * 6))": : "i"(op), "r"((u64)(gpr)) : "memory"); \

static inline void amxset()   { nop_op_imm5(17, 0); }
static inline void amxclr() { nop_op_imm5(17, 1); }
// clang-format on

// Define short types u8, i16, f32, etc
// clang-format off
#define _def_type(n) typedef uint##n##_t u##n; typedef int##n##_t i##n;
_def_type(8) _def_type(16) _def_type(32) _def_type(64);
typedef _Float16 f16; typedef float f32; typedef double f64;
#undef _def_type
// clang-format on

#define zreg(tile, ...) (((u64)(tile) * 8) __VA_OPT__(+(u64)(__VA_ARGS__)))
#define ztile(tile, ...) (((u64)(tile) * 8) __VA_OPT__(+(u64)(__VA_ARGS__)))
#define reg(idx, ...) (((u64)(idx) * 64) __VA_OPT__(+(u64)(__VA_ARGS__)))

constexpr u64 ysign = 1ull << 26;
constexpr u64 gnl_dstz = 2ull << 25;
constexpr u64 gnl_dsty = 1ull << 25;
constexpr u64 gnl_srcy = 1ull << 10;
constexpr u64 gnl_tbly = 1ull << 59;

typedef enum
    : u64 { vimac = 0ull << 47,  // z += (x*y) >> s
	    vimdec = 1ull << 47, // z -= (x*y) >> s
	    visum = 2ull << 47,  // z += (x+y) >> s
	    viadec = 3ull << 47, // z -= (x+y) >> s
	    vizs = 4ull << 47,   // z >>= s
	    vimul = 10ull << 47, // (x*y) >> s
	    vixac = 11ull << 47, // z += (x >> s)
	    viyac = 12ull << 47, // z += (y >> s)
    } aluvi;

typedef enum
    : u64 { lndef = 0,
	    ln16p = 3ull << 42,
	    ln8q = 10ull << 42,
	    ln8p = 11ull << 42,
	    ln8q16e = 12ull << 42,
	    ln16e8q = 13ull << 42,
	    // z only
	    ln8 = 9ull << 42,
	    // extr
	    lxdef = 0,
	    lx8q = 11ull << 11,
	    lx8p = 13ull << 11,
	    lx16p = 9ull << 11,
	    // extry
	    lxydef = 0,
	    lxyi16 = 1,
    } lnwidth;

// convert to constexpr, or functions that take n param
typedef enum : u64 {
	// Single modes
	wrdef = 0ull,
	// Mode0 modes
	wrodd = 1ull << 32,
	wrevn = 2ull << 32,
	wrclr = 3ull << 32,
	wrx0 = 4ull << 32,
	wry0 = 5ull << 32,
	// Multi modes
	wrxreg = 2ull << 32,
	wryreg = 3ull << 32,
	wrxln0 = 6ull << 32,
	wryln0 = 7ull << 32,
} wrmode;

typedef enum {
	lu5i8 = 15ull << 53,
	lu5i16 = 14ull << 53,
	gn5u16 = 6ull << 53,
	gn5i16 = 4ull << 53,
} alugnl;

static inline u64 zshift(u64 n) { return n << 58; }

static inline wrmode wrfirst(u64 n) { return 4ull << 38 | n << 32; }

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

static inline void ldx64(u64 reg, const void *ptr) { amx(0, reg << 56 | (u64)ptr); }

static inline void ldy64(u64 reg, const void *ptr) { amx(1, reg << 56 | (u64)ptr); }

static inline void stx128(u64 reg, const void *ptr) { amx(2, 1ull << 62 | reg << 56 | (u64)ptr); }
