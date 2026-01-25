# You need to build AppImageBuilder.Dockerfile locally before you can build this:
# docker build -f AppImageBuilder.Dockerfile -t appimage-builder

FROM vicinae/appimage-build-env:latest

ARG RUNNER_VERSION=2.327.1
ENV REPO_URL=https://github.com/vicinaehq/vicinae

WORKDIR /home/runner

RUN curl -o actions-runner-linux-x64.tar.gz -L \
    "https://github.com/actions/runner/releases/download/v${RUNNER_VERSION}/actions-runner-linux-x64-${RUNNER_VERSION}.tar.gz" && \
    tar xzf actions-runner-linux-x64.tar.gz && \
    rm actions-runner-linux-x64.tar.gz

RUN ./bin/installdependencies.sh

COPY start.sh /start.sh
RUN chmod +x /start.sh

ENTRYPOINT ["/start.sh"]
