#! /bin/sh

cd .. && protoc --plugin=api/node_modules/.bin/protoc-gen-ts_proto omnicast/protocols/extension/extension.proto --ts_proto_out api/src
