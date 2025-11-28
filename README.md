# amx_base64.c

_For Apple AMX_

This software is available under the following license: [BSD 2-Clause](#license)

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

# License

BSD 2-Clause License

Copyright (c) 2025, DanielShuey

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
