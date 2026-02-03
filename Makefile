BUILD_DIR						:= build
BIN_DIR							:= build/bin
RM								:= rm
TAG 							:= $(shell git describe --tags --abbrev=0)
APPIMAGE_BUILD_ENV_DIR			:= ./scripts/runners/appimage/
APPIMAGE_BUILD_ENV_IMAGE_TAG	:= vicinae/appimage-build-env

release:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: release

host-optimized:
	CXXFLAGS="${CXXFLAGS} -march=native" cmake -DLTO=ON -G Ninja -B build
	cmake --build $(BUILD_DIR)
.PHONY: optimized

preview:
	cmake -G Ninja -DENABLE_PREVIEW_FEATURES=ON -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: preview

debug:
	cmake -G Ninja -DLTO=OFF -DENABLE_PREVIEW_FEATURES=ON -DENABLE_SANITIZERS=ON -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: debug

genicon:
	node scripts/generate-icons.js
.PHONY: genicon

install:
	cmake --install $(BUILD_DIR)
.PHONY: install

strip:
	strip -s ./build/vicinae/vicinae
.PHONY: strip

test:
	./$(BIN_DIR)/vicinae-emoji-tests
	./$(BIN_DIR)/xdgpp-tests
	./$(BIN_DIR)/scriptcommand-tests
.PHONY: test

static:
	cmake -G Ninja -DPREFER_STATIC_LIBS=ON -DCMAKE_BUILD_TYPE=Release -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: static

# optimize for portability (build problematic libs statically)
# this will increase compile time as more libraries will have to be compiled from source,
# but the resulting binary will be more portable across different distros, especially the ones
# shipping older packages.
portable:
	cmake -G Ninja -DUSE_SYSTEM_PROTOBUF=OFF -DUSE_SYSTEM_ABSEIL=OFF -DUSE_SYSTEM_CMARK_GFM=OFF -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
.PHONY: portable

appimage:
	cmake -G Ninja -DCMAKE_INSTALL_PREFIX=./build/install -DVICINAE_PROVENANCE=appimage -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	cmake --install $(BUILD_DIR)
	./scripts/mkappimage.sh ./build/install AppDir
.PHONY: appimage

appimage-build-env-run:
	docker run -v$(PWD):/work --cap-add SYS_ADMIN --device /dev/fuse -it $(APPIMAGE_BUILD_ENV_IMAGE_TAG) 
.PHONY: appimage-dev

appimage-build-env:
	docker build -f $(APPIMAGE_BUILD_ENV_DIR)/AppImageBuilder.Dockerfile $(APPIMAGE_BUILD_ENV_DIR) -t $(APPIMAGE_BUILD_ENV_IMAGE_TAG)
.PHONY: appimage-build-env

appimage-build-gh-runner: appimage-build-env
	docker build -f $(APPIMAGE_BUILD_ENV_DIR)/gh-runner.Dockerfile $(APPIMAGE_BUILD_ENV_DIR) -t vicinae/appimage-gh-runner
.PHONY: appimage-build-gh-runner

appimage-build-env-push:
	docker push $(APPIMAGE_BUILD_ENV_IMAGE_TAG)
.PHONY: appimage-build-env-push

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

clean:
	rm -rf $(BUILD_DIR)
	$(RM) -rf ./src/typescript/api/node_modules
	$(RM) -rf ./src/typescript/api/dist
	$(RM) -rf ./src/typescript/api/src/proto
	$(RM) -rf ./src/typescript/extension-manager/dist/
	$(RM) -rf ./src/typescript/extension-manager/node_modules
	$(RM) -rf ./src/typescript/extension-manager/src/proto
	$(RM) -rf ./scripts/.tmp
	$(RM) -rf ./src/lib/*/build
.PHONY: clean

re: clean release
.PHONY: re

redev: clean dev
.PHONY: redev
