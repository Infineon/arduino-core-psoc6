---
description: Hardware testing for Arduino Core PSoC6 (CY8CKIT-062S2-AI board)
applyTo: "**/tests/**,**/libraries/**"
---

# Hardware Testing Skill — Arduino Core PSoC6

## Hardware Environment

- **Board**: CY8CKIT-062S2-AI (Infineon PSoC™ 6)
- **FQBN**: `infineon:psoc6:cy8ckit_062s2_ai`
- **Debug Probe**: KitProg3 USB-UART (integrated on board)
- **Serial Port**: Detect via `Get-PnpDevice -Class Ports -Status OK` — look for "KitProg3 USB-UART"
- **Arduino CLI**: `arduino-cli` (verify with `arduino-cli version`)
- **OpenOCD**: Used for flashing via KitProg3
- **Installed Core Path**: `%LOCALAPPDATA%\Arduino15\packages\infineon\hardware\psoc6\1.0.0`

## On-Board Peripherals (No External Wiring Needed)

- **PDM Microphone**: MEMS mic on pins P10_4 (clock, Arduino pin 37) and P10_5 (data, Arduino pin 38)
- **IMU**: 6-axis
- **Magnetometer**: 3-axis
- **Pressure Sensor**: Digital
- **Radar Sensor**

## Compile → Flash → Monitor Workflow

```powershell
# 1. Compile
arduino-cli compile --fqbn infineon:psoc6:cy8ckit_062s2_ai "path\to\sketch.ino" --warnings all

# 2. Upload
arduino-cli upload --fqbn infineon:psoc6:cy8ckit_062s2_ai --port COM<N> "path\to\sketch.ino"

# 3. Monitor serial output (Python — use immediately after upload)
Start-Sleep -Seconds 1
python -c "
import serial, time, sys
ser = serial.Serial('COM<N>', 115200, timeout=1)
start = time.time()
output = ''
last_data_time = start
while time.time() - start < 60:
    if ser.in_waiting > 0:
        data = ser.read(ser.in_waiting).decode('utf-8', errors='replace')
        output += data
        sys.stdout.write(data)
        sys.stdout.flush()
        last_data_time = time.time()
        if 'ALL TESTS PASSED' in output or 'SOME TESTS FAILED' in output:
            time.sleep(1)
            if ser.in_waiting > 0:
                sys.stdout.write(ser.read(ser.in_waiting).decode('utf-8', errors='replace'))
            break
    else:
        if output and (time.time() - last_data_time > 20):
            print('[TIMEOUT]')
            break
        time.sleep(0.1)
ser.close()
"
```

**Important**: The board auto-resets after upload. Connect serial immediately after upload completes — do NOT use DTR toggle for reset (it doesn't work on this board).

## Testing New Libraries

If testing a library from a development branch that isn't in the installed core:

```powershell
# Copy library to installed core
Copy-Item -Path "libraries\<LibName>" -Destination "$env:LOCALAPPDATA\Arduino15\packages\infineon\hardware\psoc6\1.0.0\libraries\<LibName>" -Recurse -Force
```

## Test Sketch Pattern

Write test sketches that:
1. Use `Serial.begin(115200)` with a `delay(2000)` for connection setup
2. Print `Serial.flush()` after each test result line
3. Output `[PASS]` or `[FAIL]` markers per test
4. Print `*** ALL TESTS PASSED ***` or `*** SOME TESTS FAILED ***` at the end
5. Run all tests in `setup()`, leave `loop()` empty

## PDM Library — Known Facts

| Parameter | Valid Range | Notes |
|-----------|------------|-------|
| Channels | 1 (mono), 2 (stereo) | Mode: LEFT for mono, STEREO for 2ch |
| Sample Rate | 8000, 16000, 22050, 44100, 48000 Hz | All tested and working |
| Gain (setGain) | **-21 to +21** | ≥22 returns error, ≥23 **HANGS the MCU** |
| Buffer Size | ≥128 samples | Must call `setBufferSize()` BEFORE `begin()` |
| DMA Buffer | 128 samples (fixed) | Internal, not user-configurable |
| Word Length | 16-bit | Samples expanded to 32-bit in ISR |
| Audio Clock | 24.576 MHz PLL | Configured internally by `_clockInit()` |

### PDM Bug: setGain Hangs for Values ≥ 23

`cyhal_pdm_pcm_set_gain()` does not return for gain values ≥ 23. The library has no input validation. Gain 22 returns HAL error 0x04018201. Safe range is -21 to +21.

## Existing Test Infrastructure

- **Framework**: Unity (C unit tests on embedded hardware)
- **Test config**: `tests/test_config.h` (pin definitions)
- **HIL wiring**: `tests/cy8ckit-062s2-ai-hil-test-table.md`
- **Test runner**: `make` in `extras/arduino-core-tests/`
- **CI config**: `config/project.yml` (test matrix)

## Pin Reference

| Arduino Pin | PSoC Pin | Function |
|------------|----------|----------|
| 37 | P10_4 | PDM_MIC_CLK |
| 38 | P10_5 | PDM_MIC_DATA |
| A0 | - | Analog test (VREF) |
| A1 | - | Analog test (divider) |
| 4-7 | P9.4-P9.7 | Digital IO / Sync / Pulse |
