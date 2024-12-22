BUILD_DIR := build

omnicast: configure
	cmake --build $(BUILD_DIR) -j$(shell nproc)
.PHONY: omnicast

extension-manager:
	cd extension-manager && node build.mjs && node install.mjs
.PHONY: extension-manager

all: omnicast extension-manager
	cmake --build $(BUILD_DIR) -j$(shell nproc)
.PHONY: all

configure:
	PARALLEL_LEVEL=$(shell nproc) cmake -B $(BUILD_DIR)
.PHONY: configure

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

re: clean all
.PHONY: re
