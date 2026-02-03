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
ARG QT_VERSION=6.10.1
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

RUN apt-get install -y python3-pip libxml2-dev
RUN pip install meson

# extra vicinae deps
RUN git clone https://gitlab.freedesktop.org/wayland/wayland && cd wayland && meson build/ -Ddocumentation=false --prefix=/usr/local && ninja -C build/ install

RUN git clone https://gitlab.freedesktop.org/wayland/wayland-protocols && cd wayland-protocols && meson build/ --prefix=/usr/local && ninja -C build/ install

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

# compile modern libqalculate version, system one would be too old for us

RUN apt-get install -y	\
	autotools-dev		\
	autoconf			\
	libtool				\
	autopoint			\
	libcurl4-openssl-dev	\
	libmpfr-dev			\
	gettext				\
	libxml2-dev

RUN git clone https://github.com/Qalculate/libqalculate --branch v5.9.0
RUN cd libqalculate && ./autogen.sh && ./configure --disable-static --enable-compiled-definitions && make -j$(nproc) && make install

RUN git clone https://github.com/fcitx/xcb-imdkit.git && cd xcb-imdkit && cmake . && cmake --build . && cmake --install .

RUN apt-get install -y libdbus-1-dev libuv1-dev libcairo2-dev libxkbfile-dev iso-codes nlohmann-json3-dev libpango1.0-dev libgdk-pixbuf-2.0-dev

RUN git clone https://github.com/fcitx/fcitx5 && cd fcitx5 && git checkout 4c7e571a84908839af13e566bd2a8df36ab480b6 && cmake \
	-DENABLE_WAYLAND=ON . \
	-DEVENT_LOOP_BACKEND=none	\
	-DENABLE_SERVER=OFF			\
	-DENABLE_TEST=OFF			\
	-DENABLE_TESTING_ADDONS=OFF	\
	-DENABLE_ENCHANT=OFF		\
	-DBUILD_SPELL_DICT=OFF		\
	-DENABLE_XDGAUTOSTART=OFF	\
	&& cmake --build . --parallel $(nproc) && cmake --install . 

RUN git clone --recursive https://github.com/fcitx/fcitx5-qt
RUN cd fcitx5-qt && mkdir build && \
	/usr/bin/cmake -B build \
	-DCMAKE_POLICY_VERSION_MINIMUM=3.5	\
	-DBUILD_ONLY_PLUGIN=ON 	\
	-DENABLE_QT4=OFF 	\
	-DENABLE_QT5=OFF	\
	-DENABLE_QT6=ON 	&& \
	/usr/bin/cmake --build build --parallel $(nproc) && \
	/usr/bin/cmake --install build

# install node 22 (used to build the main vicinae binary and bundled in the app image)
RUN wget https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz
RUN mkdir /opt/node && tar -xf node-v${NODE_VERSION}-linux-x64.tar.xz --strip-components=1 -C /opt/node && rm -rf *.tar.xz

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
        wayland-protocols \
        fuse \
        file \
        ca-certificates \
		curl				\
		wget				\
		qtkeychain-qt6-dev	\
		libminizip-dev		\
		squashfs-tools		\
		ccache				\
		wayland-protocols	\
		libicu-dev			\
		libxml2-dev			\
		libgmp3-dev			\
		libmpfr-dev			\
		vim					\
		libudev-dev			\
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY --from=deps-builder /opt/gcc /opt/gcc
COPY --from=deps-builder /usr/local /usr/local
COPY --from=deps-builder /opt/node /opt/node

ENV LINUXDEPLOY_APPIMAGE_URL "https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20251107-1/linuxdeploy-x86_64.AppImage"
ENV LINUXDEPLOY_PLUGIN_QT_APPIMAGE_URL "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/1-alpha-20250213-1/linuxdeploy-plugin-qt-x86_64.AppImage"

RUN wget -O /usr/local/bin/linuxdeploy "${LINUXDEPLOY_APPIMAGE_URL}" && chmod +x /usr/local/bin/linuxdeploy
RUN wget -O /usr/local/bin/linuxdeploy-plugin-qt "${LINUXDEPLOY_PLUGIN_QT_APPIMAGE_URL}" && chmod +x /usr/local/bin/linuxdeploy-plugin-qt

ENV PATH="/opt/gcc/bin:/opt/node/bin:${PATH}"
ENV LD_LIBRARY_PATH="/opt/gcc/lib64:/usr/local/lib:/usr/local/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}"
ENV CC=/opt/gcc/bin/gcc
ENV CXX=/opt/gcc/bin/g++

RUN git config --global --add safe.directory /work

WORKDIR /work

ENTRYPOINT ["/bin/bash"]
