# SafeVel — ESP32 accident-detection prototype firmware

SafeVel is a **prototype and educational project**, not a certified safety device, crash detector, emergency dispatch system, or substitute for human judgment. It reads an MPU6050, logs events to microSD, obtains NEO-6M GPS coordinates, can sound a buzzer, and can send an explicitly labeled SMS **only to configured trusted contacts**. It never contacts emergency services automatically.

## What is included

- ESP32/Arduino firmware with modular IMU, GPS, detection, SD logging, SMS, and camera-bridge code.
- A separate AI Thinker ESP32-CAM sketch for optional network-based JPEG capture.
- Wiring, power, setup, and safety notes; deterministic C++ detection tests and a manual hardware test plan.

## Architecture

```
MPU6050 ─I2C─> main ESP32 <─UART─ NEO-6M
                  ├─UART─ SIM800L ──SMS──> trusted contacts only
                  ├─SPI── microSD (CSV event log)
                  ├────── buzzer / SOS button
                  └─Wi-Fi HTTP (optional)──> separate ESP32-CAM ──> its own SD JPEGs
```

The main controller's defaults are deliberately safe: `SMS_ENABLED` and `CAMERA_HTTP_ENABLED` are both `false`, and trusted-contact slots are empty.

## Hardware

1. ESP32 DevKit (main controller)
2. MPU6050 breakout
3. NEO-6M GPS
4. SIM800L plus antenna, activated SIM, and a separate 3.7–4.2V / 2A-capable supply
5. 3.3V-compatible microSD SPI adapter and card
6. SOS button, resistor, and transistor-driven active buzzer
7. Optional: AI Thinker ESP32-CAM plus its own microSD card

Read [WIRING.md](docs/WIRING.md) before powering the setup. The SIM800L supply requirement is non-negotiable: an ESP32 regulator/USB lead will commonly brown out under GSM transmit bursts.

## Build and flash the main ESP32

This repository uses PlatformIO. Install [PlatformIO](https://platformio.org/install), connect the ESP32, then from this folder run:

```powershell
pio run
pio run -t upload
pio device monitor -b 115200
```

Alternatively, Arduino IDE users can create a project from `src/main.cpp`, install **Adafruit MPU6050**, **Adafruit Unified Sensor**, and **TinyGPSPlus**, select an ESP32 Dev Module, and preserve the `src/` headers alongside the sketch. PlatformIO is the tested project layout.

## Configure controlled SMS

Edit `src/config.h` before flashing:

```cpp
constexpr bool SMS_ENABLED = true;
constexpr const char* TRUSTED_CONTACTS[] = {"+919999999999", "", ""};
```

Only use numbers belonging to people who have agreed to receive the test alerts. Never add police, ambulance, or other emergency-service numbers. Each message is intentionally prefixed `SAFEVEL TEST/PROTOTYPE ALERT`. Leave the default `false` to disable all outbound SMS.

## Optional ESP32-CAM evidence node

Flash `camera_node/camera_node.ino` separately to an AI Thinker ESP32-CAM, set its Wi-Fi credentials, and confirm `/health` and `/capture` from a device on the same network. Then give the main ESP32 the same Wi-Fi credentials, set `CAMERA_HTTP_ENABLED = true`, and replace `CAMERA_CAPTURE_URL` with the camera node's fixed local address.

This is a clearly defined Wi-Fi/HTTP integration—not direct camera wiring to the main ESP32. The camera stores JPEGs on its own card; the main controller logs only capture status. See [WIRING.md](docs/WIRING.md) for constraints.

## Logging and thresholds

The main SD card receives `/safevel_events.csv` rows containing time since boot, severity, impact/gyro values, location (if fresh), and camera status. The detection algorithm is an intentionally simple threshold heuristic. It does not infer injuries, confirm a collision, distinguish potholes reliably, or make an emergency call. Adjust thresholds only after safe, recorded bench testing.

## Tests

Run logic tests with a host C++ compiler:

```powershell
powershell -ExecutionPolicy Bypass -File tests/run_host_tests.ps1
```

Then follow the full [hardware test plan](docs/TEST_PLAN.md). Firmware source has been structured to compile with PlatformIO, but no physical hardware build, SIM delivery, GPS fix, camera board, or vehicle installation has been verified in this environment.

## Publish to GitHub

```powershell
git init
git add .
git commit -m "Initial SafeVel ESP32 prototype firmware"
git branch -M main
git remote add origin https://github.com/YOUR-ACCOUNT/safevel-firmware.git
git push -u origin main
```

Create the empty GitHub repository first and replace `YOUR-ACCOUNT`. Review `src/config.h` before publishing: phone numbers and Wi-Fi passwords should not be committed. For private settings, keep them in a local copy excluded by `.gitignore` and merge them into your upload configuration manually.

## Known limitations

- SMS delivery is best effort; the current implementation does not parse delivery reports or retry failures.
- GPIO/power mappings are specific to the stated ESP32 DevKit and AI Thinker ESP32-CAM; different boards need review.
- There is no cloud service, authentication, encryption, OTA update, medical assessment, or autonomous dispatch.
- The SOS button sends on its rising edge; add debounce/hold-to-confirm behavior before any field deployment.

