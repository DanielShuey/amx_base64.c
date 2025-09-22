#pragma once

#include <stddef.h>

struct amx_base64_string {
	int len;
	char data[];
};

typedef struct amx_base64_string amx_base64_encoded;
typedef struct amx_base64_string amx_base64_decoded;

amx_base64_encoded amx_base64_encode(const char *src);
amx_base64_decoded amx_base64_decode(const char *src);
