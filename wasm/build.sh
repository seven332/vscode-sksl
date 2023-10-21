#!/usr/bin/env bash

set -eu

SOURCE_DIR=$(realpath $(dirname "$0"))
DOWNLOAD_DIR=${SOURCE_DIR}/download
BUILD_DIR=${SOURCE_DIR}/build

# download cmake

if [ $(uname) == "Linux" ] && [ $(uname -m) == "x86_64" ]; then
  CMAKE_TAR_URL="https://github.com/Kitware/CMake/releases/download/v3.27.7/cmake-3.27.7-linux-x86_64.tar.gz"
  CMAKE_TAR_FILE="${DOWNLOAD_DIR}/cmake-3.27.7-linux-x86_64.tar.gz"
  CMAKE_DIR="${DOWNLOAD_DIR}/cmake-3.27.7-linux-x86_64"
  CMAKE_FILE="${CMAKE_DIR}/bin/cmake"

  mkdir -p ${DOWNLOAD_DIR}

  if [ ! -f ${CMAKE_TAR_FILE} ]; then
    curl ${CMAKE_TAR_URL} -L --output ${CMAKE_TAR_FILE}
  fi

  if [ ! -d ${CMAKE_DIR} ]; then
    tar xf ${CMAKE_TAR_FILE} -C ${DOWNLOAD_DIR}
  fi
else
  CMAKE_FILE="cmake"
fi

# init emsdk

EMSDK_DIR="${SOURCE_DIR}/third_party/externals/emsdk"
EMCMAKE_FILE="${EMSDK_DIR}/upstream/emscripten/emcmake"

"${EMSDK_DIR}/emsdk" install 3.1.47

cat <<EOT >"${EMSDK_DIR}/upstream/emscripten/.emscripten"
LLVM_ROOT = '${EMSDK_DIR}/upstream/bin'
BINARYEN_ROOT = '${EMSDK_DIR}/upstream'
NODE_JS = '${EMSDK_DIR}/node/16.20.0_64bit/bin/node'
EOT

# build

${EMCMAKE_FILE} ${CMAKE_FILE} -DCMAKE_BUILD_TYPE=Release -G Ninja -S ${SOURCE_DIR} -B ${BUILD_DIR}
${CMAKE_FILE} --build ${BUILD_DIR} --target sksl-wasm
