BUILD_DIR := build

omnicast: configure
	cmake --build $(BUILD_DIR) -j$(shell nproc)
.PHONY: omnicast

extension-manager:
	cd extension-manager && npm run build && npm run install
.PHONY: extension-manager

wayland:
	wayland-scanner client-header ./wlr-clipman/protocols/wlr-data-control-unstable-v1.xml wlr-clipman/include/wayland-wlr-data-control-client-protocol.h
	wayland-scanner public-code ./wlr-clipman/protocols/wlr-data-control-unstable-v1.xml wlr-clipman/src/wayland-wlr-data-control-client-protocol.c

all: omnicast extension-manager
	cmake --build $(BUILD_DIR) -j$(shell nproc)
.PHONY: all

format:
	@echo 'omnicast\nwlr-clip\nproto\nomnictl' | xargs -I{} find {} -type d -iname 'build' -prune -o -type f -iname '*.hpp' -o -type f -iname '*.cpp' | xargs -I{} bash -c '[ -f {} ] && clang-format -i {} && echo "Formatted {}" || echo "Failed to format {}"'
.PHONY: format

configure:
	PARALLEL_LEVEL=$(shell nproc) cmake -B $(BUILD_DIR)
.PHONY: configure

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

re: clean all
.PHONY: re
