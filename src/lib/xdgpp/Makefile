BUILD_DIR := build

all: test

static:
	cmake -GNinja -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: static

shared:
	cmake -GNinja -DBUILD_SHARED_LIBS=ON -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: shared

test:
	cmake -GNinja -DXDGPP_BUILD_TESTS=ON -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	./$(BUILD_DIR)/xdgpp-tests
.PHONY: test

install:
	cmake --install $(BUILD_DIR)
.PHONY: test

format:
	@echo -e 'src' | xargs -I{} find {} -type d -iname 'build' -prune -o -type f -iname '*.hpp' -o -type f -iname '*.cpp' | xargs -I{} bash -c '[ -f {} ] && clang-format -i {} && echo "Formatted {}" || echo "Failed to format {}"'
.PHONY: format

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

re: clean all
.PHONY: re

