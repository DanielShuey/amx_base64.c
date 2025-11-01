#pragma once
#include <stdlib.h>

#define buflen(n) ((n + 512 - 1) / 512) * 512
static inline char *b64alloc(int n) { return malloc(buflen(n)); }
void amx_base64_encode(const char *s, int len, char *buf);
#undef buflen
