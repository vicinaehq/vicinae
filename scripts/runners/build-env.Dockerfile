# This image is used as a build environment for the main vicinae CI
# We start from a relatively old base (previous ubuntu LTS) so that we can keep the resulting
# binary or app image compatible with older glibc versions.
# We compile our own version of QT because Vicinae makes usage of newer QT features.
# Note that we don't link to QT statically, the main goal of this is so that the AppImage can be bundled with these QT
# shared libraries.
# Users that want to run the raw binary produced from the CI will need QT >= QT_VERSION installed on their system.

# Note: In order for linuxqtdeploy to work, the docker environment needs to be passed the fuse device.
# Typically done like so: docker run --cap-add SYS_ADMIN --device /dev/fuse <image_name>

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update &&	\
	apt-get -y upgrade && 	\
    apt-get -y install 		\
    build-essential			\
    ninja-build				\
    cmake					\
    git						\
    perl					\
    python3					\
    libwayland-dev			\
    libssl-dev				\
    libgl1-mesa-dev			\
    '^libxcb.*-dev'			\
    libx11-xcb-dev			\
    libglu1-mesa-dev		\
    libxrender-dev			\
    libxi-dev				\
    libxkbcommon-dev		\
    libxkbcommon-x11-dev	\
    libfontconfig1-dev		\
    libfreetype6-dev		\
    libx11-dev				\
    libxext-dev				\
    libxfixes-dev			\
    libxi-dev				\
    libxrender-dev			\
    libxcb1-dev				\
    libxcb-cursor-dev		\
    libxcb-glx0-dev			\
    libxcb-keysyms1-dev		\
    libxcb-image0-dev		\
    libxcb-shm0-dev			\
    libxcb-icccm4-dev		\
    libxcb-sync-dev			\
    libxcb-xfixes0-dev		\
    libxcb-shape0-dev		\
    libxcb-randr0-dev		\
    libxcb-render-util0-dev \
    libxcb-xinerama0-dev	\
    libxcb-xkb-dev			\
    pkg-config

ARG QT_VERSION=6.8.3
ARG INSTALL_DIR=/usr/local
ARG NODE_VERSION=22.19.0

RUN git clone --branch v${QT_VERSION} https://code.qt.io/qt/qt5.git qt6

WORKDIR /qt6

RUN perl init-repository --module-subset=qtbase,qtsvg,qtwayland

RUN ./configure					\
    -release					\
    -ltcg						\
    -reduce-exports				\
    -prefix ${INSTALL_DIR}		\
    -xcb						\
    -feature-wayland-client		\
    -no-feature-wayland-server	\
    -feature-sql				\
    -feature-sql-sqlite			\
    -no-sql-mysql				\
    -no-sql-psql				\
    -no-sql-odbc				\
    -skip qtdeclarative			\
    -skip qtlanguageserver		\
    --							\
    -DBUILD_qtdeclarative=OFF

RUN cmake --build . --parallel $(nproc)
RUN cmake --install .

WORKDIR /work

RUN rm -rf /qt6

# Install C++20 capable compiler
RUN apt-get -y install software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update -y && apt-get install -y gcc-13 g++-13

# extra vicinae deps
RUN apt-get install	-y	\
	curl				\
	wget				\
	libcmark-gfm-dev	\
	qtkeychain-qt6-dev	\
	libqalculate-dev	\
	libminizip-dev		\
	liblayershellqtinterface-dev

# install node 22 (used to build the main vicinae binary and bundled in the app image)
RUN wget https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz
RUN tar -xf node-v22.19.0-linux-x64.tar.xz --strip-components=1 -C ${INSTALL_DIR}

# install linuxdeployqt (tool to create appimage from qt app)

RUN apt-get install -y libfuse2 file
RUN wget -O linuxdeployqt https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
RUN chmod +x linuxdeployqt && mv linuxdeployqt /usr/local/bin/linuxdeployqt

ENV CC=gcc-13
ENV CXX=g++-13

WORKDIR /work

ENTRYPOINT ["/bin/bash"]
