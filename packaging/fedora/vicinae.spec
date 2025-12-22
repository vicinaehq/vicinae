Name:           vicinae
Version:        0.17.1
Release:        1%{?dist}
Summary:        High-performance native launcher for your desktop

License:        MIT
URL:            https://github.com/vicinaehq/vicinae
Source0:        %{url}/archive/refs/tags/v%{version}.tar.gz
Source1:        https://github.com/github/cmark-gfm/archive/refs/tags/0.29.0.gfm.13.tar.gz
Patch0:         cmark-gfm-no-git.patch

BuildRequires:  cmake
BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  pkgconf-pkg-config
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qttools-devel
BuildRequires:  qt6-qtsvg-devel
BuildRequires:  qt6-qtwayland-devel
BuildRequires:  openssl-devel
BuildRequires:  protobuf-devel
BuildRequires:  minizip-compat-devel
BuildRequires:  cmark-gfm-devel
BuildRequires:  libqalculate-devel
BuildRequires:  abseil-cpp-devel
BuildRequires:  qtkeychain-qt6-devel
BuildRequires:  wayland-devel
BuildRequires:  layer-shell-qt-devel
BuildRequires:  nodejs
BuildRequires:  npm
BuildRequires:  extra-cmake-modules
BuildRequires:  git

Requires:       qt6-qtbase
Requires:       qt6-qtsvg
Requires:       qt6-qtwayland
Requires:       openssl
Requires:       protobuf
Requires:       cmark-gfm
Requires:       qtkeychain-qt6

%description
Vicinae is a high-performance, native launcher for your desktop, built with C++ and Qt.
It provides fast, keyboard-first access to apps, files, and extensions, and supports
TypeScript/React-based extensions.

%prep
%autosetup -n %{name}-%{version} -p1
# Make the cmark-gfm source archive available in the source tree for CMake
cp %{SOURCE1} cmark-gfm-0.29.0.gfm.13.tar.gz

%build
# Set BUILD_SHARED_LIBS=OFF to prevent xdgpp from building as shared library
# (xdgpp installation is disabled, so shared library would create missing runtime dependency)
# Using :BOOL type to force the cache variable to override any default from %cmake macro
%cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS:BOOL=OFF \
  -DTYPESCRIPT_EXTENSIONS=ON \
  -DINSTALL_NODE_MODULES=ON \
  -DWAYLAND_LAYER_SHELL=ON \
  -DUSE_SYSTEM_CMARK_GFM=ON
%cmake_build

%install
%cmake_install
# Remove glaze headers and CMake files that shouldn't be packaged
# (glaze is a build-time only dependency, headers shouldn't be installed)
rm -rf %{buildroot}%{_includedir}/glaze
rm -rf %{buildroot}%{_datadir}/glaze

%files
%license LICENSE
%doc README.md
%{_bindir}/vicinae
%{_datadir}/applications/vicinae.desktop
%{_datadir}/applications/vicinae-url-handler.desktop
%{_datadir}/icons/hicolor/512x512/apps/vicinae.png
%{_prefix}/lib/systemd/user/vicinae.service
%{_datadir}/vicinae/themes

%changelog
* Sun Dec 22 2025 dacrab <dacrab@example.com> - 0.17.1-1
- Update to version 0.17.1
- Fix xdgpp dependency issue by forcing BUILD_SHARED_LIBS=OFF (prevents missing libxdgpp.so runtime dependency)
- Fix CMake generator conflict by removing -G Ninja (use default Unix Makefiles)
- Remove glaze headers/CMake files from package (build-time only dependency)
- Enable WAYLAND_LAYER_SHELL support
* Sun Nov 16 2025 You <you@example.com> - 0.16.6-1
- Initial COPR packaging for vicinae
