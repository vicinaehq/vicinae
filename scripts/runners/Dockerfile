FROM ubuntu:22.04

ARG QT_VERSION=6.5.3

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && apt-get -y upgrade && \
    apt-get -y install \
    build-essential \
    ninja-build \
    cmake \
    git \
    perl \
    python3 \
    libwayland-dev \
    libssl-dev \
    libgl1-mesa-dev \
    '^libxcb.*-dev' \
    libx11-xcb-dev \
    libglu1-mesa-dev \
    libxrender-dev \
    libxi-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    libx11-dev \
    libxext-dev \
    libxfixes-dev \
    libxi-dev \
    libxrender-dev \
    libxcb1-dev \
    libxcb-cursor-dev \
    libxcb-glx0-dev \
    libxcb-keysyms1-dev \
    libxcb-image0-dev \
    libxcb-shm0-dev \
    libxcb-icccm4-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-shape0-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    pkg-config && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir -p /opt/qt

RUN git clone --branch v${QT_VERSION} https://code.qt.io/qt/qt5.git qt6

WORKDIR /qt6

RUN perl init-repository --module-subset=qtbase,qtsvg,qtwayland

RUN ./configure \
    -release \
    -ltcg \
    -reduce-exports \
    -prefix /opt/qt \
    -xcb \
    -feature-wayland-client \
    -no-feature-wayland-server \
    -feature-sql \
    -feature-sql-sqlite \
    -no-sql-mysql \
    -no-sql-psql \
    -no-sql-odbc \
    -skip qtdeclarative \
    -skip qtlanguageserver \
    -- \
    -DBUILD_qtdeclarative=OFF

RUN cmake --build . --parallel $(nproc)
RUN cmake --install .

# Install C++20 capable compiler
RUN apt-get update && apt-get -y install software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt update -y && apt-get install -y gcc-13 g++-13

# vicinae deps
RUN apt-get install	-y wget libcmark-gfm-dev	\
	qtkeychain-qt6-dev	\
	libqalculate-dev	\
	libminizip-dev

# install node 22

RUN wget https://nodejs.org/dist/v22.19.0/node-v22.19.0-linux-x64.tar.xz
RUN mkdir -p /opt/node && tar -xf node-v22.19.0-linux-x64.tar.xz --strip-components=1 -C /opt/node

# install linuxdeployqt (tool to create appimage from qt app)

RUN apt-get install -y libfuse2
RUN wget -O linuxdeployqt https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
RUN chmod +x linuxdeployqt && cp linuxdeployqt /usr/local/bin/linuxdeployqt

ENV CC=gcc-13
ENV CXX=g++-13

# Set environment variables for Qt
ENV PATH="/opt/qt/bin:/opt/node/bin:${PATH}"
ENV QT_PLUGIN_PATH="/opt/qt/plugins"

WORKDIR /work

CMD ["bash"]
