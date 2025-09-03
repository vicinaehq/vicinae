#! /bin/sh
# To build a static version of QT that fits our needs.
# This is not part of the main build system mainly because of my cmake
# skill issues.
# We build it beforehand and then point cmake to the local QT installation

git clone --branch v6.9.2 git://code.qt.io/qt/qt5.git qt6 # lts release
cd qt6

rm -rf build install

mkdir build install

cd build

../configure \
	-init-submodules	\
    -release \
    -static \
    -ltcg \
    -reduce-exports \
    -gc-binaries \
	-qt-zlib	\
	-qt-libjpeg	\
	-qt-libpng	\
	-qt-pcre	\
    -prefix ../install \
    -submodules qtbase,qtsvg,qtwayland \
	-skip qtdeclarative,qtlanguageserver	\
    -feature-sql \
    -feature-sql-sqlite \
    -no-sql-mysql \
    -no-sql-psql \
    -no-sql-odbc	\
	-feature-wayland-client \
    -no-feature-wayland-server \
	--	\
	-DBUILD_qtdeclarative=OFF # we need this to force disable it, otherwise it's pulled by qtwayland (although it's an optional dep...)

cmake --build . --parallel
cmake --install .
