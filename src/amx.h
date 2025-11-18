#include <stdlib.h>
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

static inline void ldx256(u64 reg, const void *ptr)
{
	amx(0, 1ull << 62 | 1ull << 60 | reg << 56 | (u64)ptr);
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
