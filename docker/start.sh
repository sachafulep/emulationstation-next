#!/usr/bin/env bash
# Launches the dev-built emulationstation binary inside the container with X11/GPU passthrough.
# Any extra arguments are passed straight through to emulationstation, e.g.:
#   docker/start.sh --home /src
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE="es-next-dev"
# The container is --rm'd on exit, so anything written under the container's
# HOME (/root/.emulationstation - settings, keybinds, downloaded themes) is
# normally lost. Bind-mount a host directory over it so it survives restarts.
ES_HOME_DIR="$REPO_ROOT/.emulationstation-home"

if [ ! -x "$REPO_ROOT/emulationstation" ]; then
  echo "emulationstation binary not found — run docker/build.sh first." >&2
  exit 1
fi

mkdir -p "$ES_HOME_DIR/.emulationstation"

xhost +SI:localuser:"$(whoami)" >/dev/null

podman run -it --rm \
  --security-opt label=disable \
  --group-add keep-groups \
  --device /dev/dri \
  -e DISPLAY="$DISPLAY" \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v "$REPO_ROOT:/src:Z" \
  -v "$ES_HOME_DIR/.emulationstation:/root/.emulationstation:Z" \
  -w /src \
  "$IMAGE" ./emulationstation --windowed --debug --resolution 480 320 "$@"
