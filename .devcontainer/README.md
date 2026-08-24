# Dev Container Setup

This repository uses the default dev container configuration in `devcontainer.json`.
It runs with the project workspace mounted at `/myLocalWorkingDir`, the host `/dev`
 tree bound into the container, and privileged mode enabled so board tools and USB
 access can work from inside the container.

## How to use

1. In VS Code, run "Dev Containers: Reopen in Container" or "Open Folder in Container...".
2. Select the repository root and let the container build.
3. The `postCreateCommand` runs automatically and executes the repo setup scripts.

If you need to rerun the setup manually from the repository root:

```sh
bash .devcontainer/post-create.sh
```

You can also run the individual steps directly:

```sh
bash .devcontainer/scripts/git-setup.sh
bash .devcontainer/scripts/usb-check.sh
bash .devcontainer/scripts/dev-setup.sh
```

## What the setup does

The container bootstrap performs the following:

- `git-setup.sh`: verifies Git, adds the repo to the safe.directory list, imports SSH config/known_hosts when an agent is forwarded, and initializes submodules.
- `usb-check.sh`: checks whether serial devices and `/dev/bus/usb` are visible from inside the container.
- `dev-setup.sh`: runs `tools/dev-setup.sh`, checks for `arduino-cli`, and links the repo into the Arduino sketchbook hardware path so this checkout can be used as a local Arduino core.

Use `git-setup.sh` after changing Git credentials or submodule state, `usb-check.sh`
when attaching a board or troubleshooting serial/USB access, and `dev-setup.sh`
after changing the core, Arduino CLI, or sketchbook configuration.

## Notes

- The dev container uses the makers Docker image and includes Git support.
- SSH forwarding is optional; without a host SSH agent, HTTPS Git remotes still work.
- The default container is intentionally privileged and binds `/dev` to support USB serial devices and board programming workflows.
- If `/dev/bus/usb` or `/dev/ttyACM*` devices are not visible, verify the host exposes them and reopen the container after attaching the board.
- On WSL, attach boards with `usbipd.exe attach --wsl --busid <BUSID>` before reopening the container. On native Linux, ensure the host user can access the board and that the relevant USB devices are exposed to the container.
