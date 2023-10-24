#!/usr/bin/env bash

set -eu

SOURCE_DIR=$(realpath $(dirname "$0"))
BUILD_DIR=${SOURCE_DIR}/build

emcmake cmake -DCMAKE_BUILD_TYPE=Release -S ${SOURCE_DIR} -B ${BUILD_DIR}

cmake --build ${BUILD_DIR} -j --target sksl-wasm
