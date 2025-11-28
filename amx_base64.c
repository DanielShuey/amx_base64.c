#include "amx_base64.h"
#include <stdlib.h>
#include <string.h>
// clang-format off
#define overload __attribute__((overloadable))
typedef uint64_t u64; typedef uint16_t u16; typedef int16_t i16; typedef uint8_t u8; 
typedef amx_base64_result b64; typedef size_t ptr; 
// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                 AMX LIB                                  │
// ╰──────────────────────────────────────────────────────────────────────────╯
#define nop_op_imm5(op, imm5)                                                                      \
	__asm("nop\nnop\nnop\n.word (0x201000 + (%0 << 5) + %1)" : : "i"(op), "i"(imm5) : "memory")
#define amx(op, gpr) \
	__asm(".word (0x201000 + (%0 << 5) + 0%1 - ((0%1 >> 4) * 6))": : "i"(op), "r"((u64)(gpr)) : "memory");
// clang-format on
#define XR(idx, ...) (((u64)(idx) * 64) __VA_OPT__(+(u64)(__VA_ARGS__)))
#define YR(idx, ...) (((u64)(idx) * 64) __VA_OPT__(+(u64)(__VA_ARGS__)))
#define ZR(idx, ...) (((u64)(idx) * 8) __VA_OPT__(+(u64)(__VA_ARGS__)))

typedef enum : u64 { f128 = 1ull << 62, f256 = 1ull << 60 } fldst;
typedef enum : u64 { RSHUF = 7, RMUL = 6, RGEN = 5, RLUT = 4 } tblreg;
typedef enum : u64 { wrevn = 2ull << 32, wrclr = 3ull << 32 } wrmode;
typedef enum : u64 { ysign = 1ull << 26 } fvecint;
typedef enum : u64 {
        dstz = 2ull << 25,
        dsty = 1ull << 25,
        srcy = 1ull << 10,
        tbly = 1ull << 59
} fgenlut;
typedef enum : u64 {
        // vecint
        mac    = 0ull << 47,  // z += (x*y) >> s
        rshift = 4ull << 47,  // z >>= s
        mul    = 10ull << 47, // (x*y) >> s
        xac    = 11ull << 47, // z += (x >> s)

        // genlut
        lut5i8  = 15ull << 53,
        lut5i16 = 14ull << 53,
        gen5u16 = 6ull << 53
} alu;
typedef enum : u64 {
        ln8p  = 11ull << 42,
        ln8   = 9ull << 42,
        xtr8p = 13ull << 11
} lnwidth;

static inline u64    zshift(u64 n) { return n << 58; }
static inline wrmode wrfirst(u64 n) { return 4ull << 38 | n << 32; }
static inline void   amxset() { nop_op_imm5(17, 0); }
static inline void   amxclr() { nop_op_imm5(17, 1); }
static inline void   ldx64(u64 xr, u64 ptr) { amx(0, xr << 56 | ptr); }
static inline void   ldy64(u64 yr, u64 ptr) { amx(1, yr << 56 | ptr); }
static inline void   ldx256(u64 xr, u64 ptr) { amx(0, f256 | xr << 56 | ptr); }
static inline void   stx128(u64 xr, u64 ptr) { amx(2, f128 | xr << 56 | ptr); }

static inline void extrx(lnwidth lx, u64 z, u64 x, u64 opt)
{
        amx(8, 1ull << 26 | lx | z << 20 | x | opt);
}
static inline void genlut(alu alu, u64 src, u64 tbl, u64 xyr, u64 opt)
{
        amx(22, alu | src | tbl << 60 | xyr << 20 | opt);
}
static inline void vecint(alu alu, lnwidth ln, u64 x, u64 y, u64 z, u64 opt)
{
        amx(18, alu | ln | x << 10 | y | z << 20 | opt);
}

// ╭──────────────────────────────────────────────────────────────────────────╮
// │                            AMX BASE64 ENCODE                             │
// ╰──────────────────────────────────────────────────────────────────────────╯
static void encprep()
{
        ldy64(RMUL,
              (u8[64]){0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4,
                       0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4,
                       0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4,
                       0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4, 0, 64, 16, 4});

        ldy64(RSHUF,
              (u8[64]){0,  4,   1,  6,  41, 192, 28,  4, 146, 90, 128, 53,  7,
                       30, 140, 64, 78, 10, 170, 189, 0, 103, 13, 54,  239, 0,
                       0,  0,   0,  0,  0,  0,   0,   0, 0,   0,  0,   0,   0,
                       0,  0,   0,  0,  0,  0,   0,   0, 0,   0,  0,   0,   0,
                       0,  0,   0,  0,  0,  0,   0,   0, 0,   0,  0,   0});

        ldy64(RGEN, (u16[32]){0, 26, 52, 62, 63, 64, 0, 0, 0, 0,    0,
                              0, 0,  0,  0,  0,  0,  0, 0, 0, 0,    0,
                              0, 0,  0,  0,  0,  0,  0, 0, 0, 65535});

        ldy64(RLUT, (i16[32]){65, 71, -4, -19, -16, 0, 0, 0, 0, 0,   0,
                              0,  0,  0,  0,   0,   0, 0, 0, 0, 0,   0,
                              0,  0,  0,  0,   0,   0, 0, 0, 0, -127});
}

static inline void encread(const char *s)
{
        for (int i = 0; i < 8; i++) {
                int bsrc = i * 48;
                ldx64(i, s + bsrc + 24);
                genlut(lut5i8, YR(RSHUF), i, ZR(i), srcy | dstz);
                ldx64(i, s + bsrc);
                genlut(lut5i8, YR(RSHUF), i, i, srcy);
                extrx(0, ZR(i), XR(i, 32), wrfirst(32));
        }
}

static inline void enc_u6()
{
        for (int i = 0; i < 8; i++) {
                vecint(mul, ln8p, XR(i), YR(RMUL), ZR(i), 0);
                vecint(rshift, ln8, 0, 0, ZR(i, 0), zshift(2));
                vecint(rshift, ln8, 0, 0, ZR(i, 1), zshift(2));
        }
        for (int i = 0; i < 8; i++) {
                vecint(mac, ln8p, XR(i, 1), YR(RMUL, 1), ZR(i), zshift(8));
        }
}

static inline void encextr()
{
        for (int i = 0; i < 8; i++)
                extrx(xtr8p, ZR(i), XR(i), 0);
}

static inline void enclut(int bz)
{
        for (int i = 0; i < 4; i++) {
                extrx(0, ZR(i, bz), XR(i), wrclr);
                extrx(0, ZR(i, bz), XR(i), wrevn);
        }
        for (int i = 0; i < 4; i++) {
                genlut(gen5u16, XR(i), RGEN, i, dsty | tbly);
                genlut(lut5i16, XR(i), RLUT, ZR(i, bz), dstz | srcy | tbly);
                vecint(xac, 0, XR(i), XR(i), ZR(i, bz), ysign);
        }
}

static inline void encout(const char *buf)
{
        for (int i = 0; i < 8; i += 2)
                stx128(i, buf + XR(i));
}

static inline ptr buflen(ptr n) { return ((n + 512 - 1) / 512) * 512; }
static inline int b64pad(int n) { return (3 - (n % 3)) % 3; }
static inline ptr enclen(ptr n) { return ((n + 2) / 3) * 4; }
static inline int declen(int n) { return (n / 4) * 3 - b64pad(n); }

static inline void encpad(b64 *b64)
{
        int padlen = b64pad(b64->srclen);
        memset(b64->dat + b64->len - padlen, '=', padlen);
        b64->dat[b64->len] = 0;
}

overload void amx_base64_encode(const char *s, amx_base64_result *b64)
{
        amxset();
        encprep();
        for (u64 buf = b64->dat, end = s + b64->srclen; s < end;
             s += 48 * 8, buf += 512) {
                encread(s);
                enc_u6();
                enclut(ZR(0, 0));
                enclut(ZR(0, 1));
                enclut(ZR(4, 0));
                enclut(ZR(4, 1));
                encextr();
                encout(buf);
        }
        encpad(b64);
        amxclr();
}

overload b64 amx_base64_encode(const char *s)
{
        b64 result;
        result.srclen = strlen(s);
        result.len    = enclen(result.srclen);
        result.dat    = malloc(buflen(result.len));
        amx_base64_encode(s, &result);
        return result;
}

// ╭──────────────────────────────────────────────────────────────────────────╮
// │                            AMX BASE64 DECODE                             │
// ╰──────────────────────────────────────────────────────────────────────────╯
static inline void decread(const char *s)
{
        ldx256(0, s);
        ldx256(4, s + 256);
}

overload void amx_base64_decode(const char *s, amx_base64_result *b64)
{
        amxset();
        for (u64 buf = b64->dat, end = s + b64->srclen; s < end;
             s += 48 * 8, buf += 512) {
                decread(s);
        }
        amxclr();
}
