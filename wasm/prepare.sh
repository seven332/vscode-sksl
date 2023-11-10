#!/usr/bin/env bash

set -eu

if ! command -v cmake &>/dev/null; then
    echo "cmake could not be found. Please install CMake."
    exit 1
fi

if ! command -v emcmake &>/dev/null; then
    echo "emcmake could not be found. Please install Emscripten."
    exit 1
fi
