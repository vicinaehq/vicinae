FROM archlinux:latest

RUN pacman -Syu --needed --noconfirm \
    curl jq git base-devel gcc clang cmake ninja mold ccache \
    nodejs npm \
    qt6-base qt6-svg qt6-declarative qt6-shadertools qt6-tools \
    qtkeychain-qt6 layer-shell-qt syntax-highlighting extra-cmake-modules \
    libqalculate \
    catch2 wayland-protocols \
  && pacman -Scc --noconfirm
