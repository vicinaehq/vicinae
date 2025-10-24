// Stub workspace interface definitions
// These are required by Cosmic protocols but not yet implemented
#include <wayland-client.h>

const struct wl_interface zcosmic_workspace_handle_v1_interface = {
    .name = "zcosmic_workspace_handle_v1",
    .version = 1,
    .method_count = 0,
    .methods = NULL,
    .event_count = 0,
    .events = NULL,
};

const struct wl_interface ext_workspace_handle_v1_interface = {
    .name = "ext_workspace_handle_v1",
    .version = 1,
    .method_count = 0,
    .methods = NULL,
    .event_count = 0,
    .events = NULL,
};
