flatpak run \
  --socket=wayland \
  --socket=x11 \
  --filesystem=host \
  --command=bash	\
  com.yourcompany.vicinae $@
