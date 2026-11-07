FROM archlinux:latest

RUN pacman -Syu --needed --noconfirm \
    curl jq git base-devel gcc clang cmake ninja mold ccache \
    nodejs npm \
    qt6-base qt6-svg qt6-declarative qt6-shadertools qt6-tools qt6-multimedia qt6-multimedia-ffmpeg \
    qtkeychain-qt6 layer-shell-qt syntax-highlighting extra-cmake-modules \
    libqalculate \
    catch2 wayland-protocols \
    shaderc vulkan-headers vulkan-icd-loader spirv-headers \
  && pacman -Scc --noconfirm

# Prebuilt so CI doesn't compile ggml's Vulkan shaders on every run. Same version as the one fetched
# in src/server/CMakeLists.txt. The AUR package is too old and comes without parakeet.
RUN git clone --depth 1 --branch v1.9.3 https://github.com/ggml-org/whisper.cpp /tmp/whisper \
  && cmake -S /tmp/whisper -B /tmp/whisper/build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DGGML_NATIVE=OFF \
    -DGGML_VULKAN=ON \
    -DWHISPER_BUILD_TESTS=OFF \
    -DWHISPER_BUILD_EXAMPLES=OFF \
  && cmake --build /tmp/whisper/build --parallel $(nproc) \
  && cmake --install /tmp/whisper/build \
  && rm -rf /tmp/whisper
