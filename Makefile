.PHONY: release debug test clean

release:
	@mkdir -p build/release
	@cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Debug > /dev/null
	@printf "→ Building @ %s\n" build/release
	@ninja -j10 -C build/release -d keepdepfile

debug:
	@mkdir -p build/debug
	@cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug > /dev/null
	@printf "→ Building @ %s\n" build/debug
	@ninja -j10 -C build/debug -d keepdepfile

test: debug
	@printf "→ Running tests\n"
	build/debug/amx_base64_tests

clean:
	@rm -rf build
