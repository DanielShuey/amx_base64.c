#include "amx_base64.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char *data;
	int len;
	bool exists;
} file;

static file readfile(const char *filename)
{
	char *buffer = 0;
	long length;
	FILE *f = fopen(filename, "rb");

	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}

	file result;
	if (buffer) {
		result.data = buffer;
		result.len = length;
		result.exists = true;
		return result;
	} else {
		return result;
	}
}

char *b64enc(const char *s)
{
	int len = strlen(s);
	char *buf = b64alloc(len);
	amx_base64_encode(s, len, buf);
	return buf;
}

void test_go_by_example()
{
	// from https://gobyexample.com/base64-encoding
	const char *src = "abc123!?$*&()'-=@~";
	const char *enc = "YWJjMTIzIT8kKiYoKSctPUB+";

	assert(strncmp(b64enc(src), enc, strlen(enc)) == 0);
}

void test_tutorials_point()
{
	// from https://www.tutorialspoint.com/java8/java8_base64.htm
	const char *src = "TutorialsPoint?java8";
	const char *enc = "VHV0b3JpYWxzUG9pbnQ/amF2YTg=";

	assert(strncmp(b64enc(src), enc, strlen(enc)) == 0);
}

void test_wikipedia()
{
	// from Wikipedia page
	const char *src =
	    "Man is distinguished, not only by his reason, but by this singular passion from \
other animals, which is a lust of the mind, that by a perseverance of delight \
in the continued and indefatigable generation of knowledge, exceeds the short \
vehemence of any carnal pleasure.";

	const char *enc =
	    "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz\
IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg\
dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu\
dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo\
ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

	assert(strncmp(b64enc(src), enc, strlen(enc)) == 0);
}

static inline int b64len(int n) { return ((n + 2) / 3) * 4; }
#define buflen(n) ((n + 512 - 1) / 512) * 512

void test_ipsum()
{
	file fsrc = readfile("fixtures/ipsum_4096.txt");
	const char *src = fsrc.data;

	file fenc = readfile("fixtures/ipsum_4096_enc.txt");
	const char *enc = fenc.data;

	assert(strncmp(b64enc(src), enc, strlen(enc) - 1) == 0);
}

int main()
{
	test_go_by_example();
	test_tutorials_point();
	test_wikipedia();
	test_ipsum();
	return 0;
}
