#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"

bash "$script_dir/scripts/git-setup.sh"
bash "$script_dir/scripts/usb-check.sh"
bash "$script_dir/scripts/dev-setup.sh"

echo "[devcontainer] Setup complete."
