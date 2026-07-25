#!/usr/bin/env bash
# Fast incremental recompile after editing source — skips the image build and
# submodule checks that docker/build.sh does. Requires docker/build.sh to have
# been run at least once already.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE="es-next-dev"

if [ ! -d "$REPO_ROOT/build" ]; then
  echo "No build/ directory found — run docker/build.sh first." >&2
  exit 1
fi

podman run --rm \
  --security-opt label=disable \
  -v "$REPO_ROOT:/src:Z" \
  -w /src/build \
  "$IMAGE" make -j"$(nproc)"

echo "Rebuild complete: $REPO_ROOT/emulationstation"
