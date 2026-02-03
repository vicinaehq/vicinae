BUILD_DIR := build

all: test
.PHONY: all

test:
	cmake -GNinja -DBUILD_TESTS=ON -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	./$(BUILD_DIR)/emoji-tests
.PHONY: test

# we run this from time to time only, it's not part of the build pipeline
gen-db:
	mkdir -p {include,src}/generated
	cd ./scripts/emoji && npm install && tsc --outDir dist && node dist/main.js
	cp ./scripts/emoji/dist/emoji.hpp include/generated/db.hpp
	cp ./scripts/emoji/dist/emoji.cpp src/generated/db.cpp
.PHONY: gen-db

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

re: clean all
.PHONY: re

