#pragma once

#include <stdlib.h>

#define overload __attribute__((overloadable))

typedef struct {
        char  *dat;
        size_t len;
        size_t srclen;
} amx_base64_result;

overload void amx_base64_encode(const char *s, amx_base64_result *b64);

overload amx_base64_result amx_base64_encode(const char *s);

#undef overload
