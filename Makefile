BUILD_DIR := build
RM := /usr/bin/rm
TAG := $(shell git describe --tags --abbrev=0)

release:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: release

host-optimized:
	CXXFLAGS="${CXXFLAGS} -march=native" cmake -DLTO=ON -G Ninja -B build
	cmake --build $(BUILD_DIR)
.PHONY: optimized

debug:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: debug

dev: debug
.PHONY: dev

runner:
	cd ./scripts/runners/ && ./start.sh
.PHONY:
	runner

format:
	@echo -e 'vicinae\nwlr-clip' | xargs -I{} find {} -type d -iname 'build' -prune -o -type f -iname '*.hpp' -o -type f -iname '*.cpp' | xargs -I{} bash -c '[ -f {} ] && clang-format -i {} && echo "Formatted {}" || echo "Failed to format {}"'
.PHONY: format

# if we need to manually create a release
gh-release:
	mkdir -p dist
	cmake -G Ninja -DCMAKE_INSTALL_PREFIX=dist -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	cmake --install build
	tar -czvf vicinae-linux-x86_64-$(TAG).tar.gz -C dist .
.PHONY: gh-release

# we run this from time to time only, it's not part of the build pipeline
gen-contrib:
	node ./scripts/gen-contrib.js
.PHONY: gen-contrib

# we run this from time to time only, it's not part of the build pipeline
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

re: clean release
.PHONY: re

redev: clean dev
.PHONY: redev
