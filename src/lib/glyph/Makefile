BUILD_DIR := build

all: test
.PHONY: all

test:
	cmake -GNinja -DBUILD_TESTS=ON -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	./$(BUILD_DIR)/vicinae-glyph-tests
.PHONY: test

# Regenerate the glyph tables from the latest Unicode / Emoji / CLDR data.
# Not part of the build pipeline: run it from time to time and commit the result.
# Pin releases with UNICODE_VERSION=17.0.0 / CLDR_REF=48.2.0 (see scripts/fetch.ts).
gen-db:
	node scripts/fetch.ts
	node scripts/gen.ts
	clang-format -i src/glyph.cpp src/emoji-properties.cpp include/glyph/glyph.hpp
.PHONY: gen-db

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

re: clean all
.PHONY: re

