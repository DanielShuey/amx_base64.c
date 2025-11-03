#pragma once

#include <stdlib.h>

#define buflen(n) ((n + 512 - 1) / 512) * 512

static inline int base64_encode_len(int n) { return ((n + 2) / 3) * 4; }

static inline char *amx_base64_alloc(int n)
{
	return malloc(buflen(base64_encode_len(n)));
}

void amx_base64_encode(const char *s, int len, char *buf);

#undef buflen
