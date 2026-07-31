#!/usr/bin/env sh
set -eu

ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT/build"
BUILD_TYPE="${BUILD_TYPE:-Release}"

echo "==> Configuring ($BUILD_TYPE)..."
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "==> Building..."
cmake --build "$BUILD_DIR"

echo "==> Running ProtoSeq..."
exec "$BUILD_DIR/ProtoSeq"
