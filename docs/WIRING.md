# Wiring and electrical safety

## Main ESP32

| Component | ESP32 pins | Notes |
|---|---|---|
| MPU6050 | GPIO21 SDA, GPIO22 SCL, 3.3V, GND | Use a 3.3V-safe breakout. I2C address normally `0x68`. |
| NEO-6M GPS | GPIO16 RX <- GPS TX; GPIO17 TX -> GPS RX; GND | Many GPS boards accept 3.3–5V supply; verify the exact board. UART is 9600 baud by default. |
| SIM800L | GPIO26 RX <- SIM800 TX; GPIO27 TX -> SIM800 RX; common GND | Do **not** power from the ESP32 3.3V rail. Use 3.7–4.2V supply rated for 2A bursts, 470–1000uF capacitor at module, and a logic-level shifter for ESP32 TX to SIM800 RX. |
| microSD (SPI) | CS GPIO5, SCK GPIO18, MISO GPIO19, MOSI GPIO23, 3.3V, GND | Use a 3.3V-compatible SD adapter. |
| SOS button | GPIO34 to 3.3V, 10k resistor GPIO34 to GND | GPIO34 is input-only and has no internal pull resistor. |
| Active buzzer | GPIO25 through transistor/driver, GND | Do not draw a high-current buzzer directly from a GPIO. |

All modules must share a common ground. Use a fused vehicle-rated DC/DC converter and test first from a current-limited bench supply. Vehicle power transients and loose wiring can destroy boards or create fire risk.

## Separate ESP32-CAM node

The ESP32-CAM is **not electrically integrated** with the main ESP32 in this project. It runs `camera_node/camera_node.ino` on its own AI Thinker ESP32-CAM, joins the same trusted Wi-Fi network, and exposes `GET /capture`. The main ESP32 requests that URL only when `CAMERA_HTTP_ENABLED` is enabled.

For the AI Thinker board, the sketch uses its standard OV2640 pin assignment and mounts its own microSD over `SD_MMC` in 1-bit mode. That microSD belongs to the camera board, not the main ESP32's SPI card. Camera board layouts differ; verify the module silkscreen/pinout and use a USB-to-TTL adapter for flashing. Do not assume its pins can be connected to the main ESP32 camera signals.

Camera capture is best-effort prototype evidence: the camera stores JPEGs locally and the main controller only records the HTTP reply in its event CSV. It does not copy images to the main SD card, authenticate the camera request, encrypt traffic, or upload anything.

