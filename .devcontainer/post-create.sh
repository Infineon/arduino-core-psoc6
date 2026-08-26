#!/usr/bin/env bash
# Post-create hook: runs all devcontainer setup scripts in order
set -euo pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"

# Initialize USB, and development environment
bash "$script_dir/scripts/usb-set-kitprog3.sh"
bash "$script_dir/scripts/usb-check.sh"
bash "$script_dir/scripts/dev-setup.sh"

echo "[devcontainer] Setup complete."
