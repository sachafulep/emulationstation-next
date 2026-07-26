#!/usr/bin/env bash
# Builds the dev container image and compiles emulationstation inside it.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE="es-next-dev"

podman build -t "$IMAGE" -f "$REPO_ROOT/docker/Dockerfile.dev" "$REPO_ROOT"

podman run --rm \
  --security-opt label=disable \
  -v "$REPO_ROOT:/src:Z" \
  "$IMAGE" bash -c '
    set -e
    git config --global --add safe.directory /src
    cd /src
    git submodule update --init --recursive
    mkdir -p build
    cd build
    cmake .. -DGL=1 -DCEC=0 -DROCKNIX=1
    make -j"$(nproc)"
  '

echo "Build complete: $REPO_ROOT/emulationstation"
