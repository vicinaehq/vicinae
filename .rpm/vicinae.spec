Name:           vicinae
Version:        0.21.2
Release:        1%{?dist}
Summary:        A focused launcher for your desktop
License:        GPL-3.0
URL:            https://github.com/vicinaehq/vicinae
Source0:        https://github.com/vicinaehq/vicinae/archive/refs/tags/v%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  ninja-build
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  qt6-qtsvg-devel
BuildRequires:  qt6-qtshadertools-devel
BuildRequires:  qt6-qtquickcontrols2-devel
BuildRequires:  kf6-syntax-highlighting-devel
BuildRequires:  wayland-devel
BuildRequires:  wayland-protocols-devel
BuildRequires:  libsecret-devel
BuildRequires:  nodejs
BuildRequires:  git

%description
Vicinae is a high-performance, native command palette for your desktop.
It can serve as app search, clipboard history, text expander, file search,
browser tab switcher, emoji picker, calculator, window switcher, and more.

%prep
%autosetup -n %{name}-v%{version}

%build
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DUSE_SYSTEM_KF6=ON \
    -DVICINAE_GIT_TAG=v%{version} \
    -DVICINAE_GIT_COMMIT_HASH=7745e8ed8
cmake --build build --parallel $(nproc)

%install
cmake --install build --prefix %{buildroot}%{_prefix}

%files
%license LICENSE
%{_bindir}/*
%{_datadir}/%{name}/
%{_datadir}/applications/
%{_datadir}/icons/

%changelog
* Thu May 28 2026 killcrb <killcrb@users.noreply.github.com> - 0.21.2-1
- Bump to v0.21.2
