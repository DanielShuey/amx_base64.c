# amx_base64.c

Ref: Daniel Lemiere (<https://github.com/lemire/fastbase64>)

Research task for educational purposes only. Not recommended for production.

Tried to do a Hi|Lo byte split but wasn't smart enough to pull it off :(. Using the provided lane widths (16/32/64) with `genlut` is quite simple to use however and still very fast.

I will complete the rest later w' proper benchmarking code and maybe revisit the split.

- [x] Encode
- [ ] Decode
- [ ] Hi | Lo split shuffle?

> 1,000,000 iterations w' 1 heap allocation

## `amx_base64_encode`

| Threads | Length | Input    | Output  |
| ------- | ------ | -------- | ------- |
| 1       | 4096   | ~7.5GB/s | ~10GB/s |

## `amx_base64_decode`

| Threads | Length | Input | Output |
| ------- | ------ | ----- | ------ |
| 1       | 4096   | ?     | ?      |
