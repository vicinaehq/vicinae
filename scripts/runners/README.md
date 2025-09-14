# Runners

This directory contains Dockerfiles for:

- the reference vicinae build environment used for all CI work, from testing to release
- a github runner image built upon the vicinae build environment, allowing to deploy self-hosted runners that use the vicinae build env

For more information about the vicinae build environment read `build-env.Dockerfile`.

## Usage

Start by building the vicinae build environment image. Note that this will probably take a long time, as we compile QT in there.

```bash
docker build -f build-env.Dockerfile -t vicinae-build-env
```

### Compile vicinae inside the build environment

You can then mount the vicinae source tree inside the image:

```bash
docker run -v/path/to/vicinae/source:/work -it vicinae-build-env
```

And from the container shell, do whatever you want:

```bash
make
```

### Building the AppImage

Unlike the standalone vicinae binary, which can be built on a variety of systems, the app image is meant to be built from inside the vicinae build env and nowhere else.

Issuing the following command will start building the app image and output it at the root of vicinae source tree:

```bash
docker run -v/path/to/vicinae/source:/work --cap-add SYS_ADMIN --device /dev/fuse vicinae-build-env -c 'make clean && make appimage'
```

From there you should be able to execute the appimage on a variety of distributions. 
