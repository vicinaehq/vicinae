#! /bin/sh

TARGET=extension-manager/src/protocols

cd .. && mkdir -p $TARGET && protoc --plugin=api/node_modules/.bin/protoc-gen-ts_proto --proto_path=omnicast/protocols/extension/ extension.proto preference.proto --ts_proto_out ${TARGET}
