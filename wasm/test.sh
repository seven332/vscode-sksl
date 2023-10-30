#!/usr/bin/env bash

set -eu

SOURCE_DIR=$(realpath $(dirname "$0"))
BUILD_DIR=${SOURCE_DIR}/build_test

cmake -DCMAKE_BUILD_TYPE=Debug -S ${SOURCE_DIR} -B ${BUILD_DIR}

cmake --build ${BUILD_DIR} -j --target sksl-wasm-lib-test sksl-wasm-golden-test

${BUILD_DIR}/src/sksl-wasm-lib-test
${BUILD_DIR}/golden/sksl-wasm-golden-test

cat <<EOT >${SOURCE_DIR}/.clangd
CompileFlags:
  CompilationDatabase: "${BUILD_DIR}"
Diagnostics:
  UnusedIncludes: Strict
EOT
