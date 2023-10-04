#!/usr/bin/env bash

set -eu

SOURCE_DIR=$(realpath $(dirname "$0"))
BUILD_DIR=${SOURCE_DIR}/build-test

cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja -S ${SOURCE_DIR} -B ${BUILD_DIR}

cmake --build ${BUILD_DIR} --target sksl-wasi-lib-test

${BUILD_DIR}/src/sksl-wasi-lib-test
