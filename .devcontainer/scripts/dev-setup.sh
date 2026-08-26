#!/usr/bin/env bash
# Set up the development environment: run repo setup, configure Arduino, and link PSoC6 core
set -euo pipefail

repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$repo_root"

# Run the main repository setup script
echo "[devcontainer] Running core setup..."
bash tools/dev-setup.sh

arduino-cli version
# Link the repo into Arduino's hardware directory so it can be used as a local core
sketchbook_dir="$(arduino-cli config get directories.user)"
arduino_git_dir="${sketchbook_dir}/hardware/arduino-git"
mkdir -p "$arduino_git_dir"
ln -sfn "$repo_root" "$arduino_git_dir/psoc6"
echo "[devcontainer] Linked $arduino_git_dir/psoc6 -> $repo_root"