FROM archlinux:latest

# Install dependencies
RUN pacman -Syu --needed --noconfirm \
    curl \
    jq \
    git \
    sudo \
    docker \
    base-devel \
    cmake \
    ninja \
    nodejs \
    npm \
    qt6-base \
    qt6-svg \
    protobuf \
    cmark-gfm \
    layer-shell-qt \
    dotnet-runtime \
    dotnet-sdk \
    aspnet-runtime \
    libqalculate \
    minizip \
    gcc		\
    qtkeychain-qt6	\
    rapidfuzz-cpp	\
    mold		\
    ccache		\
    extra-cmake-modules	\
    wayland-protocols	\
	clang				\
    catch2

WORKDIR /home/runner

# Download and install GitHub Actions runner
RUN curl -o actions-runner-linux-x64.tar.gz -L \
    "https://github.com/actions/runner/releases/download/v2.327.1/actions-runner-linux-x64-2.327.1.tar.gz" && \
    tar xzf actions-runner-linux-x64.tar.gz && \
    rm actions-runner-linux-x64.tar.gz

# Copy startup script
COPY start.sh /start.sh
RUN chmod +x /start.sh

ENTRYPOINT ["/start.sh"]
