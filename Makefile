BUILD_DIR := build
RM := rm
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
	cmake -G Ninja -DLTO=OFF -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: debug

strip:
	strip -s ./build/vicinae/vicinae
.PHONY: strip

test:
	cmake -G Ninja -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	./build/tests/all_tests
.PHONY: test

no-ts-ext:
	cmake -G Ninja -DTYPESCRIPT_EXTENSIONS=OFF -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: no-ts-ext

static:
	cmake -G Ninja -DPREFER_STATIC_LIBS=ON -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: static

# optimize for portability (build problematic libs statically)
# this will increase compile time as more libraries will have to be compiled from source,
# but the resulting binary will be more portable across different distros, especially the ones
# shipping older packages.
portable:
	cmake -G Ninja -DUSE_SYSTEM_RAPIDFUZZ=OFF -DUSE_SYSTEM_PROTOBUF=OFF -DUSE_SYSTEM_ABSEIL=OFF -DUSE_SYSTEM_CMARK_GFM=OFF -DUSE_SYSTEM_MINIZIP=OFF -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: portable

appimage:
	cmake -G Ninja -DCMAKE_INSTALL_PREFIX=./build/install -DVICINAE_PROVENANCE=appimage -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	cmake --install $(BUILD_DIR)
	./scripts/mkappimage.sh ./build/install AppDir
.PHONY: appimage

dev: debug
.PHONY: dev

extdev: debug
	./scripts/vicinae-ext-dev.sh
.PHONY: extdev

runner:
	cd ./scripts/runners/ && ./start.sh
.PHONY:
	runner

format:
	@echo -e 'vicinae\nwlr-clip' | xargs -I{} find {} -type d -iname 'build' -prune -o -type f -iname '*.hpp' -o -type f -iname '*.cpp' | xargs -I{} bash -c '[ -f {} ] && clang-format -i {} && echo "Formatted {}" || echo "Failed to format {}"'
.PHONY: format

bump-patch:
	./scripts/bump_version.sh patch
	make update-manifest
.PHONY: bump-patch

bump-minor:
	./scripts/bump_version.sh minor
	make update-manifest
.PHONY: bump-minor

update-manifest:
	./scripts/update-manifest.sh ./manifest.yaml
.PHONY: update-manifest

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
	$(RM) -rf ./typescript/api/node_modules
	$(RM) -rf ./typescript/api/dist
	$(RM) -rf ./typescript/api/src/proto
	$(RM) -rf ./typescript/extension-manager/dist/
	$(RM) -rf ./typescript/extension-manager/node_modules
	$(RM) -rf ./typescript/extension-manager/src/proto
	$(RM) -rf ./scripts/.tmp
.PHONY: clean

re: clean release
.PHONY: re

redev: clean dev
.PHONY: redev
