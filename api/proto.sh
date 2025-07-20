#! /bin/sh

TARGET=api/src/proto
PROTO_DIR=../omnicast/proto/extensions

PROTO_FILES="$(ls -1 $PROTO_DIR | xargs -I{} echo {})"

cd .. && mkdir -p $TARGET && protoc --plugin=api/node_modules/.bin/protoc-gen-ts_proto --proto_path=omnicast/proto/extensions ${PROTO_FILES} --ts_proto_out ${TARGET}
