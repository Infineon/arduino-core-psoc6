#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$repo_root"

echo "[devcontainer] Running core setup..."
bash tools/dev-setup.sh

if command -v python3 >/dev/null 2>&1; then
  python3 --version
fi

echo "[devcontainer] Checking Arduino CLI..."
if ! command -v arduino-cli >/dev/null 2>&1; then
  echo "[devcontainer] WARNING: arduino-cli is not installed in the container."
  echo "[devcontainer] Install it inside the container to compile/upload sketches."
  exit 0
fi

arduino-cli version
sketchbook_dir="$(arduino-cli config get directories.user)"
arduino_git_dir="${sketchbook_dir}/hardware/arduino-git"
mkdir -p "$arduino_git_dir"
ln -sfn "$repo_root" "$arduino_git_dir/psoc6"
echo "[devcontainer] Linked $arduino_git_dir/psoc6 -> $repo_root"