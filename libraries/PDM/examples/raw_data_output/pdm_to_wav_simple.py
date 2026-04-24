"""
Simple PDM Audio Serial to WAV Recorder

A simple version that reads raw binary audio data from serial
without sync pattern requirements. Good for quick testing.

This works with an Arduino sketch that just sends raw audio bytes
continuously over serial.

Usage:
    python pdm_to_wav_simple.py COM3 5 recording.wav

Where:
    COM3         = Serial port
    5            = Duration in seconds
    recording.wav = Output filename 
"""

import os
import sys
import time
import wave

try:
    import serial
except ImportError:
    print("Error: pyserial is required.")
    print("Install with: pip install pyserial")
    sys.exit(1)


# Configuration matching PDM library and Arduino sketch
BAUD_RATE = 1000000  # Must match Arduino sketch; 115200 is too slow for 16kHz/16-bit
SAMPLE_RATE = 16000  # Hz, matches PDM.begin(2, 16000)
CHANNELS = 2  # Stereo
BIT_DEPTH = 16  # bits
BYTES_PER_SAMPLE = 2  # 2 bytes (16 bits) per sample, with sign extension


def record_to_wav(port, duration_seconds, output_filename):
    """
    Record raw PCM audio data from a serial port and save it as a WAV file.

    Opens a serial connection at BAUD_RATE, reads raw little-endian 16-bit
    signed PCM samples streamed by the Arduino PDM sketch, and writes them
    into a standard WAV file. The recording can be stopped early with Ctrl+C.

    Args:
        port:             Serial port name (e.g. 'COM3' on Windows, '/dev/ttyACM0' on Linux).
        duration_seconds: Maximum recording duration in seconds.
        output_filename:  Path for the output .wav file.

    Raises:
        SystemExit: If a serial connection error occurs.
    """
    total_bytes = int(duration_seconds * SAMPLE_RATE * BYTES_PER_SAMPLE * CHANNELS)

    print(f"PDM to WAV Recorder")
    print(f"-------------------")
    print(f"Port:        {port}")
    print(f"Baud rate:   {BAUD_RATE}")
    print(f"Sample rate: {SAMPLE_RATE} Hz")
    print(f"Duration:    {duration_seconds} seconds")
    print(f"Output:      {output_filename}")
    print()

    try:
        ser_port = serial.Serial(port, BAUD_RATE, timeout=1)
        ser_port.reset_input_buffer()

        print("Recording... Press Ctrl+C to stop early.")

        audio_data = bytearray()
        start_time = time.time()

        while len(audio_data) < total_bytes:
            # Read available data
            bytes_needed = total_bytes - len(audio_data)
            chunk = ser_port.read(min(4096, bytes_needed))

            if chunk:
                audio_data.extend(chunk)

            # Progress
            elapsed = time.time() - start_time
            progress = min(100, len(audio_data) * 100 // total_bytes)
            samples = len(audio_data) // BYTES_PER_SAMPLE
            print(
                f"\rProgress: {progress}% | Samples: {samples} | Time: {elapsed:.1f}s",
                end="",
                flush=True,
            )

        ser_port.close()
        print("\n\nRecording complete!")

    except KeyboardInterrupt:
        print("\n\nRecording stopped by user.")
        ser_port.close()
    except serial.SerialException as e:
        print(f"\nSerial error: {e}")
        sys.exit(1)

    # Ensure even number of bytes (for 16-bit samples)
    if len(audio_data) % 2 == 1:
        audio_data = audio_data[:-1]

    # Save WAV file
    if audio_data:
        with wave.open(output_filename, "wb") as wav:
            wav.setnchannels(CHANNELS)
            wav.setsampwidth(BYTES_PER_SAMPLE)
            wav.setframerate(SAMPLE_RATE)
            wav.writeframes(bytes(audio_data))

        # Each frame contains CHANNELS samples of BYTES_PER_SAMPLE bytes each
        actual_duration = len(audio_data) / (BYTES_PER_SAMPLE * CHANNELS) / SAMPLE_RATE
        print(f"Saved: {output_filename}")
        print(f"  Duration: {actual_duration:.2f} seconds")
        print(f"  Samples:  {len(audio_data) // BYTES_PER_SAMPLE}")
    else:
        print("No audio data recorded.")


def main():
    """Parse CLI arguments and start recording.

    Expects at least a serial port argument. If no arguments are given,
    prints usage information and lists available serial ports.
    """
    if len(sys.argv) < 2:
        print(__doc__)
        print("\nAvailable serial ports:")
        try:
            import serial.tools.list_ports

            ports = list(serial.tools.list_ports.comports())
            for p in ports:
                print(f"  {p.device}: {p.description}")
            if not ports:
                print("  (none found)")
        except:
            print("  (unable to list)")
        sys.exit(1)

    port = sys.argv[1]
    duration = float(sys.argv[2]) if len(sys.argv) > 2 else 5.0
    output = sys.argv[3] if len(sys.argv) > 3 else "recording.wav"

    if os.path.exists(output):
        os.remove(output)
        print(f"Deleted existing file: {output}")

    record_to_wav(port, duration, output)


if __name__ == "__main__":
    main()
