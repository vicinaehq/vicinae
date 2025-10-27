# TEMP NOTES

sudo setcap cap_dac_read_search,cap_sys_admin=+ep ./vicinae

## TODO

fs:
error when manually triggering a database rebuild
Since it marks the filesystem, ensure that it only uses mark_filesystem_mount when the folder is a mount point?
note/warning about bftrs
Rebuild is likely using /mnt/root for bftrs, while the monitoring will use /
Add ignored paths so that I can ignore /mnt/root when monitoring
