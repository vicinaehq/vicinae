# We compile our own version of QT because Vicinae makes usage of newer QT features.
# Note that we don't link to QT statically, the main goal of this is so that the AppImage can be bundled with these QT
# shared libraries.

# Note: In order for linuxqtdeploy to work, the docker environment needs to be passed the fuse device.
# Typically done like so: docker run --cap-add SYS_ADMIN --device /dev/fuse <image_name>

FROM ubuntu:22.04 as base-builder

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
    pkg-config				\
	wget

FROM base-builder AS gcc-builder

RUN wget 'https://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-15.2.0/gcc-15.2.0.tar.gz' \
    && tar xzf gcc-15.2.0.tar.gz \
    && rm gcc-15.2.0.tar.gz

WORKDIR /gcc-15.2.0
RUN ./contrib/download_prerequisites \
    && mkdir build \
    && cd build \
    && ../configure \
        --prefix=/opt/gcc \
        --disable-multilib \
        --enable-languages=c,c++ \
        --disable-bootstrap \
        --disable-libstdcxx-pch \
    && make -j$(nproc) \
    && make install \
    && cd / \
    && rm -rf /gcc-15.2.0


# Build QT
FROM gcc-builder AS qt-builder

ENV PATH="/opt/gcc/bin:${PATH}"
ENV LD_LIBRARY_PATH="/opt/gcc/lib64:${PATH}"
ENV CC=/opt/gcc/bin/gcc
ENV CXX=/opt/gcc/bin/g++
ARG QT_VERSION=6.8.3
ARG INSTALL_DIR=/usr/local

RUN git clone --branch v${QT_VERSION} https://code.qt.io/qt/qt5.git /qt6
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

RUN cmake --build . --parallel $(nproc) \
    && cmake --install . \
    && cd / \
    && rm -rf /qt6

# other dep builders
FROM qt-builder AS deps-builder
ARG NODE_VERSION=22.19.0

# extra vicinae deps
RUN apt-get install	-y	\
	wayland-protocols

RUN git clone --branch v6.18.0 https://github.com/KDE/extra-cmake-modules ecm &&	\
	cd ecm			\
	&& mkdir build	\
	&& cmake 		\
	-DBUILD_DOC=OFF	\
	-DBUILD_TESTING=OFF	\
	-B build			\
	&& cmake --build build	\
	&& cmake --install build	\
	&& rm -rf /ecm


# we install latest cmake cause we depend on some newer features
RUN git clone https://github.com/Kitware/CMake --branch v4.1.2 &&	\
	cd CMake &&									\
	mkdir build &&								\
	cmake										\
	-B build && 								\
	cmake --build build --parallel $(nproc)	&&	\
	cmake --install build &&					\
	rm -rf /CMake

RUN git clone https://github.com/vicinaehq/layer-shell-qt &&	\
	cd layer-shell-qt &&						\
	mkdir build &&								\
	cmake										\
	-DLAYER_SHELL_QT_DECLARATIVE=OFF			\
	-B build && 								\
	cmake --build build --parallel $(nproc) &&	\
	cmake --install build &&					\
	rm -rf /layer-shell-qt

RUN git clone https://github.com/protocolbuffers/protobuf.git --branch v32.0 &&	\
	cd protobuf &&																\
	mkdir build &&								\
	cmake										\
	-Dprotobuf_BUILD_TESTS=OFF					\
	-B build && 								\
	cmake --build build --parallel $(nproc) &&	\
	cmake --install build &&					\
	rm -rf /protobuf

RUN git clone https://github.com/github/cmark-gfm --branch 0.29.0.gfm.13 &&	\
	cd cmark-gfm &&															\
	mkdir build &&															\
	cmake																	\
	-DCMAKE_POLICY_VERSION_MINIMUM=3.5										\
	-DCMARK_TESTS=OFF														\
	-B build && 															\
	cmake --build build --parallel $(nproc) &&								\
	cmake --install build &&												\
	rm -rf /cmark-gfm

RUN git clone https://github.com/zlib-ng/minizip-ng --branch 4.0.10 &&	\
	cd minizip-ng &&													\
	mkdir build &&														\
	cmake																\
	-B build && 														\
	cmake --build build --parallel $(nproc) &&							\
	cmake --install build &&											\
	rm -rf /minizip-ng

RUN git clone https://github.com/rapidfuzz/rapidfuzz-cpp --branch v3.3.3 &&	\
    cd rapidfuzz-cpp &&														\
	mkdir build &&															\
	cmake																	\
	-B build && 															\
	cmake --build build --parallel $(nproc) &&								\
	cmake --install build &&												\
	rm -rf /rapidfuzz-cpp

# install node 22 (used to build the main vicinae binary and bundled in the app image)
RUN wget https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz
RUN mkdir /opt/node && tar -xf node-v${NODE_VERSION}-linux-x64.tar.xz --strip-components=1 -C /opt/node && rm -rf *.tar.xz

# install linuxdeployqt (tool to create appimage from qt app)

FROM ubuntu:22.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Install only runtime dependencies
RUN apt-get update \
    && apt-get install -y \
		make			\
		cmake			\
		ninja-build		\
		git				\
		zlib1g-dev		\
		libwayland-dev	\
        libgl1-mesa-glx \
    	libgl1-mesa-dev	\
        libglu1-mesa \
        libxrender1 \
        libxi6 \
		libxkbcommon-dev	\
        libxkbcommon0 \
        libxkbcommon-x11-0 \
        libfontconfig1 \
        libfreetype6 \
        libx11-6 \
        libxext6 \
        libxfixes3 \
        libxcb1 \
        libxcb-cursor0 \
        libxcb-glx0 \
        libxcb-keysyms1 \
        libxcb-image0 \
        libxcb-shm0 \
        libxcb-icccm4 \
        libxcb-sync1 \
        libxcb-xfixes0 \
        libxcb-shape0 \
        libxcb-randr0 \
        libxcb-render-util0 \
        libxcb-xinerama0 \
        libxcb-xkb1 \
        libwayland-client0 \
        libwayland-egl1 \
    	libssl-dev		\
        libcmark-gfm-dev \
        libqalculate-dev \
        libminizip1 \
        wayland-protocols \
        libfuse2 \
        file \
        ca-certificates \
		curl				\
		wget				\
		libcmark-gfm-dev	\
		qtkeychain-qt6-dev	\
		libqalculate-dev	\
		libminizip-dev		\
		squashfs-tools		\
		ccache				\
		wayland-protocols	\
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY --from=deps-builder /opt/gcc /opt/gcc
COPY --from=deps-builder /usr/local /usr/local
COPY --from=deps-builder /opt/node /opt/node

RUN wget -O linuxdeployqt https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
RUN chmod +x linuxdeployqt && mv linuxdeployqt /usr/local/bin/linuxdeployqt

ENV PATH="/opt/gcc/bin:/opt/node/bin:${PATH}"
ENV LD_LIBRARY_PATH="/opt/gcc/lib64:/usr/local/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}"
ENV CC=/opt/gcc/bin/gcc
ENV CXX=/opt/gcc/bin/g++

RUN git config --global --add safe.directory /work

WORKDIR /work

ENTRYPOINT ["/bin/bash"]

