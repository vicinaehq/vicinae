BUILD_DIR := build

vicinae: configure
	cmake --build $(BUILD_DIR)

.PHONY: vicinae

extension-manager:
	cd api && npm install && tsc --outDir dist
	cd extension-manager && npm run build
	cp extension-manager/dist/runtime.js vicinae/assets/extension-runtime.js

.PHONY: extension-manager

wayland:
	wayland-scanner client-header ./wlr-clipman/protocols/wlr-data-control-unstable-v1.xml wlr-clipman/include/wayland-wlr-data-control-client-protocol.h
	wayland-scanner public-code ./wlr-clipman/protocols/wlr-data-control-unstable-v1.xml wlr-clipman/src/wayland-wlr-data-control-client-protocol.c

all: vicinae extension-manager
	cmake --build $(BUILD_DIR)
.PHONY: all

format:
	@echo 'vicinae\nwlr-clip\nproto\nomnictl' | xargs -I{} find {} -type d -iname 'build' -prune -o -type f -iname '*.hpp' -o -type f -iname '*.cpp' | xargs -I{} bash -c '[ -f {} ] && clang-format -i {} && echo "Formatted {}" || echo "Failed to format {}"'
.PHONY: format

configure:
	cmake -G Ninja -B $(BUILD_DIR)
.PHONY: configure

gen-emoji:
	cd ./scripts/emoji && npm install && tsc --outDir dist && node dist/main.js
	cp ./scripts/emoji/dist/emoji.{cpp,hpp} vicinae/src/services/emoji-service/
.PHONY: gen-emoji

clean:
	rm -rf $(BUILD_DIR)
.PHONY: clean

re: clean all
.PHONY: re
