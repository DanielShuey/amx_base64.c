#pragma once

#include <stddef.h>

typedef struct {
	int len;
	char data[];

} amx_s;

amx_s amx_base64_encode(const char *src);
amx_s amx_base64_decode(const char *src);
