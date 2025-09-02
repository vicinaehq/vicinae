#! /bin/sh
# Wrapper to launch Vicinae with INTERNAL extension dev mode stuff on. 
# Not sure how much we will use this, but it is there.
#
# Every time a change is made to the api or the extension the manager will be stopped
# and restarted automatically to use the new bundle.
#
# Note: for changes made to `.proto` files, recompilation is still required.

export PATH="./build/vicinae:${PATH}"
export EXT_RECONCILER_LOG_PASSTHROUGH=1
export EXT_MANAGER_PATH="${PWD}/extension-manager/dist/runtime.js"

./build/vicinae/vicinae server
