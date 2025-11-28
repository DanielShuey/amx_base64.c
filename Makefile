.PHONY: debug release test clean

FDEBUG=-std=c23 -Isrc -MJ build/compile_commands.tmp -Wno-everything
SRC=src/amxb64enc.c

debug:
	@mkdir -p build/debug
	@printf "→ Building @ %s\n" build/debug
	$(CC) src/amx_base64.c -c $(FDEBUG)
	@mv amx_base64.o build/debug
	@echo "[`sed 's/.$$//' build/compile_commands.tmp`]" > build/compile_commands.tmp
	@jq '(.[]["arguments"]) |= map(select(. != "-Xclang" and (contains("clang-vendor-feature") | not) and . != "-fno-odr-hash-protocols"))' build/compile_commands.tmp > build/compile_commands.json

# release:
# 	@mkdir -p build/release
# 	@printf "→ Building @ %s\n" build/release
# 	$(CC) ios_json.c -c -std=c23 -O3 -march=native -fblocks
# 	@mv ios_json.o build/release/ios_json.o

test:
	@mkdir -p build/test
	$(CC) src/amx_base64.c tests/*.c $(FDEBUG) -Itests
	@printf "→ Running tests\n"
	@mv a.out build/test/unit
	build/test/unit

clean:
	@rm -rf build
