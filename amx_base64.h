#pragma once

#include <stdlib.h>

#define buflen(n) ((n + 512 - 1) / 512) * 512

static inline int amx_base64_encode_padding(int n) { return (3 - (n % 3)) % 3; }

static inline int amx_base64_encode_len(int n) { return ((n + 2) / 3) * 4; }

static inline int amx_base64_decode_len(int n)
{
        return (n / 4) * 3 - amx_base64_encode_padding(n);
}

static inline char *amx_base64_encode_alloc(int n)
{
        return malloc(buflen(amx_base64_encode_len(n)));
}

static inline char *amx_base64_decode_alloc(int n)
{
        return malloc(buflen(amx_base64_decode_len(n)));
}

void amx_base64_decode(const char *s, int len, char *buf);
void amx_base64_encode(const char *s, int len, char *buf);

#undef buflen
