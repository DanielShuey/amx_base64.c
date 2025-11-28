#include "amx_base64.h"
#include <stdlib.h>
#include <string.h>
// clang-format off
typedef uint64_t u64; typedef uint16_t u16; typedef int16_t i16; typedef uint8_t u8;
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

typedef enum : u64 { ldst128 = 1ull << 62, ld256 = 1ull << 60 } fldst;
typedef enum : u64 { RSHUF = 7, RMUL = 6, RGEN = 5, RLUT = 4 } tblreg;
typedef enum : u64 { wrevn = 2ull << 32, wrclr = 3ull << 32 } wrmode;
typedef enum : u64 { ysign = 1ull << 26 } fvecint;
typedef enum
    : u64 { dstz = 2ull << 25,
            dsty = 1ull << 25,
            srcy = 1ull << 10,
            tbly = 1ull << 59 } fgenlut;
typedef enum
    : u64 { lu5i8  = 15ull << 53,
            lu5i16 = 14ull << 53,
            gn5u16 = 6ull << 53 } alugnl;
typedef enum
    : u64 { ln8p = 11ull << 42,
            ln8  = 9ull << 42,
            lx8p = 13ull << 11 } lnwidth;
typedef enum
    : u64 { vimac   = 0ull << 47,  // z += (x*y) >> s
            vishift = 4ull << 47,  // z >>= s
            vimul   = 10ull << 47, // (x*y) >> s
            vixac   = 11ull << 47, // z += (x >> s)
    } aluvi;

static inline u64    zshift(u64 n) { return n << 58; }
static inline wrmode wrfirst(u64 n) { return 4ull << 38 | n << 32; }
static inline void   amxset() { nop_op_imm5(17, 0); }
static inline void   amxclr() { nop_op_imm5(17, 1); }
static inline void   ldx64(u64 xr, u64 ptr) { amx(0, xr << 56 | ptr); }
static inline void   ldy64(u64 yr, u64 ptr) { amx(1, yr << 56 | ptr); }
static inline void   ldx256(u64 xr, u64 ptr) { amx(0, ld256 | xr << 56 | ptr); }
static inline void stx128(u64 xr, u64 ptr) { amx(2, ldst128 | xr << 56 | ptr); }

static inline void extrx(lnwidth lx, u64 z, u64 bx, u64 opt)
{
        amx(8, 1ull << 26 | lx | z << 20 | bx | opt);
}
static inline void genlut(alugnl alu, u64 bsrc, u64 tbl, u64 dst, u64 opt)
{
        amx(22, alu | bsrc | tbl << 60 | dst << 20 | opt);
}
static inline void vecint(aluvi alu, lnwidth ln, u64 bx, u64 by, u64 z, u64 opt)
{
        amx(18, alu | ln | bx << 10 | by | z << 20 | opt);
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
                genlut(lu5i8, YR(RSHUF), i, ZR(i), srcy | dstz);
                ldx64(i, s + bsrc);
                genlut(lu5i8, YR(RSHUF), i, i, srcy);
                extrx(0, ZR(i), XR(i, 32), wrfirst(32));
        }
}

static inline void enc_u6()
{
        for (int i = 0; i < 8; i++) {
                vecint(vimul, ln8p, XR(i), YR(RMUL), ZR(i), 0);
                vecint(vishift, ln8, 0, 0, ZR(i, 0), zshift(2));
                vecint(vishift, ln8, 0, 0, ZR(i, 1), zshift(2));
        }
        for (int i = 0; i < 8; i++) {
                vecint(vimac, ln8p, XR(i, 1), YR(RMUL, 1), ZR(i), zshift(8));
        }
}

static inline void encextr()
{
        for (int i = 0; i < 8; i++)
                extrx(lx8p, ZR(i), XR(i), 0);
}

static inline void enclut(int bz)
{
        for (int i = 0; i < 4; i++) {
                extrx(0, ZR(i, bz), XR(i), wrclr);
                extrx(0, ZR(i, bz), XR(i), wrevn);
        }
        for (int i = 0; i < 4; i++) {
                genlut(gn5u16, XR(i), RGEN, i, dsty | tbly);
                genlut(lu5i16, XR(i), RLUT, ZR(i, bz), dstz | srcy | tbly);
                vecint(vixac, 0, XR(i), XR(i), ZR(i, bz), ysign);
        }
}

static inline void encout(const char *buf)
{
        for (int i = 0; i < 8; i += 2)
                stx128(i, buf + XR(i));
}

static inline void encpad(char *buf, int len)
{
        int enclen = amx_base64_encode_len(len);
        int padlen = amx_base64_encode_padding(len);
        memset(buf + enclen - padlen, '=', padlen);
        buf[enclen] = 0;
}

void amx_base64_encode(const char *s, int len, char *buf)
{
        amxset();
        encprep();
        for (u64 mem = buf, end = s + len; s < end; s += 48 * 8, mem += 512) {
                encread(s);
                enc_u6();
                enclut(ZR(0, 0));
                enclut(ZR(0, 1));
                enclut(ZR(4, 0));
                enclut(ZR(4, 1));
                encextr();
                encout(mem);
        }
        encpad(buf, len);
        amxclr();
}

// ╭──────────────────────────────────────────────────────────────────────────╮
// │                            AMX BASE64 DECODE                             │
// ╰──────────────────────────────────────────────────────────────────────────╯
static inline void decread(const char *s)
{
        ldx256(0, s);
        ldx256(4, s + 256);
}

void amx_base64_decode(const char *s, int len, char *buf)
{
        amxset();
        for (u64 mem = buf, end = s + len; s < end; s += 48 * 8, mem += 512) {
                decread(s);
        }
        amxclr();
}
