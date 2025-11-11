# amx_base64.c

For Apple AMX

Ref: Daniel Lemiere (<https://github.com/lemire/fastbase64>)

Ref: Peter Cawley (<https://github.com/corsix/amx/>)

Research task for educational purposes only. Not recommended for production.

Well, I'm no match for the legendary Lemiere, however this is enough proof for me to explore some more suitable use cases such as games. :)

Tried to do a Hi|Lo byte split but wasn't smart enough to pull it off :(. In contrast the provided lane widths (16/32/64) with `genlut` is quite simple to use, and much faster when use cases directly require these widths.

I will complete the rest later w' proper benchmarking code and maybe revisit the split.

- [x] Encode
- [ ] Decode
- [ ] Hi | Lo split shuffle?

#### amx_base64_encode

> @ 1,000,000 iterations w' 1 heap allocation

| Threads     | Length | Input    | Output   |
| ----------- | ------ | -------- | -------- |
| ? (iPhone)  | 4096   | ?        | ?        |
| 1 (iMac)    | 4096   | ~7.5GB/s | ~10GB/s  |
| Theoretical |        |          |          |
| 2? (Max)    | 4096   | ~15GB/s? | ~20GB/s? |
| 4? (Ultra)  | 4096   | ~30GB/s? | ~40GB/s? |

#### amx_base64_decode

| Threads | Length | Input | Output |
| ------- | ------ | ----- | ------ |
| 1       | 4096   | ?     | ?      |

#### Special Thanks

Ben Sims (<https://github.com/arycama>)
