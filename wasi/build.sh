#!/usr/bin/env bash

set -eu

SOURCE_DIR=$(realpath $(dirname "$0"))
DOWNLOAD_DIR=${SOURCE_DIR}/download
BUILD_DIR=${SOURCE_DIR}/build
SDK_FILE=${DOWNLOAD_DIR}/wasi-sdk-20.0-linux.tar.gz
SDK_DIR=${DOWNLOAD_DIR}/wasi-sdk-20.0
SDK_URL="https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz"
WASI_SDK_PREFIX=${SDK_DIR}
CMAKE_TOOLCHAIN_FILE=${SDK_DIR}/share/cmake/wasi-sdk.cmake

mkdir -p ${DOWNLOAD_DIR}

if [ ! -f ${SDK_FILE} ]; then
    curl ${SDK_URL} -L --output ${SDK_FILE}
fi

if [ ! -d ${SDK_DIR} ]; then
    tar xf ${SDK_FILE} -C ${DOWNLOAD_DIR}
fi

cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -S ${SOURCE_DIR} -B ${BUILD_DIR} \
    -DWASI_SDK_PREFIX=${WASI_SDK_PREFIX} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}

cmake --build ${BUILD_DIR} --target sksl-wasi

cp ${BUILD_DIR}/src/sksl-wasi ${SOURCE_DIR}/../build
