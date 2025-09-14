FROM archlinux:latest

RUN useradd -m -G wheel dummy

RUN echo 'dummy ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers

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
    gcc14	\
    qtkeychain-qt6	\
    rapidfuzz-cpp

# Download and install GitHub Actions runner
WORKDIR /home/runner
RUN curl -o actions-runner-linux-x64.tar.gz -L \
    "https://github.com/actions/runner/releases/download/v2.327.1/actions-runner-linux-x64-2.327.1.tar.gz" && \
    tar xzf actions-runner-linux-x64.tar.gz && \
    rm actions-runner-linux-x64.tar.gz

# manual install on arch
#RUN ./bin/installdependencies.sh

# Copy startup script
COPY start.sh /start.sh
RUN chmod +x /start.sh

ENV CC=gcc-14
ENV CXX=g++-14

ENTRYPOINT ["/start.sh"]
