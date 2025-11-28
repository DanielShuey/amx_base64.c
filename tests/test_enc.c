#include "amx_base64.h"
#include <assert.h>

static const unsigned char ipsum4096[] = {
#embed "ipsum_4096.txt"
    , 0};

static const unsigned char ipsum4096enc[] = {
#embed "ipsum_4096_enc.txt"
    , 0};

static char *b64enc(const char *s)
{
	int   len = strlen(s);
	char *buf = amx_base64_encode_alloc(len);
	amx_base64_encode(s, len, buf);
	return buf;
}

static void test_go_by_example()
{
	// from https://gobyexample.com/base64-encoding
	const char *src    = "abc123!?$*&()'-=@~";
	const char *enc    = "YWJjMTIzIT8kKiYoKSctPUB+";
	char       *result = b64enc(src);
	assert(!strncmp(result, enc, strlen(enc)));
	free(result);
}

static void test_tutorials_point()
{
	// from https://www.tutorialspoint.com/java8/java8_base64.htm
	const char *src    = "TutorialsPoint?java8";
	const char *enc    = "VHV0b3JpYWxzUG9pbnQ/amF2YTg=";
	char       *result = b64enc(src);
	assert(!strncmp(result, enc, strlen(enc)));
	free(result);
}

static void test_wikipedia()
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
	char *result = b64enc(src);
	assert(!strncmp(result, enc, strlen(enc)));
	free(result);
}

static void test_ipsum()
{
	char *result = b64enc(ipsum4096);
	assert(!strncmp(result, ipsum4096enc, strlen(ipsum4096enc) - 1));
	free(result);
}

int test_enc()
{
	test_go_by_example();
	test_tutorials_point();
	test_wikipedia();
	test_ipsum();
	return 0;
}
