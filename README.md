# amx_base64.c

Ref: Daniel Lemiere (<https://github.com/lemire/fastbase64>)

Research task for educational purposes only. Not recommended for production.

Tried to do a Hi|Lo byte split but wasn't smart enough to pull it off :(. Using the provided lane widths (16/32/64) with `genlut` is quite simple to use however and still fast.

I will complete the rest later w' proper benchmarking code and maybe revisit the split.

- [x] Encode
- [ ] Decode
- [ ] Hi | Lo split shuffle?

loremipsum_4096.txt @ 1,000,000 iterations w' 1 heap allocation & 1 thread

| In      | Out    |
| ------- | ------ |
| ~7.5GBs | ~10GBs |
