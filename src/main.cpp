#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "imu_service.h"
#include "gps_service.h"
#include "sd_logger.h"
#include "sms_alert.h"
#include "camera_bridge.h"
#include "detection_engine.h"

// Main ESP32 pin map. See docs/WIRING.md before connecting power.
constexpr int PIN_I2C_SDA = 21, PIN_I2C_SCL = 22;
constexpr int PIN_GPS_RX = 16, PIN_GPS_TX = 17;
constexpr int PIN_MODEM_RX = 26, PIN_MODEM_TX = 27;
constexpr int PIN_SD_CS = 5, PIN_SD_SCK = 18, PIN_SD_MISO = 19, PIN_SD_MOSI = 23;
constexpr int PIN_SOS = 34, PIN_BUZZER = 25;
HardwareSerial gpsSerial(1), modemSerial(2);
ImuService imu;
GpsService gps;
SdLogger eventLog;
SmsAlert sms(modemSerial);
CameraBridge camera;
DetectionEngine detector(SafeVelConfig::IMPACT_G_THRESHOLD, SafeVelConfig::HIGH_SEVERITY_G_THRESHOLD,
                         SafeVelConfig::GYRO_THRESHOLD_DPS, SafeVelConfig::ALERT_COOLDOWN_MS);
bool imuReady = false;
bool sosWasPressed = false;

void beep(unsigned int durationMs) { digitalWrite(PIN_BUZZER, HIGH); delay(durationMs); digitalWrite(PIN_BUZZER, LOW); }
void handleEvent(const DetectionResult& event) {
  const unsigned long now = millis();
  const String location = gps.locationText(now, SafeVelConfig::GPS_FIX_STALE_MS);
  const String cameraStatus = camera.capture();
  eventLog.logEvent(event, location, cameraStatus);
  beep(event.severity == Severity::High ? 700 : 200);
  sms.alertTrustedContacts(event.severity, location, gps.mapsUrl(now, SafeVelConfig::GPS_FIX_STALE_MS));
  Serial.printf("EVENT severity=%s impact=%.2f gyro=%.2f location=%s camera=%s\n",
                event.severity == Severity::High ? "high" : "low", event.impactG, event.peakGyroDps,
                location.c_str(), cameraStatus.c_str());
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUZZER, OUTPUT); digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_SOS, INPUT); // GPIO34 needs the external pull-down documented in WIRING.md.
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  gpsSerial.begin(9600, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);
  modemSerial.begin(9600, SERIAL_8N1, PIN_MODEM_RX, PIN_MODEM_TX);
  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  imuReady = imu.begin();
  eventLog.begin(PIN_SD_CS);
  sms.begin();
  if (SafeVelConfig::CAMERA_HTTP_ENABLED && strlen(SafeVelConfig::WIFI_SSID)) {
    WiFi.begin(SafeVelConfig::WIFI_SSID, SafeVelConfig::WIFI_PASSWORD);
  }
  Serial.printf("SafeVel boot: IMU=%s SD=%s SMS=%s (prototype only)\n", imuReady ? "ok" : "missing",
                "check serial log", SafeVelConfig::SMS_ENABLED ? "enabled" : "disabled");
  beep(80);
}

void loop() {
  gps.poll(gpsSerial);
  if (imuReady) {
    const DetectionResult detection = detector.evaluate(imu.read(millis()));
    if (detection.triggered) handleEvent(detection);
  }
  const bool sosPressed = digitalRead(PIN_SOS) == HIGH;
  if (sosPressed && !sosWasPressed) {
    DetectionResult sos{true, Severity::Low, 0.0f, 0.0f};
    handleEvent(sos);
  }
  sosWasPressed = sosPressed;
  delay(40);
}

