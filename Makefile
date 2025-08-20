BUILD_DIR := build
RM := /usr/bin/rm
TAG := $(shell git describe --tags --abbrev=0)

vicinae: configure
	cmake --build $(BUILD_DIR)

.PHONY: vicinae

wayland:
	wayland-scanner client-header ./wlr-clipman/protocols/wlr-data-control-unstable-v1.xml wlr-clipman/include/wayland-wlr-data-control-client-protocol.h
	wayland-scanner public-code ./wlr-clipman/protocols/wlr-data-control-unstable-v1.xml wlr-clipman/src/wayland-wlr-data-control-client-protocol.c

all: vicinae
	cmake --build $(BUILD_DIR)
.PHONY: all

release:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: release

optimized:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DLTO=ON -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: optimized

debug:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: debug

runner:
	cd ./scripts/runners/ && ./start.sh
.PHONY:
	runner

format:
	@echo 'vicinae\nwlr-clip\nproto\nomnictl' | xargs -I{} find {} -type d -iname 'build' -prune -o -type f -iname '*.hpp' -o -type f -iname '*.cpp' | xargs -I{} bash -c '[ -f {} ] && clang-format -i {} && echo "Formatted {}" || echo "Failed to format {}"'
.PHONY: format

# if we need to manually create a release
gh-release:
	mkdir -p dist
	cmake -G Ninja -DCMAKE_INSTALL_PREFIX=dist -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	cmake --install build
	tar -czvf vicinae-linux-x86_64-$(TAG).tar.gz -C dist .
.PHONY: gh-release

configure:
	cmake -G Ninja -B $(BUILD_DIR)
.PHONY: configure

gen-emoji:
	cd ./scripts/emoji && npm install && tsc --outDir dist && node dist/main.js
.PHONY: gen-emoji

clean:
	rm -rf $(BUILD_DIR)
	$(RM) -rf ./api/node_modules
	$(RM) -rf ./api/dist
	$(RM) -rf ./extension-manager/dist/
	$(RM) -rf ./extension-manager/node_modules
	$(RM) -rf ./scripts/.tmp
.PHONY: clean

re: clean all
.PHONY: re
