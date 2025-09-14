# Building the AppImage

Issuing the following command will start building the app image and output it at the root of vicinae source tree:

```bash
docker run -v/path/to/vicinae/source:/work --cap-add SYS_ADMIN --device /dev/fuse appimage-builder -c 'make clean && make appimage'
```

From there you should be able to execute the appimage on a variety of distributions. 
