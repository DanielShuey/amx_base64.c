#include "../amx_base64.h"
#include <assert.h>

static const unsigned char ipsum4096[] = {
#embed "ipsum_4096.txt"
    , 0};

static const unsigned char ipsum4096enc[] = {
#embed "ipsum_4096_enc.txt"
    , 0};

const char wiki_src[] =
    "Man is distinguished, not only by his reason, but by this singular passion from \
other animals, which is a lust of the mind, that by a perseverance of delight \
in the continued and indefatigable generation of knowledge, exceeds the short \
vehemence of any carnal pleasure.";

const char wiki_enc[] =
    "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz\
IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg\
dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu\
dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo\
ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

static void test(const char *input, const char *expect)
{
        amx_base64_result result = amx_base64_encode(input);
        assert(!strncmp(result.dat, expect, strlen(expect)));
        free(result.dat);
}

static void enc_go_by_example()
{
        // from https://gobyexample.com/base64-encoding
        test("abc123!?$*&()'-=@~", "YWJjMTIzIT8kKiYoKSctPUB+");
}

static void enc_tutorials_point()
{
        // from https://www.tutorialspoint.com/java8/java8_base64.htm
        test("TutorialsPoint?java8", "VHV0b3JpYWxzUG9pbnQ/amF2YTg=");
}

static void enc_wikipedia()
{
        // from Wikipedia page
        test(wiki_src, wiki_enc);
}

static void enc_ipsum()
{
        amx_base64_result result = amx_base64_encode(ipsum4096);
        assert(!strncmp(result.dat, ipsum4096enc, strlen(ipsum4096enc) - 1));
        free(result.dat);
}

static void dec_go_by_example()
{
        // from https://gobyexample.com/base64-encoding
}

static void dec_tutorials_point()
{
        // from https://www.tutorialspoint.com/java8/java8_base64.htm
}

static void dec_wikipedia()
{
        // from Wikipedia page
}

static void dec_ipsum() {}

int test_enc()
{
        enc_go_by_example();
        enc_tutorials_point();
        enc_wikipedia();
        enc_ipsum();
        return 0;
}

int test_dec()
{
        dec_go_by_example();
        dec_tutorials_point();
        dec_wikipedia();
        dec_ipsum();
        return 0;
}

int main()
{
        test_enc();
        test_dec();
}
