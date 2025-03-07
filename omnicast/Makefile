BUILD_DIR := build

all: configure
	cmake --build $(BUILD_DIR) -j$(shell nproc)
.PHONY: all

configure:
	cmake -B $(BUILD_DIR)
.PHONY: configure

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

re: clean all
.PHONY: re
